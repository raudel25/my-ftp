//
// Created by raude on 5/9/2023.
//

#ifndef MY_FTP_UTILS_H
#define MY_FTP_UTILS_H

char *path_browser_to_server(char *path, char *root_path);

char **split_line(char *line);

char *path_server_to_browser(char *path, char *root_path);

#endif //MY_FTP_UTILS_H