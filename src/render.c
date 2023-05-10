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

void build_table_name(char *html_response, char *path, char *root_path, char *file) {
    strcat(html_response, "<a href=\"");

    char *redirect = path_server_to_browser(path, root_path);
    strcat(html_response, redirect);

    strcat(html_response, "/");
    strcat(html_response, file);
    free(redirect);

    strcat(html_response, "\">");
    strcat(html_response, file);

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

int compare(const void *a, const void *b) {
    const char **ia = (const char **) a;
    const char **ib = (const char **) b;
    return strcmp(*ia, *ib);
}

char *build_table(DIR *d, char *path, char *root_path) {
    char *html_response = (char *) malloc(MAX_SIZE_BUFFER);
    strcpy(html_response, "<html><body><table><tr><th>Name</th><th>Size</th><th>Last Date</th></tr>");

    struct dirent *dir;
    struct stat st;
    int capacity = 32;
    int count = 0;
    char **files = (char **) malloc(capacity * sizeof(char *));

    while ((dir = readdir(d)) != NULL) {
        if (count == capacity) {
            capacity *= 2;
            files = realloc(files, capacity * sizeof(char *));
        }
        files[count] = malloc(strlen(dir->d_name) + 1);
        strcpy(files[count], dir->d_name);
        count++;
    }

    qsort(files, count, sizeof(char *), compare);

    for (int i = 0; i < count; i++) {
        char aux_path[strlen(path) + strlen(files[i]) + 1];

        strcpy(aux_path, path);
        strcat(aux_path, "/");
        strcat(aux_path, files[i]);

        stat(aux_path, &st);

        strcat(html_response, "<tr><th style=\"text-align: left;\">");

        build_table_name(html_response, path, root_path, files[i]);

        strcat(html_response, "</th><th>");

        build_table_size(html_response, st);

        strcat(html_response, "</th><th>");

        build_table_last_date(html_response, st);

        strcat(html_response, "</th></tr>");
    }

    strcat(html_response, "</table></body></html>");
    free(files);

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
