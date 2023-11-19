#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 7777

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;

    // Create client socket
    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address struct
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(SERVER_PORT);

    // Get user input
    char buffer[1024];
    printf("Enter a string: ");
    fgets(buffer, sizeof(buffer), stdin);
    strtok(buffer, "\n"); // Remove the newline character

    // Send the string to the server
    if (sendto(clientSocket, buffer, strlen(buffer), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error sending data");
        exit(EXIT_FAILURE);
    }

    // Receive and print the uppercase string from the server
    ssize_t recvLen = recvfrom(clientSocket, buffer, sizeof(buffer), 0, NULL, NULL);

    if (recvLen == -1) {
        perror("Error receiving data");
        exit(EXIT_FAILURE);
    }

    printf("Uppercase string from server: %s\n", buffer);

    close(clientSocket);
    return 0;
}
