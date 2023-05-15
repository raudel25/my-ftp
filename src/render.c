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
#define HTTP_HEADER "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
#define HTML_TABLE_HEAR "<!-- TABLE_HEAR -->"
#define LINK_STYLE "onmouseover=\"this.style.color='blue'\" onmouseout=\"this.style.color='black'\" style=\"color: black; text-decoration: none;\""

void build_table_name(char *html_response, char *path, char *root_path, char *file, struct stat st) {
    strcat(html_response, "<td>");
    strcat(html_response, "<a ");
    strcat(html_response, LINK_STYLE);
    strcat(html_response, "href=\"");

    char *aux = path_server_to_browser(path, root_path);
    char *redirect = (char *) malloc(strlen(aux) + strlen(file) + 2);
    strcpy(redirect, aux);
    strcat(redirect, "/");
    strcat(redirect, file);

    char *url = path_to_url(redirect);

    strcat(html_response, url);
    free(redirect);
    free(aux);
    free(url);

    if (S_ISDIR(st.st_mode)) {
        strcat(html_response, "\"><span><div class=\"folder\"></div> ");
    } else {
        strcat(html_response, "\"><span><div class=\"file\"></div> ");
    }
    strcat(html_response, file);

    strcat(html_response, "</span></a>");
    strcat(html_response, "</td>");
}

void build_table_size(char *html_response, struct stat st) {
    strcat(html_response, "<td class=\"center\">");

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

    strcat(html_response, "</td>");
}

void build_table_last_date(char *html_response, struct stat st) {
    strcat(html_response, "<td class=\"center\">");

    struct tm *tm;

    tm = localtime(&st.st_mtime);

    char aux[64];
    sprintf(aux, "%d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec);

    strcat(html_response, aux);
    strcat(html_response, "</td>");
}

char **load_html() {
    FILE *fp = fopen("../resources/index.html", "r");

    char buffer[MAX_SIZE_BUFFER];

    fread(buffer, MAX_SIZE_BUFFER, 1, fp);

    char *p = strstr(buffer, HTML_TABLE_HEAR);

    char **html = (char **) malloc(2 * sizeof(char *));

    html[1] = (char *) malloc(strlen(p) + 1);
    strcpy(html[1], p);

    buffer[p - buffer] = 0;
    html[0] = (char *) malloc(strlen(buffer) + 1);
    strcpy(html[0], buffer);

    fclose(fp);

    return html;
}

void build_back(char *html_response, char *path, char *root_path) {
    strcat(html_response, "<td>");
    strcat(html_response, "<a href=\"");

    char *redirect = path_server_to_browser(path, root_path);
    back_path(redirect);

    char *url = path_to_url(redirect);
    strcat(html_response, url);
    free(redirect);
    free(url);

    strcat(html_response, "\">");
    strcat(html_response, "<div class=\"gg-arrow-left\"></div></a></td>");
    strcat(html_response, "<td class=\"center\"></td><td class=\"center\"></td></tr>");
}

char *build_html(DIR *d, char *path, char *root_path) {
    char **html = load_html();
    int ind = 2;
    char *html_response = (char *) malloc(MAX_SIZE_BUFFER * ind);
    strcpy(html_response, HTTP_HEADER);
    strcat(html_response, html[0]);

    struct stat st;
    struct dirent *dir;

    build_back(html_response, path, root_path);

    while ((dir = readdir(d)) != NULL) {
        if (strcmp(dir->d_name, ".") == 0) continue;
        if (strcmp(dir->d_name, "..") == 0) continue;

        char aux_path[strlen(path) + strlen(dir->d_name) + 1];

        strcpy(aux_path, path);
        strcat(aux_path, "/");
        strcat(aux_path, dir->d_name);

        stat(aux_path, &st);

        strcat(html_response, "<tr>");

        build_table_name(html_response, path, root_path, dir->d_name, st);
        build_table_size(html_response, st);
        build_table_last_date(html_response, st);

        strcat(html_response, "</tr>");

        if (MAX_SIZE_BUFFER * ind - strlen(html_response) < MAX_SIZE_BUFFER) {
            ind *= 2;
            html_response = (char *) realloc(html_response, MAX_SIZE_BUFFER * ind);
        }
    }

    strcat(html_response, html[1]);
    free(html);

    return html_response;
}

char *render(DIR *d,char *path, char *root_path) {
    char *html_response = build_html(d, path, root_path);

    return html_response;
}
