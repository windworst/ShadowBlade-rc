#include <winsock2.h>
#include <windows.h>

#include "socket_func.h"

typedef struct
{
	const char* func_name;
	void** p_func;
}export_func;

static export_func s_socket_func_list[] =
{
	{"socket",			(void*)&d_socket},
	{"connect",			(void*)&d_connect},
	{"bind",			(void*)&d_bind},
	{"listen",			(void*)&d_listen},
	{"accept",			(void*)&d_accept},
	{"htons",			(void*)&d_htons},
	{"htonl",			(void*)&d_htonl},
	{"send",			(void*)&d_send},
	{"recv",			(void*)&d_recv},
	{"getsockopt",		(void*)&d_getsockopt},
	{"setsockopt",		(void*)&d_setsockopt},
	{"closesocket",		(void*)&d_closesocket},
	{"WSAStartup",		(void*)&d_WSAStartup},
	{"__WSAFDIsSet",	(void*)&d_WSAFDIsSet},
	{"select",			(void*)&d_select},
	{"gethostbyname",	(void*)&d_gethostbyname},
};


static HMODULE s_ws2_32_dll=NULL;

void socket_func_clean()
{
	if(s_ws2_32_dll!=NULL)
	{
		FreeLibrary(s_ws2_32_dll);
		s_ws2_32_dll = NULL;
	}
}

int socket_func_init()
{
	socket_func_clean();

	s_ws2_32_dll = LoadLibrary("ws2_32.dll");
	if(s_ws2_32_dll==NULL)return -1;

	int i;
	int s = sizeof(s_socket_func_list)/sizeof(*s_socket_func_list);
	for(i=0;i<s;++i)
	{
		void* h_proc = GetProcAddress(s_ws2_32_dll,s_socket_func_list[i].func_name);
		if(h_proc==NULL)
		{
			socket_func_clean();
			return 0;
		}
		*s_socket_func_list[i].p_func = h_proc;
	}
	return 1;
}

#ifdef SOCKET_FUNC_TEST_MAIN
#include <stdio.h>
int main()
{
   printf("%d\n",socket_func_init());
   printf("%d\n",d_htonl(10000));
   socket_func_clean();
   return 0;
}
#endif
