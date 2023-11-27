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

char dataToConvert[30];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # Functions
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// -- Function to convert a specific vowel to uppercase --
void *convertVowelToUppercase(void *arg) {
    char vowel = *((char *)arg);

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < strlen(dataToConvert); i++) {
        if (dataToConvert[i] == vowel || dataToConvert[i] == (vowel - 32)) {
            dataToConvert[i] = dataToConvert[i] - 32; // Convert to uppercase
        }
    }
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// # MAIN
// # =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
int main() {
    pthread_t threads[NUM_THREADS];
    char vowels[NUM_THREADS] = {'a', 'e', 'i', 'o', 'u'};

    // Create threads for each vowel
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, convertVowelToUppercase, (void *)&vowels[i]);
    }

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
        // Reset the dataToConvert array for each new message
        memset(dataToConvert, 0, sizeof(dataToConvert));

        char message[BUFFER_SIZE];
        int bytes_received = recv(server_socket, message, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            connectionTerminated();
            break;
        } else {
            printf("Data request received! :D\n");

            // Convert vowels to uppercase using threads
            for (int i = 0; i < NUM_THREADS; i++) {
                pthread_join(threads[i], NULL);
            }

            printf("Done Recieving.. \n\n");
        }

        // Send the uppercase message back to the main server
        send(server_socket, dataToConvert, bytes_received, 0);
    }

    // Join threads and close sockets for cleanup
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    close(server_socket);
    close(helper_socket);

    return 0;
}
