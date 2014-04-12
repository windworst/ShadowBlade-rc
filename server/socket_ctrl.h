#pragma once

#include "socket_func.h"

int socket_init();
void socket_clean();

long gethost(const char* name);
SOCKET tcp_connect(const char* host,int port,int time_wait);
