#pragma once

SOCKET PASCAL FAR (*d_socket)(SOCKET,int,int);
int PASCAL FAR (*d_connect)(SOCKET,struct sockaddr*,int);
int PASCAL FAR  (*d_bind)(SOCKET,struct sockaddr*,int);
int PASCAL FAR (*d_listen)(SOCKET,int);
SOCKET PASCAL FAR  (*d_accept)(SOCKET,struct sockaddr*,int*);
unsigned short PASCAL FAR  (*d_htons)(unsigned short);
unsigned long PASCAL FAR  (*d_htonl)(unsigned long);
int PASCAL FAR (*d_send)(SOCKET,char*,int,unsigned int);
int PASCAL FAR (*d_recv)(SOCKET,char*,int,unsigned int);
int (*d_getsockopt)(SOCKET,int,int,void*,int*);
int PASCAL FAR (*d_setsockopt)(SOCKET,int,int,const void*,int);
int PASCAL FAR (*d_closesocket)(SOCKET);
int (*d_WSAStartup)(WORD,LPWSADATA);
int PASCAL FAR (*d_WSAFDIsSet)(SOCKET,fd_set*);
int PASCAL FAR  (*d_select)(SOCKET,fd_set*,fd_set*,fd_set*,struct timeval*);
struct hostent * PASCAL FAR  (*d_gethostbyname)(const char*);

void socket_func_clean();

int socket_func_init();
