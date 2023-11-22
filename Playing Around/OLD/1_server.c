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

void send_client_list(int current_client) {
    char client_list[BUFFER_SIZE];
    memset(client_list, 0, BUFFER_SIZE);

    for (int i = 0; i < client_count; i++) {
        strcat(client_list, clients[i].name);
        strcat(client_list, "\n");
    }

    char message[BUFFER_SIZE];
    snprintf(message, BUFFER_SIZE, "Logged in clients:\n%s", client_list);

    send(current_client, message, strlen(message), 0);
}

void broadcast(char *message, int current_client, char *sender_name) {
    char formatted_message[BUFFER_SIZE];
    snprintf(formatted_message, BUFFER_SIZE, "[%s]=> %s", sender_name, message);

    for (int i = 0; i < client_count; i++) {
        if (clients[i].socket != current_client) {
            send(clients[i].socket, formatted_message, strlen(formatted_message), 0);
        }
    }
}

void notify_new_client(char *sender_name) {
    char new_client_message[BUFFER_SIZE];
    snprintf(new_client_message, BUFFER_SIZE, "New client joined: %s\n", sender_name);

    for (int i = 0; i < client_count; i++) {
        send(clients[i].socket, new_client_message, strlen(new_client_message), 0);
    }
}

void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    char message[BUFFER_SIZE];
    char sender_name[BUFFER_SIZE];
    int bytes_received;

    // Receive the sender's name from the client
    bytes_received = recv(client_socket, sender_name, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
        close(client_socket);
        pthread_exit(NULL);
    }

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

    while (1) {
        bytes_received = recv(client_socket, message, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            // Client disconnected
            close(client_socket);

            for (int i = 0; i < client_count; i++) {
                if (clients[i].socket == client_socket) {
                    for (int j = i; j < client_count - 1; j++) {
                        clients[j] = clients[j + 1];
                    }
                    client_count--;
                }
            }

            pthread_exit(NULL);
        }

        message[bytes_received] = '\0';
        broadcast(message, client_socket, sender_name);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
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

    addr_size = sizeof(client_addr);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);

        if (client_count < MAX_CLIENTS) {
            pthread_create(&tid, NULL, handle_client, &client_socket);
        } else {
            printf("Chatroom is full. Try again later.\n");
            close(client_socket);
        }
    }

    return 0;
}
