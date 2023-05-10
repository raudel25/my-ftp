//
// Created by raudel on 5/5/2023.
//
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/stat.h>
#include <time.h>

#include "utils.h"

#define MAX_SIZE_BUFFER 10240
#define HTTP_HTML "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"

void build_table_name(char *html_response, char *path, char *root_path, struct dirent *dir) {
    strcat(html_response, "<a href=\"");

    char *redirect = path_server_to_browser(path, root_path);
    strcat(html_response, redirect);

    strcat(html_response, "/");
    strcat(html_response, dir->d_name);
    free(redirect);

    strcat(html_response, "\">");
    strcat(html_response, dir->d_name);

    strcat(html_response, "</a>");
}

void build_table_size(char *html_response, struct stat st) {
    char aux[64];
    if (!S_ISDIR(st.st_mode)) {
        unsigned long t = st.st_size / 1024;
        char *type;

        if (t > 1024) {
            t /= 1024;
            type = " mb";

            if (t > 1024) {
                t /= 1024;
                type = " gb";
            }
        } else {
            type = " kb";
        }

        sprintf(aux, "%ld", t);
        strcat(html_response, aux
        );
        strcat(html_response, type
        );
    } else {
        strcat(html_response,
               "-");
    }
}

void build_table_last_date(char *html_response, struct stat st) {
    struct tm *tm;

    tm = localtime(&st.st_mtime);

    char aux[64];
    sprintf(aux, "%d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec);

    strcat(html_response, aux);
}

char *build_table(DIR *d, char *path, char *root_path) {
    char *html_response = (char *) malloc(MAX_SIZE_BUFFER);
    strcpy(html_response, "<html><body><table><tr><th>Name</th><th>Size</th><th>Last Date</th></tr>");

    struct dirent *dir;
    struct stat st;

    while ((dir = readdir(d)) != NULL) {
        char aux_path[strlen(path) + strlen(dir->d_name) + 1];
        strcpy(aux_path, path);
        strcat(aux_path, "/");
        strcat(aux_path, dir->d_name);

        stat(aux_path, &st);

        strcat(html_response, "<tr><th>");

        build_table_name(html_response, path, root_path, dir);

        strcat(html_response, "</th><th>");

        build_table_size(html_response, st);

        strcat(html_response, "</th><th>");

        build_table_last_date(html_response, st);

        strcat(html_response, "</th></tr>");
    }

    strcat(html_response, "</table></body></html>");

    return html_response;
}

char *render(DIR *d,char *path, char *root_path) {

    char *html_response = build_table(d,path, root_path);
    char *http_header = HTTP_HTML;
    char *response = malloc(MAX_SIZE_BUFFER);

    strcpy(response, http_header);
    strcat(response, html_response);
    free(html_response);

    return response;
}
