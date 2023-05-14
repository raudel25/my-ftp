//
// Created by raudel on 5/9/2023.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>

#define MAX_SIZE_BUFFER 1024

char **split_line(char *line, char *split) {
    int buf_size = MAX_SIZE_BUFFER, position = 0;
    char **tokens = malloc(buf_size * sizeof(char *));
    char *token;

    token = strtok(line, split);
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

        token = strtok(NULL, split);
    }
    tokens[position] = NULL;
    return tokens;
}

void back_path(char *path) {
    int len = (int) strlen(path);
    if (len >= 3 && path[len - 1] == '.' && path[len - 2] == '.' && path[len - 3] == '/') {
        int ind = len - 4;

        while (path[ind] != '/') {
            ind--;
            if (ind == -1) break;
        }

        path[ind + 1] = 0;
    }

    if (len >= 2 && path[len - 1] == '.' && path[len - 2] == '/') path[len - 2] = 0;
}

char *path_to_url(char *path) {
    char **args = split_line(path, "/");

    char *url = (char *) malloc(MAX_SIZE_BUFFER);
    url[0] = 0;

    if (args[0] == NULL) strcat(url, "/");

    for (int i = 0; args[i] != NULL; i++) {
        char *output;

        CURL *curl = curl_easy_init();

        output = curl_easy_escape(curl, args[i], (int) strlen(args[i]));

        strcat(url, "/");
        strcat(url, output);
        curl_easy_cleanup(curl);
        free(output);
    }

    free(args);

    return url;
}

char *url_to_path(char *path) {
    char *output;
    CURL *curl = curl_easy_init();

    int output_length;
    output = curl_easy_unescape(curl, path, (int) strlen(path), &output_length);
    curl_easy_cleanup(curl);

    return output;
}

char *path_browser_to_server(char *path, char *root_path) {
    char *url_path = url_to_path(path);

    int len = (int) strlen(url_path);
    int len_root_path = (int) strlen(root_path);

    char *new_path = (char *) malloc(len_root_path + len + 1);
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

    char *new_path = (char *) malloc(len + 1);

    for (i = 0; i < len - len_root_path; i++) {
        new_path[i] = path[i + len_root_path];
    }
    new_path[i] = 0;

    return new_path;
}