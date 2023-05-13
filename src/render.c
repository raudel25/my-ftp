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

#define MAX_SIZE_BUFFER 1024
#define HTTP_HTML "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"

void build_table_name(char *html_response, char *path, char *root_path, char *file) {
    strcat(html_response, "<th style=\"text-align: left;\">");
    strcat(html_response, "<a href=\"");

    char *aux = path_server_to_browser(path, root_path);
    char *redirect = (char *) malloc(strlen(aux) + strlen(file) + 2);
    strcpy(redirect, aux);
    strcat(redirect, "/");
    strcat(redirect, file);

    back_path(redirect);
    char *url = path_to_url(redirect);

    strcat(html_response, url);
    free(redirect);
    free(aux);
    free(url);

    strcat(html_response, "\">");
    strcat(html_response, file);

    strcat(html_response, "</a>");
    strcat(html_response, "</th>");
}

void build_table_size(char *html_response, struct stat st) {
    strcat(html_response, "<th>");

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
        strcat(html_response, aux);
        strcat(html_response, type);
    } else {
        strcat(html_response,
               "-");
    }

    strcat(html_response, "</th>");
}

void build_table_last_date(char *html_response, struct stat st) {
    strcat(html_response, "<th>");

    struct tm *tm;

    tm = localtime(&st.st_mtime);

    char aux[64];
    sprintf(aux, "%d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec);

    strcat(html_response, aux);
    strcat(html_response, "</th>");
}

int compare(const void *a, const void *b) {
    const char **ia = (const char **) a;
    const char **ib = (const char **) b;
    return strcmp(*ia, *ib);
}

char **get_files(DIR *d, int *count) {
    struct dirent *dir;
    int capacity = 32;
    *count = 0;
    char **files = (char **) malloc(capacity * sizeof(char *));

    while ((dir = readdir(d)) != NULL) {
        if (*count == capacity) {
            capacity *= 2;
            files = realloc(files, capacity * sizeof(char *));
        }
        files[*count] = malloc(strlen(dir->d_name) + 1);
        strcpy(files[*count], dir->d_name);
        (*count)++;
    }

    qsort(files, *count, sizeof(char *), compare);

    return files;
}

char *build_html(DIR *d, char *path, char *root_path) {
    int ind = 2;
    char *html_response = (char *) malloc(MAX_SIZE_BUFFER * 2);
    strcpy(html_response, HTTP_HTML);
    strcat(html_response, "<html><head><title>My FTP</title><meta charset=\"UTF-8\"></head>");
    strcat(html_response, "<body><table><tr><th>Name</th><th>Size</th><th>Last Date</th></tr>");

    struct stat st;
    int count;

    char **files = get_files(d, &count);

    for (int i = 0; i < count; i++) {
        char aux_path[strlen(path) + strlen(files[i]) + 1];

        strcpy(aux_path, path);
        strcat(aux_path, "/");
        strcat(aux_path, files[i]);

        stat(aux_path, &st);

        strcat(html_response, "<tr>");

        build_table_name(html_response, path, root_path, files[i]);
        build_table_size(html_response, st);
        build_table_last_date(html_response, st);

        strcat(html_response, "</tr>");

        if (MAX_SIZE_BUFFER * ind - strlen(html_response) < MAX_SIZE_BUFFER) {
            ind *= 2;
            html_response = (char *) realloc(html_response, MAX_SIZE_BUFFER * ind);
        }
    }

    strcat(html_response, "</table></body></html>");
    free(files);

    return html_response;
}

char *render(DIR *d,char *path, char *root_path) {
    char *html_response = build_html(d, path, root_path);

    return html_response;
}
