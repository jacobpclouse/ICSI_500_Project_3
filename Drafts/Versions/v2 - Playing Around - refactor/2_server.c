// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// 2_server.c
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Importing Libraries / Modules / Headers
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#include "encDec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <time.h>

// constants for the server / client
#define MAXIMUM_CONNECTED_CLIENTS 6
#define BUFFER_SIZE 2048

// here we have our info for our helper server
#define HELPER_SERVER_IP "127.0.0.1"
#define HELPER_SERVER_PORT 1996
int socketForHelperServer;

// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Structs -- move to header if can
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// Structure to model client data on
// Referenced code here: https://www.geeksforgeeks.org/structures-c/#
struct organizedClientData
{
    int socket;
    char name[BUFFER_SIZE];
};

// creating struct
struct organizedClientData clients[MAXIMUM_CONNECTED_CLIENTS];
int client_count = 0;

// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Functions
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// -- Function to send out list of already logged in clients to new client --
// Source: https://stackoverflow.com/questions/41104762/sent-a-message-to-all-clients-with-c-programming
void outputConnectedClients(int selectedClient)
{

    char listOfClients[BUFFER_SIZE];       // Create a buffer to store the list of clients
    memset(listOfClients, 0, BUFFER_SIZE); // Initialize the buffer with zeros

    // Loop through each logged-in client
    for (int i = 0; i < client_count; i++)
    {
        // Concatenate the name of each client to the listOfClients buffer, followed by a newline
        strcat(listOfClients, clients[i].name);
        strcat(listOfClients, "\n");
    }

    char communicationToSend[BUFFER_SIZE];                                                                                               // Create a buffer to store the final message to be sent
    snprintf(communicationToSend, BUFFER_SIZE, "-=-=-=-=-=-=-=-=-\nCurrently logged in clients:\n%s-=-=-=-=-=-=-=-=-\n", listOfClients); // Format the message with the list of clients
    send(selectedClient, communicationToSend, strlen(communicationToSend), 0);                                                           // Send the message to the specified client
}

// -- Function to send out data to all connected clients
// Source: https://stackoverflow.com/questions/41104762/sent-a-message-to-all-clients-with-c-programming
void sendPublicMessage(char *inputData, int selectedClient, char *senderName)
{

    char outboundMessage[BUFFER_SIZE];                                         // Create a buffer to store the formatted message
    snprintf(outboundMessage, BUFFER_SIZE, "[%s]=> %s", senderName, inputData); // Format the message with the sender's name

    // Iterate through all connected clients
    for (int i = 0; i < client_count; i++)
    {
        // Check if the current client is not the one specified in the parameter
        if (clients[i].socket != selectedClient)
        {
            // Send the formatted message to the current client using its socket
            send(clients[i].socket, outboundMessage, strlen(outboundMessage), 0);
        }
    }
}



// -- Function to let everyone know when a new client joins the server --
void newClientJoinedPush(char *senderName)
{
    // Create a buffer to store the new client message
    char outboundPushNotification[BUFFER_SIZE];

    // Format the message indicating the new client's name
    snprintf(outboundPushNotification, BUFFER_SIZE, "<*> New client joined: %s <*>\n", senderName);

    // Iterate through all connected clients
    for (int i = 0; i < client_count; i++)
    {
        // Send the new client message to each client's socket
        send(clients[i].socket, outboundPushNotification, strlen(outboundPushNotification), 0);
    }
}

// ****
// Function to handle communication with the helper server
void talkWithHelperServer(char *dataSentToHelper, char *returnedDataFromHelper)
{
    send(socketForHelperServer, dataSentToHelper, strlen(dataSentToHelper), 0);
    recv(socketForHelperServer, returnedDataFromHelper, BUFFER_SIZE, 0);
}
// **


// Function that handles full communication with multiple clients *****
void *clientThreadSplitFunction(void *arg)
{
    // Extract the client socket from the argument
    int client_socket = *((int *)arg);

    // Buffer to store messages and sender names
    char message[BUFFER_SIZE];
    char sender_name[BUFFER_SIZE];
    int bytes_received;

    // Receive the sender's name from the client
    bytes_received = recv(client_socket, sender_name, BUFFER_SIZE, 0);
    if (bytes_received <= 0)
    {
        // If unable to receive, close the socket and exit the thread
        close(client_socket);
        pthread_exit(NULL);
    }

    // Null-terminate the sender's name
    sender_name[bytes_received] = '\0';

    // Store the client information in the array
    struct organizedClientData new_client;
    new_client.socket = client_socket;
    strncpy(new_client.name, sender_name, BUFFER_SIZE);

    clients[client_count] = new_client;
    client_count++;

    // Send the client list to the new client
    outputConnectedClients(client_socket);

    // Notify all clients about the new client
    newClientJoinedPush(sender_name);

    // Main loop to handle client messages
    while (1)
    {
        // Receive a message from the client
        bytes_received = recv(client_socket, message, BUFFER_SIZE, 0);
        if (bytes_received <= 0)
        {
            // If client disconnected, close the socket and remove from the array
            close(client_socket);

            // Find the client in the array and remove it
            for (int i = 0; i < client_count; i++)
            {
                if (clients[i].socket == client_socket)
                {
                    for (int j = i; j < client_count - 1; j++)
                    {
                        clients[j] = clients[j + 1];
                    }
                    client_count--;
                }
            }

            // Exit the thread
            pthread_exit(NULL);
        }
        // Null-terminate the received message
        message[bytes_received] = '\0';

        // -----
        // Send the message to the helper server for processing
        char result[BUFFER_SIZE];
        talkWithHelperServer(message, result);

        // send result to all clients
        sendPublicMessage(result, client_socket, sender_name);
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
        printf("Correct usage: %s IP address Port number\n", argv[0]);
        exit(1);
    }

    char *IP_address = argv[1];
    int Port = atoi(argv[2]);

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    pthread_t tid;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(Port);
    server_addr.sin_addr.s_addr = inet_addr(IP_address);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind error");
        exit(1);
    }

    if (listen(server_socket, 100) == 0)
    {
        printf("Listening on IP: %s on Port: %d ...\n", IP_address, Port);
    }
    else
    {
        perror("Listen error");
        exit(1);
    }
    // --- main above, helper stuff below
    // Create a socket for the helper server
    socketForHelperServer = socket(AF_INET, SOCK_STREAM, 0);
    if (socketForHelperServer == -1)
    {
        perror("Helper server socket creation error");
        exit(EXIT_FAILURE);
    }

    // Configure the helper server address
    struct sockaddr_in helper_server_addr;
    helper_server_addr.sin_family = AF_INET;
    helper_server_addr.sin_port = htons(HELPER_SERVER_PORT);
    if (inet_pton(AF_INET, HELPER_SERVER_IP, &helper_server_addr.sin_addr) <= 0)
    {
        perror("Helper server address conversion error");
        exit(EXIT_FAILURE);
    }

    // Connect to the helper server
    if (connect(socketForHelperServer, (struct sockaddr *)&helper_server_addr, sizeof(helper_server_addr)) < 0)
    {
        perror("Helper server connection error");
        exit(EXIT_FAILURE);
    }

    printf("Connected to helper server\n");

    // ----
    // Set the size of the client address structure
    addr_size = sizeof(client_addr);

    // Main loop to accept incoming client connections
    while (1)
    {
        // Accept a client connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);

        // Check if the chatroom is not full
        if (client_count < MAXIMUM_CONNECTED_CLIENTS)
        {
            // Create a new thread to handle the client
            pthread_create(&tid, NULL, clientThreadSplitFunction, &client_socket);
            // create another new thread just for the helper stuff right here??
            // or maybe in the clientThreadSplitFunction function???
        }
        else
        {
            // Inform the client that the chatroom is full and close the connection
            printf("Max number of participants reached, can't connect now.\n");
            close(client_socket); // close client socket
        }
    }

    return 0;
}
