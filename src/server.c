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
#include <dirent.h>
#include <unistd.h>

#include "render.h"
#include "utils.h"

#define MAX_SIZE_BUFFER 1024
#define TOK_DELIM " \t\r\n\a"
#define ERROR "\033[0;31mmy_ftp\033[0m"
#define HTTP_NOT_FOUND "HTTP/1.1 404 Not Found\r\n\r\n"

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
        fprintf(stderr, "%s: socket creation failed\n", ERROR);
        exit(1);
    }

    setsockopt(sock1, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));
    server = build_server_addr("localhost", port);

    if (bind(sock1, (struct sockaddr *) &server, sizeof(server)) == -1) {
        fprintf(stderr, "%s: binding error\n", ERROR);
        exit(1);
    }

    if (listen(sock1, 1) == -1) {
        fprintf(stderr, "%s: listen failed\n", ERROR);
        exit(1);
    }

    return sock1;
}

int navigate(char *path, int sock_client) {
    DIR *d;
    d = opendir(path);

    if (d) {
        char *response = render(path, root_path);

        send(sock_client, response, strlen(response), 0);
        free(response);

        return 1;
    }
    closedir(d);

    return 0;
}

int send_file(char *path, int sock_client) {
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        return 0;
    }

    fseek(fp, 0L, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);

    char header[MAX_SIZE_BUFFER];
    snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\n"
                                     "Content-Type: application/octet-stream\r\n"
                                     "Content-Disposition: attachment; filename=\"%s\"\r\n"
                                     "Content-Length: %ld\r\n"
                                     "\r\n", path, file_size);

    send(sock_client, header, strlen(header), 0);

    char buffer[MAX_SIZE_BUFFER];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        send(sock_client, buffer, bytes_read, 0);
    }

    fclose(fp);

    return 1;
}

void *handle_client(void *arg) {
    int sock = *(int *) arg;

    char buffer[MAX_SIZE_BUFFER];

    struct sockaddr_in client;
    int len = sizeof(client);
    int sock_client = accept(sock, (struct sockaddr *) &client, (socklen_t *) &len);

    int not_found = 0;

    recv(sock_client, buffer, MAX_SIZE_BUFFER, 0);

    char **args = split_line(buffer);

    if (strcmp(args[0], "GET") == 0 && args[1] != NULL) {

        char *path = path_browser_to_server(args[1], root_path);

        int action = navigate(path, sock_client);
        action = action || send_file(path, sock_client);

        if (!action) not_found = 1;

        free(path);
    } else {
        not_found = 1;
    }

    if (not_found) {
        char *response = HTTP_NOT_FOUND;
        send(sock_client, response, strlen(response), 0);
    }

    close(sock_client);

    free(args);

    pthread_exit(NULL);
}