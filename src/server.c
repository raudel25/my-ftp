//
// Created by raudel on 5/5/2023.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

#include "render.h"
#include "utils.h"

#define TOK_BUF_SIZE 1024
#define TOK_DELIM " \t\r\n\a"

char *root_path = NULL;

struct sockaddr_in build_server_addr(char *server_ip, int server_port) {
    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    inet_aton(server_ip, &server.sin_addr);
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

void *handle_client(void *arg) {
    int sock = *(int *) arg;

    char *response;
    char buffer[TOK_BUF_SIZE];

    struct sockaddr_in client;
    int len = sizeof(client);
    int sock_client = accept(sock, (struct sockaddr *) &client, (socklen_t *) &len);

    recv(sock_client, buffer, TOK_BUF_SIZE, 0);
    fflush(0);
    printf("%s", buffer);

    char **args = split_line(buffer);

    if (strcmp(args[0], "GET") == 0 && args[1] != NULL) {

        char *path = path_browser_to_server(args[1], root_path);
        response = render(path, root_path);

        send(sock_client, response, strlen(response), 0);
        free(response);
        free(path);
    }

    free(args);

    pthread_exit(NULL);
}