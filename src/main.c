#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <pwd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#include "server.h"

#define MAX_SIZE_BUFFER 1024
#define GREEN "\033[0;32m"
#define RESET "\033[0m"
#define ERROR "\033[0;31mmy_ftp\033[0m"

_Noreturn void loop(int port, char *root_path) {

    int sock = create_server(port);

    while (1) {
        struct sockaddr_in client;
        int len = sizeof(client);
        int sock_client = accept(sock, (struct sockaddr *) &client, (socklen_t *) &len);

        struct Client client_s;
        client_s.sock_client = sock_client;
        client_s.root_path = (char *) malloc(MAX_SIZE_BUFFER);
        strcpy(client_s.root_path, root_path);

        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, &client_s);
    }
}

int main(int argn, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);

    int port;
    char *root_path;

    uid_t uid;

    uid = getuid();
    struct passwd *pw = getpwuid(uid);


    if (argv[1] == NULL) port = 5000;
    else {
        char *p;
        int q = (int) strtol(argv[1], &p, 10);

        if (strlen(p) == 0) port = q;
        else {
            fprintf(stderr, "%s: the port is not valid\n", ERROR);

            return 0;
        }
    }

    if (argv[1] == NULL || argv[2] == NULL) {
        root_path = pw->pw_dir;
    } else {
        DIR *d;
        d = opendir(argv[2]);

        if (d) root_path = argv[2];
        else {
            fprintf(stderr, "%s: the path does not exist\n", ERROR);

            return 0;
        }
        closedir(d);
    }

    char print_path[MAX_SIZE_BUFFER];
    char print_url[MAX_SIZE_BUFFER];

    sprintf(print_path, "%smy_ftp-path%s: %s", GREEN, RESET, root_path);
    sprintf(print_url, "%smy_ftp-url%s:  http://localhost:%d", GREEN, RESET, port);

    puts(print_path);
    puts(print_url);

    loop(port, root_path);

}
