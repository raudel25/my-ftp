//
// Created by raude on 5/5/2023.
//

#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>

char *render(int sock) {
    char *html_response = "<html><body><h1>Hola Mundo!</h1></body></html>";
    char *http_header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    char *response = malloc(strlen(http_header) + strlen(html_response) + 1);
    strcpy(response, http_header);
    strcat(response, html_response);

    return response;
}

