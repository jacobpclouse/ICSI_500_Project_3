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
#define NUM_THREADS 5
// #define HELPER_SERVER_IP "127.0.0.1" // can't use this right now

char sharedBuffer[BUFFER_SIZE];
pthread_mutex_t mutex;

// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Structs -- move to header if can
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// -- Struct to pass data to each thread for uppercasing --
typedef struct {
    int thread_id;
} ThreadData;

// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Functions
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// Thread function
void *threadFunction(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    // Lock the mutex to access the shared buffer
    pthread_mutex_lock(&mutex);

    // Perform transformations based on the thread ID
    switch (data->thread_id) {
        case 1:
            for (int i = 0; i < BUFFER_SIZE && sharedBuffer[i] != '\0'; ++i) {
                if (sharedBuffer[i] == 'a') {
                    sharedBuffer[i] = 'A';
                }
            }
            break;
        case 2:
            for (int i = 0; i < BUFFER_SIZE && sharedBuffer[i] != '\0'; ++i) {
                if (sharedBuffer[i] == 'e') {
                    sharedBuffer[i] = 'E';
                }
            }
            break;
        case 3:
            for (int i = 0; i < BUFFER_SIZE && sharedBuffer[i] != '\0'; ++i) {
                if (sharedBuffer[i] == 'o') {
                    sharedBuffer[i] = 'O';
                }
            }
            break;
        case 4:
            for (int i = 0; i < BUFFER_SIZE && sharedBuffer[i] != '\0'; ++i) {
                if (sharedBuffer[i] == 'i') {
                    sharedBuffer[i] = 'I';
                }
            }
            break;
        case 5:
            for (int i = 0; i < BUFFER_SIZE && sharedBuffer[i] != '\0'; ++i) {
                if (sharedBuffer[i] == 'u') {
                    sharedBuffer[i] = 'U';
                }
            }
            break;
        default:
            break;
    }

    // Unlock the mutex
    pthread_mutex_unlock(&mutex);

    // Print the result
    printf("Thread %d: Transformed string: %s\n", data->thread_id, sharedBuffer);

    // Pass the transformed string to the next thread
    if (data->thread_id < 5) {
        ThreadData nextThreadData = {data->thread_id + 1};
        pthread_t nextThread;
        pthread_create(&nextThread, NULL, threadFunction, &nextThreadData);
        pthread_join(nextThread, NULL);
    }

    pthread_exit(NULL);
}


// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # MAIN
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
int main() {
    int helper_socket;
    struct sockaddr_in helper_addr, server_addr;
    socklen_t addr_size = sizeof(server_addr);

    // Create socket for the helper server
    helper_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (helper_socket == -1) {
        perror("ERROR: Failed to create helper socket!");
        exit(EXIT_FAILURE);
    }

    // Configure helper server address
    helper_addr.sin_family = AF_INET;
    helper_addr.sin_port = htons(HELPER_PORT);
    // helper_addr.sin_addr.s_addr = HELPER_SERVER_IP;
    helper_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the helper socket
    if (bind(helper_socket, (struct sockaddr*)&helper_addr, sizeof(helper_addr)) == -1) {
        perror("ERROR: Issue with binding!");
        exit(EXIT_FAILURE);
    }

    // Listen for connections from the main server
    if (listen(helper_socket, 1) == -1) {
        perror("ERROR: Issue with listening!");
        exit(EXIT_FAILURE);
    }

    myLogo(); // startup print outs
    printf("1_helper node listening on IP: %s on Port %d...\n", INADDR_ANY, HELPER_PORT);
    

    // Accept a connection from the main server
    int server_socket = accept(helper_socket, (struct sockaddr*)&server_addr, &addr_size);
    if (server_socket == -1) {
        perror("ERROR: Experienced issue accepting connection!");
        exit(EXIT_FAILURE);
    }

    serverConnected(); // shows main 2_server has connected

    // Main loop to handle communication with the main server
    while (1) {
        char message[BUFFER_SIZE];
        int bytes_received = recv(server_socket, message, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            connectionTerminated();
            break;
        } else {
            printf("Data request recieved! :D\n");
        }

        pthread_mutex_init(&mutex, NULL);// initialize 

        // MAYBE COPY MESSAGE INTO THE SERVER STUFF

        // Lock the mutex to access the shared buffer
        pthread_mutex_lock(&mutex);

            // Copy the server data to the shared buffer
        strncpy(sharedBuffer, message, BUFFER_SIZE - 1);
        sharedBuffer[BUFFER_SIZE - 1] = '\0';

        // Unlock the mutex
        pthread_mutex_unlock(&mutex);

        // Create the first thread
        ThreadData firstThreadData = {1};
        pthread_t firstThread;
        pthread_create(&firstThread, NULL, threadFunction, &firstThreadData);
        pthread_join(firstThread, NULL);

        // Destroy mutex
        pthread_mutex_destroy(&mutex);

        // Send the uppercase message back to the main server
        send(server_socket, sharedBuffer, bytes_received, 0);


        // // Convert the received message to uppercase
        // for (int i = 0; i < bytes_received; i++) {
        //     message[i] = toupper(message[i]);
        // }

        // Send the uppercase message back to the main server
        // send(server_socket, message, bytes_received, 0);
    }

    // close sockets for cleanup
    close(server_socket);
    close(helper_socket);

    return 0;
}
