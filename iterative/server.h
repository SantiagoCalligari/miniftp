#pragma once

#include <netinet/in.h>

extern int client_fd;

int server_init(const char *ip, int port);
int server_accept(int listen_fd, struct sockaddr_in *client_addr);
void server_set_listen_fd(int fd);
void server_shutdown(void);
void server_loop(int client_fd);
