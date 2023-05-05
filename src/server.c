//
// Created by raude on 5/5/2023.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

#include "render.h"

struct sockaddr_in build_server_addr(char *server_ip, int server_port) {
    // Construir la dirección del servidor
    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;            // Familia de direcciones
    server.sin_port = htons(server_port);   // Puerto del servidor
    inet_aton(server_ip, &server.sin_addr); // Dirección IP del servidor
    return server;
}


int create_server(int port) {
    struct sockaddr_in server;
    int sock1;

    sock1 = socket(AF_INET, SOCK_STREAM, 0);

    if (sock1 == -1) {
        printf("Socket creation failed");
        exit(1);
    }

    setsockopt(sock1, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));
    server = build_server_addr("localhost", port);

    if (bind(sock1, (struct sockaddr *) &server, sizeof(server)) == -1) {
        printf("Binding error");
        exit(1);
    }

    if (listen(sock1, 1) == -1) {
        printf("Listen failed");
        exit(1);
    }

    return sock1;
}

void handle_client(int sock, char *path) {
    char *response;
    response = render(path);
    send(sock, response, strlen(response), 0);
    free(response);

    char buffer[1024];

    while (1) {
        recv(sock, buffer, 1024, 0);

        fflush(0);
        printf("%s", buffer);
    }
}