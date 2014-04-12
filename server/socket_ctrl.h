#pragma once

#include "socket_func.h"

int socket_init();
void socket_clean();

long gethost(const char* name);
struct sockaddr* make_sockaddr(const char* host,int port,struct sockaddr* sa);
struct sockaddr* get_sockaddr_by_string(char* str,struct sockaddr* sa);
struct sockaddr* get_sockaddr_by_url(char* url,struct sockaddr* sa,char* name,int timewait);
SOCKET tcp_connect(struct sockaddr* sa,int time_wait);
