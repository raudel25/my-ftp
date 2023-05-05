#include <sys/socket.h>
#include <netinet/in.h>

#include "server.h"

void loop(char *path,int port) {

    int sock1 = create_server(port);

    struct sockaddr_in client;
    int len = sizeof(client);

    int sock2 = accept(sock1, (struct sockaddr *) &client, (socklen_t *) &len);
    handle_client(sock2,path);
}

int main(int argn, char *argv[]) {


    loop("/home/raudel",5000);

    return 0;
}
