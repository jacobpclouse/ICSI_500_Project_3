#include "encDec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <time.h>

#define BUFFER_SIZE 2048

char* createFileName(const char* username) {
    time_t current_time;
    struct tm* time_info;
    char* filename = (char*)malloc(64); // Adjust the size as needed

    if (filename == NULL) {
        perror("Memory allocation error");
        exit(1);
    }

    time(&current_time);
    time_info = localtime(&current_time);

    snprintf(filename, 64, "%s_%04d%02d%02d_%02d%02d%02d.txt",
        username,
        time_info->tm_year + 1900,
        time_info->tm_mon + 1,
        time_info->tm_mday,
        time_info->tm_hour,
        time_info->tm_min,
        time_info->tm_sec);

    return filename;
}

void writeMessageToFile(FILE* outputFile, const char* message) {
    fprintf(outputFile, "%s", message);
    fflush(outputFile); // Ensure the data is written immediately
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Correct usage: %s IP address Port number\n", argv[0]);
        exit(1);
    }

    char *IP_address = argv[1];
    int Port = atoi(argv[2]);

    int client_socket;
    struct sockaddr_in server_addr;
    fd_set read_fds;
    char message[BUFFER_SIZE];
    char sender_name[BUFFER_SIZE];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(Port);
    server_addr.sin_addr.s_addr = inet_addr(IP_address);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection error");
        exit(1);
    }

    // Prompt the user for their name and send it to the server
    printf("Enter your name: ");
    fgets(sender_name, BUFFER_SIZE, stdin);
    sender_name[strcspn(sender_name, "\n")] = '\0'; // Remove the newline character
    send(client_socket, sender_name, strlen(sender_name), 0);

    // Create a filename for the text file
    char* filename = createFileName(sender_name);

    // Open the file for appending (a) instead of writing (w)
    FILE* outputFile = fopen(filename, "a");
    if (outputFile == NULL) {
        perror("Error opening file");
        exit(1);
    }

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
        FD_SET(client_socket, &read_fds);

        select(client_socket + 1, &read_fds, NULL, NULL, NULL);

        if (FD_ISSET(0, &read_fds)) {
            // User input
            fgets(message, BUFFER_SIZE, stdin);
            send(client_socket, message, strlen(message), 0);
            
            // Write the user's input to the file
            writeMessageToFile(outputFile, message);
        } else if (FD_ISSET(client_socket, &read_fds)) {
            // Message from server
            memset(message, 0, BUFFER_SIZE);
            int bytes_received = recv(client_socket, message, BUFFER_SIZE, 0);
            if (bytes_received <= 0) {
                printf("Server disconnected\n");
                break;
            }
            printf("%s", message);

            // Write the received message to the file
            writeMessageToFile(outputFile, message);
        }
    }

    // Close the file
    fclose(outputFile);
    free(filename);

    close(client_socket);

    return 0;
}
