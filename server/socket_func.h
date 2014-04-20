#include <stdio.h>
#include <string.h>

//For Windows socket function

/*!
 *	@brief:		load windows socket function from ws2_32.dll
 *	@author:	xbw
 *	@date:		2014_4_12
 */
#pragma once

#include <winsock2.h>

//dynamic socket function

SOCKET PASCAL FAR (*d_socket)(SOCKET,int,int);
int PASCAL FAR (*d_connect)(SOCKET,struct sockaddr*,int);
int PASCAL FAR  (*d_bind)(SOCKET,struct sockaddr*,int);
int PASCAL FAR (*d_listen)(SOCKET,int);
SOCKET PASCAL FAR  (*d_accept)(SOCKET,struct sockaddr*,int*);
unsigned short PASCAL FAR  (*d_htons)(unsigned short);
unsigned long PASCAL FAR  (*d_htonl)(unsigned long);
int PASCAL FAR (*d_send)(SOCKET,char*,int,unsigned int);
int PASCAL FAR (*d_recv)(SOCKET,char*,int,unsigned int);
int PASCAL FAR (*d_getsockopt)(SOCKET,int,int,void*,int*);
int PASCAL FAR (*d_getpeername)(int s, struct sockaddr *name, int *namelen);
int PASCAL FAR (*d_gethostname)(char FAR *name, int namelen);
int PASCAL FAR (*d_setsockopt)(SOCKET,int,int,const void*,int);
int PASCAL FAR (*d_closesocket)(SOCKET);
int PASCAL FAR (*d_WSAStartup)(WORD,LPWSADATA);
int PASCAL FAR (*d_WSACleanup) ();
int PASCAL FAR (*d_WSAFDIsSet)(SOCKET,fd_set*);
int PASCAL FAR  (*d_select)(SOCKET,fd_set*,fd_set*,fd_set*,struct timeval*);
struct hostent * PASCAL FAR  (*d_gethostbyname)(const char*);
int PASCAL FAR (*d_ioctlsocket)( SOCKET s, long cmd, u_long FAR* argp);

///////////////

//Setting function for Winsock
int s_func_init();				//dynamic socket function load
void s_func_clean();			//dynamic socket function unload

int s_socket_init();			//winsock init
void s_socket_clean();			//winsock clean

//Windows Socket Func (Only for Windows)
#define socket_startup			d_WSAStartup
#define socket_cleanup			d_WSACleanup
#define socket_wsa_init()		{WSADATA wsa;socket_startup(MAKEWORD(2,2),&wsa);}

#define socket_func_init		s_func_init
#define socket_func_clean		s_func_clean

//Socket func (for All)
#define socket_init				s_socket_init
#define socket_clean			s_socket_clean
#define socket_socket			d_socket
#define socket_connect			d_connect
#define socket_bind			d_bind
#define socket_recv			d_recv
#define socket_send			d_send
#define socket_close			d_closesocket
#define socket_listen			d_listen
#define socket_accept			d_accept
#define socket_select			d_select
#define socket_setsockopt		d_setsockopt
#define socket_getsockopt		d_getsockopt
#define socket_gethostbyname	d_gethostbyname
#define socket_gethostname	d_gethostname
#define socket_getpeername	d_getpeername
#define socket_ioctl			d_ioctlsocket
#define socket_htons			d_htons
#define socket_htonl			d_htonl

//Windows socket function end
