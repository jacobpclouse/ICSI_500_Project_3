// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// 2_server.c
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Importing Libraries / Modules / Headers
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#include "encDec.h" // all imports in header

// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Global Variables
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
int socketForHelperServer;

// creating struct for clients
struct organizedClientData clients[MAXIMUM_CONNECTED_CLIENTS];
int currentClientCount = 0;

// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Functions
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// -- Function to send out list of already logged in clients to new client --
// Source: https://stackoverflow.com/questions/41104762/sent-a-message-to-all-clients-with-c-programming
void outputConnectedClients(int selectedClient)
{
    // store client list
    char listOfClients[BUFFER_SIZE];
    memset(listOfClients, 0, BUFFER_SIZE);

    // run through all clients
    for (int i = 0; i < currentClientCount; i++)
    {
        // grab all names merge together for output
        strcat(listOfClients, clients[i].name);
        strcat(listOfClients, "\n");
    }

    // setup output buffer and banner
    char communicationToSend[BUFFER_SIZE];                                                                                               // Create a buffer to store the final message to be sent
    snprintf(communicationToSend, BUFFER_SIZE, "-=-=-=-=-=-=-=-=-\nCurrently logged in clients:\n%s-=-=-=-=-=-=-=-=-\n", listOfClients); // Format the message with the list of clients
    send(selectedClient, communicationToSend, strlen(communicationToSend), 0);                                                           // Send the message to the specified client
}

// -- Function to send out data to all connected clients
// Source: https://stackoverflow.com/questions/41104762/sent-a-message-to-all-clients-with-c-programming
void sendPublicMessage(char *inputData, int selectedClient, char *senderName)
{

    char outboundMessage[BUFFER_SIZE];                                          // Create a buffer to store the formatted message
    snprintf(outboundMessage, BUFFER_SIZE, "[%s]=> %s", senderName, inputData); // Format the message with the sender's name

    // run through all clients
    for (int i = 0; i < currentClientCount; i++)
    {
        // only send message to client if socket matches specific client
        if (clients[i].socket != selectedClient)
        {
            send(clients[i].socket, outboundMessage, strlen(outboundMessage), 0);
        }
    }
}

// -- Function to let everyone know when a new client joins the server --
// Source: https://stackoverflow.com/questions/41104762/sent-a-message-to-all-clients-with-c-programming
void newClientJoinedPush(char *senderName)
{
    // buffer for new client msg + grab sender name
    char outboundPushNotification[BUFFER_SIZE];
    snprintf(outboundPushNotification, BUFFER_SIZE, "<*> New client joined: %s <*>\n", senderName);

    // send to all connected clients
    for (int i = 0; i < currentClientCount; i++)
    {
        send(clients[i].socket, outboundPushNotification, strlen(outboundPushNotification), 0);
    }
}

// **** NEW TO THIS PROJECT! ****
// -- Function to handle communication with the helper server --
void talkWithHelperServer(char *dataSentToHelper, char *returnedDataFromHelper)
{
    // send and recieve with sockets
    send(socketForHelperServer, dataSentToHelper, strlen(dataSentToHelper), 0);
    recv(socketForHelperServer, returnedDataFromHelper, BUFFER_SIZE, 0);
}
// **

//  -- Function that handles full communication with multiple clients -- THREADING OH LORD*****
// Source: https://www.geeksforgeeks.org/multithreading-in-c/
void *clientThreadSplitFunction(void *arg)
{
    // grab client id, initalize vars and store important id data
    int clientSocketID = *((int *)arg);
    char dataWeWantToSend[BUFFER_SIZE];
    char origSender[BUFFER_SIZE];
    int dataBytesIncoming;

    // grab sender name, exit if can't get it
    dataBytesIncoming = recv(clientSocketID, origSender, BUFFER_SIZE, 0);
    if (dataBytesIncoming <= 0)
    {
        close(clientSocketID);
        pthread_exit(NULL);
    }

    origSender[dataBytesIncoming] = '\0';

    // init struct, grab client info & store it
    struct organizedClientData newBoi;
    newBoi.socket = clientSocketID;
    strncpy(newBoi.name, origSender, BUFFER_SIZE);

    clients[currentClientCount] = newBoi;
    currentClientCount++;

    // send out client list
    outputConnectedClients(clientSocketID);

    // send push that new client joined
    newClientJoinedPush(origSender);

    // main communication loop
    while (1)
    {
        // intake client messages
        dataBytesIncoming = recv(clientSocketID, dataWeWantToSend, BUFFER_SIZE, 0);
        if (dataBytesIncoming <= 0)
        {
            // when client disconnects, close socket and then loop through to find client id and remove it from array
            close(clientSocketID);
            // printf("Client %d left!",clientSocketID);

            for (int i = 0; i < currentClientCount; i++)
            {
                if (clients[i].socket == clientSocketID)
                {
                    for (int j = i; j < currentClientCount - 1; j++)
                    {
                        clients[j] = clients[j + 1];
                    }
                    currentClientCount--;
                }
            }

            // order 66 the thread
            pthread_exit(NULL);
        }
        dataWeWantToSend[dataBytesIncoming] = '\0';

        // -----

        // send the message to the helper server for processing
        char outputBufferBoi[BUFFER_SIZE];
        talkWithHelperServer(dataWeWantToSend, outputBufferBoi);

        // send result to all clients
        sendPublicMessage(outputBufferBoi, clientSocketID, origSender);

        // clear out result
        memset(outputBufferBoi, 0, BUFFER_SIZE);
        // result[dataBytesIncoming] = '\0'; // ?? this should work
    }

    return NULL;
}

// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # MAIN
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
int main(int argc, char *argv[])
{
    // Check if the correct number of command-line arguments is provided
    if (argc != 3)
    {
        printf("ERROR: Correct usage is as follows: %s IP address Port number\n", argv[0]);
        exit(1);
    }

    char *runServerOnThisIP = argv[1];
    int portno = atoi(argv[2]);

    // setup server and client socket + connections
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    pthread_t tid;

    // setup for binding/connection type
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portno);
    server_addr.sin_addr.s_addr = inet_addr(runServerOnThisIP);

    // bind socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("ERROR: Issue with binding!");
        exit(1);
    }

    // start listening on, error if issue
    if (listen(server_socket, 100) == 0)
    {
        myLogo(); // startup print outs
        printf("2_server listening on IP: %s on Port: %d ...\n", runServerOnThisIP, portno);
    }
    else
    {
        perror("ERROR: Issue with listening!");
        exit(1);
    }
    // --- main above, helper stuff below
    // Create a socket for the helper server
    socketForHelperServer = socket(AF_INET, SOCK_STREAM, 0);
    if (socketForHelperServer == -1)
    {
        perror("ERROR: Helper server socket creation error!");
        exit(1);
    }

    // Configure the helper server address
    struct sockaddr_in helper_server_addr;
    helper_server_addr.sin_family = AF_INET;
    helper_server_addr.sin_port = htons(HELPER_SERVER_PORT);
    if (inet_pton(AF_INET, HELPER_SERVER_IP, &helper_server_addr.sin_addr) <= 0)
    {
        perror("ERROR: Helper server address conversion error");
        exit(1);
    }

    // Connect to the helper server
    if (connect(socketForHelperServer, (struct sockaddr *)&helper_server_addr, sizeof(helper_server_addr)) < 0)
    {
        perror("ERROR: Helper server connection error");
        exit(1);
    }

    // printf("Connected to helper server\n");
    serverConnectedToHelper();

    // ----
    addr_size = sizeof(client_addr);

    // main communication loop
    while (1)
    {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size); // accept connenctions
        if (currentClientCount < MAXIMUM_CONNECTED_CLIENTS)                                 // make sure not full
        {
            // new thread created for each client
            pthread_create(&tid, NULL, clientThreadSplitFunction, &client_socket);
            // create another new thread just for the helper stuff right here??
            // or maybe in the clientThreadSplitFunction function???
        }
        else
        {
            // print if max num of client reached!
            printf("ERROR: Max num of clients reach, try again later.\n");
            close(client_socket); // close client socket
        }
    }

    return 0;
}
