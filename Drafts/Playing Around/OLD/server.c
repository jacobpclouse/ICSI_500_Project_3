#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define SERVER_PORT 7777
#define HELPER_PORT 8888

int main() {
    int serverSocket, helperSocket;
    struct sockaddr_in serverAddr, helperAddr;

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address struct
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    // Bind server socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // Set up helper address struct
    memset(&helperAddr, 0, sizeof(helperAddr));
    helperAddr.sin_family = AF_INET;
    helperAddr.sin_addr.s_addr = INADDR_ANY;
    helperAddr.sin_port = htons(HELPER_PORT);

    // Create helper socket
    helperSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (helperSocket == -1) {
        perror("Error creating helper socket");
        exit(EXIT_FAILURE);
    }

    // Main loop to handle client requests
    while (1) {
        char buffer[1024];
        struct sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);

        // Receive data from client
        ssize_t recvLen = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddr, &addrLen);

        if (recvLen == -1) {
            perror("Error receiving data");
            exit(EXIT_FAILURE);
        }

        // Send data to helper node
        if (sendto(helperSocket, buffer, recvLen, 0, (struct sockaddr *)&helperAddr, sizeof(helperAddr)) == -1) {
            perror("Error sending data to helper");
            exit(EXIT_FAILURE);
        }

        // Receive uppercase string from helper
        recvLen = recvfrom(helperSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&helperAddr, &addrLen);

        if (recvLen == -1) {
            perror("Error receiving data from helper");
            exit(EXIT_FAILURE);
        }

        // Send uppercase string back to client
        if (sendto(serverSocket, buffer, recvLen, 0, (struct sockaddr *)&clientAddr, addrLen) == -1) {
            perror("Error sending data to client");
            exit(EXIT_FAILURE);
        }
    }

    close(serverSocket);
    close(helperSocket);
    return 0;
}
