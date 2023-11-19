// helper_node.c
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8081

int main() {
    int socket_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Creating socket file descriptor
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Forcefully attaching socket to the port
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        return -1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port
    if (bind(socket_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        return -1;
    }
    if (listen(socket_fd, 3) < 0) {
        perror("listen");
        return -1;
    }

    if ((new_socket = accept(socket_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        return -1;
    }

    valread = read(new_socket, buffer, sizeof(buffer));
    
    // Convert to uppercase
    for (int i = 0; i < valread; i++) {
        buffer[i] = toupper(buffer[i]);
    }

    send(new_socket, buffer, strlen(buffer), 0);
    printf("Processed string: %s\n", buffer);
    return 0;
}
