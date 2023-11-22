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

#define MAX_CLIENTS 6
#define BUFFER_SIZE 2048

struct Client {
    int socket;
    char name[BUFFER_SIZE];
};

struct Client clients[MAX_CLIENTS];
int client_count = 0;

// Function to send the list of logged-in clients to a specified client
void send_client_list(int current_client) {
    char client_list[BUFFER_SIZE]; // Create a buffer to store the list of clients
    memset(client_list, 0, BUFFER_SIZE); // Initialize the buffer with zeros

    // Loop through each logged-in client
    for (int i = 0; i < client_count; i++) {
        // Concatenate the name of each client to the client_list buffer, followed by a newline
        strcat(client_list, clients[i].name);
        strcat(client_list, "\n");
    }

    char message[BUFFER_SIZE];// Create a buffer to store the final message to be sent
    snprintf(message, BUFFER_SIZE, "Logged in clients:\n%s", client_list);// Format the message with the list of clients
    send(current_client, message, strlen(message), 0);// Send the message to the specified client
}

// Function to broadcast a message to all connected clients, except the current client
void broadcast(char *message, int current_client, char *sender_name) {
    
    char formatted_message[BUFFER_SIZE];// Create a buffer to store the formatted message
    snprintf(formatted_message, BUFFER_SIZE, "[%s]=> %s", sender_name, message); // Format the message with the sender's name

    // Iterate through all connected clients
    for (int i = 0; i < client_count; i++) {
        // Check if the current client is not the one specified in the parameter
        if (clients[i].socket != current_client) {
            // Send the formatted message to the current client using its socket
            send(clients[i].socket, formatted_message, strlen(formatted_message), 0);
        }
    }
}

// Function to notify all existing clients about a new client
void notify_new_client(char *sender_name) {
    // Create a buffer to store the new client message
    char new_client_message[BUFFER_SIZE];

    // Format the message indicating the new client's name
    snprintf(new_client_message, BUFFER_SIZE, "New client joined: %s\n", sender_name);

    // Iterate through all connected clients
    for (int i = 0; i < client_count; i++) {
        // Send the new client message to each client's socket
        send(clients[i].socket, new_client_message, strlen(new_client_message), 0);
    }
}

// Function that handles full communication with multiple clients
void *handle_client(void *arg) {
    // Extract the client socket from the argument
    int client_socket = *((int *)arg);

    // Buffer to store messages and sender names
    char message[BUFFER_SIZE];
    char sender_name[BUFFER_SIZE];
    int bytes_received;

    // Receive the sender's name from the client
    bytes_received = recv(client_socket, sender_name, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
        // If unable to receive, close the socket and exit the thread
        close(client_socket);
        pthread_exit(NULL);
    }

    // Null-terminate the sender's name
    sender_name[bytes_received] = '\0';

    // Store the client information in the array
    struct Client new_client;
    new_client.socket = client_socket;
    strncpy(new_client.name, sender_name, BUFFER_SIZE);

    clients[client_count] = new_client;
    client_count++;

    // Send the client list to the new client
    send_client_list(client_socket);

    // Notify all clients about the new client
    notify_new_client(sender_name);

    // Main loop to handle client messages
    while (1) {
        // Receive a message from the client
        bytes_received = recv(client_socket, message, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            // If client disconnected, close the socket and remove from the array
            close(client_socket);

            // Find the client in the array and remove it
            for (int i = 0; i < client_count; i++) {
                if (clients[i].socket == client_socket) {
                    for (int j = i; j < client_count - 1; j++) {
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

        // Broadcast the message to all clients
        broadcast(message, client_socket, sender_name);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    // Check if the correct number of command-line arguments is provided
    if (argc != 3) {
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

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind error");
        exit(1);
    }

    if (listen(server_socket, 100) == 0) {
        printf("Listening on IP: %s on Port: %d ...\n", IP_address, Port);
    } else {
        perror("Listen error");
        exit(1);
    }

    // Set the size of the client address structure
    addr_size = sizeof(client_addr);

    // Main loop to accept incoming client connections
    while (1) {
        // Accept a client connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);

        // Check if the chatroom is not full
        if (client_count < MAX_CLIENTS) {
            // Create a new thread to handle the client
            pthread_create(&tid, NULL, handle_client, &client_socket);
            // create another new thread just for the helper stuff right here??
            // or maybe in the handle_client function???

            
        } else {
            // Inform the client that the chatroom is full and close the connection
            printf("Chatroom is full. Try again later.\n");
            close(client_socket);
        }
    }

    return 0;
}
