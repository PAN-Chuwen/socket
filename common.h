#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h> //用于变长数组
#include <arpa/inet.h> //用于inet_pton(),htons()

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/errno.h> //用于strerror()函数

#define SERVER_PORT 18000
#define MAX_LINE 200
#define BUF_SIZE 0xffff

void error_handler(const char *fmt,...);
void handle_connection(int client_socket);

#endif