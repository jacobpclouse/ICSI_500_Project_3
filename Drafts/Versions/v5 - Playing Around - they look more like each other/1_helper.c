// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// 1_helper.c
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
#include <pthread.h> // threading for the aieou stuff

// constants
#define HELPER_PORT 1996
#define BUFFER_SIZE 2048
// #define NUM_THREADS 5
// #define HELPER_SERVER_IP "127.0.0.1" // can't use this right now

char uppercasedBuffer[BUFFER_SIZE]; // to store thread data
pthread_mutex_t mutex;

// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Structs -- move to header if can
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// -- Struct to pass data to each thread for uppercasing --
typedef struct
{
    int thread_id;
} TheadsForAIEOU;

// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Functions
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// -- Function to create threads to break up and uppercase the input string
// Source: https://www.tutorialspoint.com/cprogramming/switch_statement_in_c.htm
// Source 2: https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/
void *toUpperThreadingFunc(void *arg)
{
    TheadsForAIEOU *inputDataStreamToUppercase = (TheadsForAIEOU *)arg;

    pthread_mutex_lock(&mutex); // lock the mutex so we can access the shared buffer

    // we uppercase based on thread id, pass data between them
    // Source: https://www.tutorialspoint.com/cprogramming/switch_statement_in_c.htm
    switch (inputDataStreamToUppercase->thread_id)
    {
    case 1:
        for (int i = 0; i < BUFFER_SIZE && uppercasedBuffer[i] != '\0'; ++i)
        {
            if (uppercasedBuffer[i] == 'a')
            {
                uppercasedBuffer[i] = 'A';
            }
        }
        // printf("charA thread output string: %s\n", uppercasedBuffer);

        break;
    case 2:
        for (int i = 0; i < BUFFER_SIZE && uppercasedBuffer[i] != '\0'; ++i)
        {
            if (uppercasedBuffer[i] == 'e')
            {
                uppercasedBuffer[i] = 'E';
            }
        }

        // printf("charE thread output string: %s\n", uppercasedBuffer);
        break;
    case 3:
        for (int i = 0; i < BUFFER_SIZE && uppercasedBuffer[i] != '\0'; ++i)
        {
            if (uppercasedBuffer[i] == 'o')
            {
                uppercasedBuffer[i] = 'O';
            }
        }

        // printf("charO thread output string: %s\n", uppercasedBuffer);
        break;
    case 4:
        for (int i = 0; i < BUFFER_SIZE && uppercasedBuffer[i] != '\0'; ++i)
        {
            if (uppercasedBuffer[i] == 'i')
            {
                uppercasedBuffer[i] = 'I';
            }
        }

        // printf("charI thread output string: %s\n", uppercasedBuffer);
        break;
    case 5:
        for (int i = 0; i < BUFFER_SIZE && uppercasedBuffer[i] != '\0'; ++i)
        {
            if (uppercasedBuffer[i] == 'u')
            {
                uppercasedBuffer[i] = 'U';
            }
        }

        // printf("charU thread output string: %s\n", uppercasedBuffer);
        break;
    default:
        break;
    }

    pthread_mutex_unlock(&mutex); // unlock
    printf("Thread %d: %s\n", inputDataStreamToUppercase->thread_id, uppercasedBuffer);

    // iterate through and pass data from string to string
    if (inputDataStreamToUppercase->thread_id < 5) // queue here
    {
        TheadsForAIEOU nextThreadData = {inputDataStreamToUppercase->thread_id + 1};
        pthread_t nextThread;
        pthread_create(&nextThread, NULL, toUpperThreadingFunc, &nextThreadData);
        pthread_join(nextThread, NULL);
    }

    pthread_exit(NULL);
}

// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # MAIN
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
int main()
{
    int helperSocket;
    struct sockaddr_in helper_addr, server_addr;
    socklen_t addr_size = sizeof(server_addr);

    // create socket, configure connection type
    helperSocket = socket(AF_INET, SOCK_STREAM, 0);
    helper_addr.sin_family = AF_INET;
    helper_addr.sin_port = htons(HELPER_PORT);
    // helper_addr.sin_addr.s_addr = HELPER_SERVER_IP;
    helper_addr.sin_addr.s_addr = INADDR_ANY;

    // bind socket
    if (bind(helperSocket, (struct sockaddr *)&helper_addr, sizeof(helper_addr)) == -1)
    {
        perror("ERROR: Issue with binding!");
        exit(1);
    }

    // listen for connections
    if (listen(helperSocket, 1) == -1)
    {
        perror("ERROR: Issue with listening!");
        exit(1);
    }

    myLogo(); // startup print outs
    printf("1_helper node listening on IP: %s on Port %d...\n", INADDR_ANY, HELPER_PORT);

    // accept incoming connections
    int server_socket = accept(helperSocket, (struct sockaddr *)&server_addr, &addr_size);
    if (server_socket == -1)
    {
        perror("ERROR: Experienced issue accepting connection!");
        exit(1);
    }

    serverConnected(); // shows main 2_server has connected

    // main communication loop
    while (1)
    {
        char datastreamFromMainServer[BUFFER_SIZE];
        int bytes_received = recv(server_socket, datastreamFromMainServer, BUFFER_SIZE, 0);
        if (bytes_received <= 0)
        {
            connectionTerminated();
            break;
        }
        else
        {
            printf("\nData request recieved! Remember Thread# 1 = A, 2 = E, 3 = I, 4 = O, 5 = U\n");
        }

        // -----
        // USE THREADING TO UPPERCASE DATA - Start!
        // -----

        pthread_mutex_init(&mutex, NULL); // initialize


        // Lock the mutex to access the shared buffer
        pthread_mutex_lock(&mutex);

        // Copy the server data to the shared buffer
        strncpy(uppercasedBuffer, datastreamFromMainServer, BUFFER_SIZE - 1);
        uppercasedBuffer[BUFFER_SIZE - 1] = '\0';

        // Unlock the mutex
        pthread_mutex_unlock(&mutex);

        // Create the first thread
        TheadsForAIEOU firstThreadData = {1};
        pthread_t firstThread;
        pthread_create(&firstThread, NULL, toUpperThreadingFunc, &firstThreadData);
        pthread_join(firstThread, NULL);

        // Destroy mutex
        pthread_mutex_destroy(&mutex);

        // -----
        // USE THREADING TO UPPERCASE DATA - End!
        // -----

        // Send the uppercase datastreamFromMainServer back to the main server
        send(server_socket, uppercasedBuffer, bytes_received, 0);


    }

    // close sockets for cleanup
    close(server_socket);
    close(helperSocket);

    return 0;
}
