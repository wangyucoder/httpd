#ifndef _HTTP_H_
#define _HTTP_H_

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define SIZE 1024*4

void handler_header(int sock);
int echo_www(int sock,const char *path, int size);
void error_response(int sock, int err_code);
void *handler_request(void *arg);
int startup(const char *ip, int port);

#endif
