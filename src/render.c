//
// Created by raudel on 5/5/2023.
//
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <pwd.h>

#include "utils.h"

#define MAX_SIZE_BUFFER 4096
#define HTTP_OK "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"

char *build_table(char *path, char *root_path) {
    char *html_response = (char *) malloc(MAX_SIZE_BUFFER);
    strcpy(html_response, "<html><body><table><tr><th>Name</th><th>Size</th></tr>");

    DIR *d;
    struct dirent *dir;
    d = opendir(path);

    while ((dir = readdir(d)) != NULL) {

        strcat(html_response, "<tr><th><a href=\"");

        char *redirect = path_server_to_browser(path, root_path);
        strcat(html_response, redirect);

        strcat(html_response, "/");
        strcat(html_response, dir->d_name);
        free(redirect);

        strcat(html_response, "\">");
        strcat(html_response, dir->d_name);

        strcat(html_response, "</a></th><th>");

        char aux[64];
        if (dir->d_ino / 1024 > 0) {
            sprintf(aux, "%ld", dir->d_ino / 1024);
            strcat(html_response, aux);
            strcat(html_response, " mb");
        } else {
            sprintf(aux, "%ld", dir->d_ino);
            strcat(html_response, aux);
            strcat(html_response, " kb");
        }

        strcat(html_response, "</th></tr>");
    }
    closedir(d);


    strcat(html_response, "</table></body></html>");

    return html_response;
}

char *render(char *path, char *root_path) {

    char *html_response = build_table(path, root_path);
    char *http_header = HTTP_OK;
    char *response = malloc(strlen(http_header) + strlen(html_response) + 1);

    strcpy(response, http_header);
    strcat(response, html_response);
    free(html_response);

    return response;
}
