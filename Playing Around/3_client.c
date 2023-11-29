// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// 3_client.c
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
#include <time.h>

// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Functions
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// -- Function to create a text file to store the correspondence between the two clients
// I wanted this to be the two users, but i can't do that at this time
// source: https://stackoverflow.com/questions/5141960/get-the-current-time-in-c
char *createFileName(const char *username)
{
    time_t currentTime;
    struct tm *timeDataBoi;
    char *filenameBoi = (char *)malloc(64);

    if (filenameBoi == NULL)
    {
        perror("ERROR: issue with Mem. Allocation.\n");
        exit(1);
    }

    time(&currentTime);
    timeDataBoi = localtime(&currentTime);

    // source: https://stackoverflow.com/questions/5141960/get-the-current-time-in-c
    snprintf(filenameBoi, 64, "%s_%04d%02d%02d_%02d%02d%02d.txt",
             username,
             timeDataBoi->tm_year + 1900,
             timeDataBoi->tm_mon + 1,
             timeDataBoi->tm_mday,
             timeDataBoi->tm_hour,
             timeDataBoi->tm_min,
             timeDataBoi->tm_sec);

    return filenameBoi;
}

// -- Function used to update our .txt file conversation history immediatly after each message
// Source: https://www.geeksforgeeks.org/use-fflushstdin-c/
void writeMessageToFile(FILE *outputFile, const char *dataToWriteToFile)
{
    fprintf(outputFile, "%s", dataToWriteToFile);
    fflush(outputFile);
}

// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # MAIN
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
int main(int argc, char *argv[])
{

    // make sure that the user provides Correct data to start the server
    if (argc != 3)
    {
        printf("ERROR: Correct usage is as follows: %s IP address Port number\n", argv[0]);
        exit(1);
    }

    // grab ip address and port from args
    char *runServerOnThisIP = argv[1];
    int portno = atoi(argv[2]);

    // setup client socket and connection
    int client_socket;
    struct sockaddr_in server_addr;
    fd_set read_fds;

    // initalizing our message and sender vars
    char outgoingDatastream[BUFFER_SIZE];
    char nameOfClient[BUFFER_SIZE];

    // connection type + family
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portno);
    server_addr.sin_addr.s_addr = inet_addr(runServerOnThisIP);

    // try connect and catch if you can't
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("ERROR: Problems trying to connect!");
        exit(1);
    }

    // have them enter their username
    printf("Enter your name: ");
    fgets(nameOfClient, BUFFER_SIZE, stdin);
    nameOfClient[strcspn(nameOfClient, "\n")] = '\0';
    send(client_socket, nameOfClient, strlen(nameOfClient), 0);

    // create file for their chat history
    char *currentChatHistoryName = createFileName(nameOfClient);
    FILE *outputFile = fopen(currentChatHistoryName, "a");
    if (outputFile == NULL)
    {
        perror("ERROR: Can't open file!");
        exit(1);
    }

    // main communication loop
    while (1)
    {
        // setup file descriptors and read from them
        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
        FD_SET(client_socket, &read_fds);

        // select allows us to send data through the server
        select(client_socket + 1, &read_fds, NULL, NULL, NULL);

        if (FD_ISSET(0, &read_fds))
        {
             // get chats from current user
            fgets(outgoingDatastream, BUFFER_SIZE, stdin);
            send(client_socket, outgoingDatastream, strlen(outgoingDatastream), 0); // sends them to the server

            // update chat file
            writeMessageToFile(outputFile, outgoingDatastream);
        }
        else if (FD_ISSET(client_socket, &read_fds))
        {
            // recieve data from server
            memset(outgoingDatastream, 0, BUFFER_SIZE);
            int bytes_received = recv(client_socket, outgoingDatastream, BUFFER_SIZE, 0);
            if (bytes_received <= 0)
            {
                printf("Server disconnected\n"); // if server disconnects, we hit this
                break;
            }
            printf("%s", outgoingDatastream);

            // update chat file
            writeMessageToFile(outputFile, outgoingDatastream); 
        }
    }

    // close chat file
    fclose(outputFile);
    free(currentChatHistoryName);
    // close client socket
    close(client_socket);

    return 0;
}
