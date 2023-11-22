#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>

#define HELPER_PORT 8888

void uppercaseString(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

int main() {
    int helperSocket, clientSocket;
    struct sockaddr_in helperAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    // Create socket
    helperSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (helperSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up helper address struct
    memset(&helperAddr, 0, sizeof(helperAddr));
    helperAddr.sin_family = AF_INET;
    helperAddr.sin_addr.s_addr = INADDR_ANY;
    helperAddr.sin_port = htons(HELPER_PORT);

    // Bind socket
    if (bind(helperSocket, (struct sockaddr *)&helperAddr, sizeof(helperAddr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    printf("Helper Node is running.\n");

    while (1) {
        char buffer[1024];
        ssize_t recvLen = recvfrom(helperSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddr, &addrLen);

        if (recvLen == -1) {
            perror("Error receiving data");
            exit(EXIT_FAILURE);
        }

        // Process and uppercase the string
        uppercaseString(buffer);

        // Send the uppercase string back to the server
        if (sendto(helperSocket, buffer, strlen(buffer), 0, (struct sockaddr *)&clientAddr, addrLen) == -1) {
            perror("Error sending data");
            exit(EXIT_FAILURE);
        }
    }

    close(helperSocket);
    return 0;
}
