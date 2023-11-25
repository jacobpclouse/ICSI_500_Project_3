#include "encDec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define HELPER_PORT 8081
#define BUFFER_SIZE 2048

int main() {
    int helper_socket;
    struct sockaddr_in helper_addr, server_addr;
    socklen_t addr_size = sizeof(server_addr);

    // Create socket for the helper server
    helper_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (helper_socket == -1) {
        perror("Helper socket creation error");
        exit(EXIT_FAILURE);
    }

    // Configure helper server address
    helper_addr.sin_family = AF_INET;
    helper_addr.sin_port = htons(HELPER_PORT);
    helper_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the helper socket
    if (bind(helper_socket, (struct sockaddr*)&helper_addr, sizeof(helper_addr)) == -1) {
        perror("Helper bind error");
        exit(EXIT_FAILURE);
    }

    // Listen for connections from the main server
    if (listen(helper_socket, 1) == -1) {
        perror("Helper listen error");
        exit(EXIT_FAILURE);
    }

    printf("Helper server listening on port %d...\n", HELPER_PORT);

    // Accept a connection from the main server
    int server_socket = accept(helper_socket, (struct sockaddr*)&server_addr, &addr_size);
    if (server_socket == -1) {
        perror("Helper accept error");
        exit(EXIT_FAILURE);
    }

    printf("Connected to main server\n");

    // Main loop to handle communication with the main server
    while (1) {
        char message[BUFFER_SIZE];
        int bytes_received = recv(server_socket, message, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            printf("Main server disconnected\n");
            break;
        }

        // Convert the received message to uppercase
        for (int i = 0; i < bytes_received; i++) {
            message[i] = toupper(message[i]);
        }

        // Send the uppercase message back to the main server
        send(server_socket, message, bytes_received, 0);
    }

    // Close sockets
    close(server_socket);
    close(helper_socket);

    return 0;
}
