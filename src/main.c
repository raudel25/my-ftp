#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <pwd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#include "server.h"

#define MAX_SIZE_BUFFER 1024
#define GREEN "\033[0;32m"
#define RESET "\033[0m"
#define ERROR "\033[0;31mmy_ftp\033[0m"

_Noreturn void loop(int port) {

    int sock1 = create_server(port);

    while (1) {
        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, &sock1);

        pthread_join(thread, NULL);
    }
}

int main(int argn, char *argv[]) {
    int port;
    int ind = 1;

    uid_t uid;

    uid = getuid();
    struct passwd *pw = getpwuid(uid);


    if (argv[ind] == NULL) port = 5000;
    else {
        char *p;
        int q = (int) strtol(argv[ind], &p, 10);
        puts(argv[ind]);
        if (strlen(p) == 0) port = q;
        else {
            fprintf(stderr, "%s: the port is not valid\n", ERROR);

            return 0;
        }
        ind++;
    }

    if (argv[ind] == NULL) {
        root_path = pw->pw_dir;
    } else {
        DIR *d;
        d = opendir(argv[ind]);

        if (d) root_path = argv[ind];
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

    loop(port);

}
