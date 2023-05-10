//
// Created by raudel on 5/9/2023.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_SIZE_BUFFER 1024
#define TOK_DELIM " \t\r\n\a"

char *spaces(char *path) {
    int len = (int) strlen(path);
    char *new_path = (char *) malloc(MAX_SIZE_BUFFER);
    int j = 0;

    for (int i = 0; i < len; i++) {
        if (i <= len - 3 && path[i] == '%' && path[i + 1] == '2' && path[i + 2] == '0') {
            new_path[j++] = ' ';
            i += 2;
            continue;
        }

        new_path[j++] = path[i];
    }
    new_path[j] = 0;

    return new_path;
}

char *path_browser_to_server(char *path, char *root_path) {
    char *spaces_path = spaces(path);

    int len = (int) strlen(spaces_path);
    int len_root_path = (int) strlen(root_path);

    char *new_path = (char *) malloc(MAX_SIZE_BUFFER);
    strcpy(new_path, root_path);
    strcat(new_path, spaces_path);

    if (new_path[len + len_root_path - 1] == '/') new_path[len + len_root_path - 1] = 0;

    free(spaces_path);
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
