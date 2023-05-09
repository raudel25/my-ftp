#include <unistd.h>
#include <pthread.h>

#include "server.h"

_Noreturn void loop(int port) {

    int sock1 = create_server(port);

    while (1) {
        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, &sock1);

        pthread_join(thread, NULL);
    }
}

int main(int argn, char *argv[]) {

    root_path = "/home/raudel";
    loop(3000);

}
