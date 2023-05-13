//
// Created by raudel on 5/9/2023.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_SIZE_BUFFER 1024
#define TOK_DELIM " \t\r\n\a"

char *url_to_path(char *path) {
    char *decoded_str;

    decoded_str = malloc(strlen(path) + 1);
    strcpy(decoded_str, path);
    char *pos = decoded_str;
    while ((pos = strstr(pos, "%")) != NULL) {
        char hex[3];
        strncpy(hex, pos + 1, 2);
        hex[2] = '\0';
        *pos = (char) strtol(hex, NULL, 16);
        memmove(pos + 1, pos + 3, strlen(pos + 3) + 1);
    }

    return decoded_str;
}

char *path_browser_to_server(char *path, char *root_path) {
    char *url_path = url_to_path(path);

    int len = (int) strlen(url_path);
    int len_root_path = (int) strlen(root_path);

    char *new_path = (char *) malloc(MAX_SIZE_BUFFER);
    strcpy(new_path, root_path);
    strcat(new_path, url_path);

    if (new_path[len + len_root_path - 1] == '/') new_path[len + len_root_path - 1] = 0;

    free(url_path);
    return new_path;
}

char *path_server_to_browser(char *path, char *root_path) {
    int i;
    int len = (int) strlen(path);
    int len_root_path = (int) strlen(root_path);

    char *new_path = (char *) malloc(MAX_SIZE_BUFFER);

    for (i = 0; i < len - len_root_path; i++) {
        new_path[i] = path[i + len_root_path];
    }
    new_path[i] = 0;

    return new_path;
}


char **split_line(char *line) {
    int buf_size = MAX_SIZE_BUFFER, position = 0;
    char **tokens = malloc(buf_size * sizeof(char *));
    char *token;

    token = strtok(line, TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= buf_size) {
            buf_size += MAX_SIZE_BUFFER;
            tokens = realloc(tokens, buf_size * sizeof(char *));
            if (!tokens) {
                fprintf(stderr, "allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}
