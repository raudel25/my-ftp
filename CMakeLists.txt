cmake_minimum_required(VERSION 3.22)
project(my_ftp C)

set(CMAKE_C_STANDARD 17)

add_executable(my_ftp src/main.c src/server.h src/server.c src/render.h src/render.c src/utils.h src/utils.c)
