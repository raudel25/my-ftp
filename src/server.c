//
// Created by raudel on 5/5/2023.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <errno.h>

#include "render.h"
#include "utils.h"
#include "server.h"

#define MAX_SIZE_BUFFER 1024
#define TOK_DELIM " \t\r\n\a"
#define ERROR "\033[0;31mmy_ftp\033[0m"
#define HTTP_NOT_FOUND "HTTP/1.1 404 Not Found\r\n\r\n"
#define HTTP_BAD_REQUEST "HTTP/1.1 400 Bad Request\r\n\r\n"
#define HTTP_INTERNAL_ERROR "HTTP/1.1 500 Internal Server Error\r\n\r\n"
#define HTTP_FORBIDDEN "HTTP/1.1 403 Forbidden\r\n\r\n"

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

/**
 * @brief Send a directory to the client, 403 if the directory is not accessible or 500 if an error occurred
 * @param path Path to the directory
 * @param sock_client Socket to the client
 * @param root_path Path to the root directory
 * @return
 * 1 if was sent a response to the client\n
 * 0 if the directory was not found\n
 * @note The response is sent using the HTTP protocol
 */
int navigate(char *path, int sock_client, char *root_path) {
    DIR *dir;
    dir = opendir(path);
    if (!dir) {
        if (errno == EACCES) {
            send(sock_client, HTTP_FORBIDDEN, strlen(HTTP_FORBIDDEN), 0);
            printf("%s: denied access to %s\n", ERROR, path);
            return 1;
        }
        closedir(dir);
        return 0;
    }

    char *response = render(dir, path, root_path);
    if (send(sock_client, response, strlen(response), 0) == -1) {
        fprintf(stderr, "%s: send failed\n", ERROR);
        send(sock_client, HTTP_INTERNAL_ERROR, strlen(HTTP_INTERNAL_ERROR), 0);
        return 1;
    }
    free(response);
    closedir(dir);
    return 1;
}

/**
 * @brief Send a file to the client or 500 if an error occurred
 * @param path Path to the file
 * @param sock_client Socket to the client
 * @return 0 if the file was not found\n
 * 1 if was sent a response to the client
 * @note The file is sent using the HTTP protocol
 */
int send_file(char *path, int sock_client) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        return 0;
    }

    off_t offset = 0;
    struct stat stat_buf;
    if (fstat(fd, &stat_buf) == -1) {
        fprintf(stderr, "%s: fstat failed\n", ERROR);
        return 1;
    }

    char header[MAX_SIZE_BUFFER];
    snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\n"
                                     "Content-Type: application/octet-stream\r\n"
                                     "Content-Disposition: attachment; filename=\"%s\"\r\n"
                                     "Content-Length: %ld\r\n"
                                     "\r\n", path, stat_buf.st_size);

    if (send(sock_client, header, strlen(header), 0) == -1
            || sendfile(sock_client, fd, &offset, stat_buf.st_size) == -1) {
        fprintf(stderr, "%s: send failed\n", ERROR);
        send(sock_client, HTTP_INTERNAL_ERROR, strlen(HTTP_INTERNAL_ERROR), 0);
    }

    close(fd);
    return 1;
}

void *handle_client(void *arg) {
    struct Client client = *(struct Client *) arg;
    int sock_client = client.sock_client;
    char *root_path = client.root_path;

    char buffer[MAX_SIZE_BUFFER];

    if (recv(sock_client, buffer, MAX_SIZE_BUFFER, 0) == -1) {
        fprintf(stderr, "%s: recv failed\n", ERROR);
        char *response = HTTP_INTERNAL_ERROR;
        send(sock_client, response, strlen(response), 0);
        exit(1);
    }

    char **args = split_line(buffer, TOK_DELIM);

    if (args[0] != NULL && strcmp(args[0], "GET") == 0 && args[1] != NULL) {
        char *path = path_browser_to_server(args[1], root_path);

        if (!navigate(path, sock_client, root_path) && !send_file(path, sock_client)) {
            char *response = HTTP_NOT_FOUND;
            send(sock_client, response, strlen(response), 0);
        }
        free(path);
    } else {
        char *response = HTTP_BAD_REQUEST;
        send(sock_client, response, strlen(response), 0);
    }

    close(sock_client);

    free(args);
    free(client.root_path);

    return NULL;
}