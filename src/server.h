//
// Created by raude on 5/5/2023.
//

#ifndef MY_FTP_SERVER_H
#define MY_FTP_SERVER_H

int create_server(int port);

void handle_client(int sock);

#endif //MY_FTP_SERVER_H
