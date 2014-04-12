#include "socket_func.h"

//Load Windows socket func

#include <windows.h>

/*
 * function_name_string: dynamic_function_address
 */

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
	{"WSACleanup",		(void*)&d_WSACleanup},
	{"__WSAFDIsSet",	(void*)&d_WSAFDIsSet},
	{"select",			(void*)&d_select},
	{"gethostbyname",	(void*)&d_gethostbyname},
	{"ioctlsocket",		(void*)&d_ioctlsocket},
};


static HMODULE s_ws2_32_dll=NULL;

/*!
 *	@brief:		clean dynamic socket function
 *	@author:	xbw
 *	@date:		2014_4_12
 */
void s_func_clean()
{
	if(s_ws2_32_dll!=NULL)
	{
		FreeLibrary(s_ws2_32_dll);
		s_ws2_32_dll = NULL;
	}
}

/*!
 *	@brief:		initialize dynamic socket function
 *	@author:	xbw
 *	@date:		2014_4_12
 *	@return:	success >0 , load library failed =-1,load function failed =0
 */
int s_func_init()
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

/*!
 *	@brief:		initialize dynamic socket function and wsa socket
 *	@author:	xbw
 *	@date:		2014_4_12
 *	@return:	success >0 , failed =0
 */
int s_socket_init()
{
	if(socket_func_init()<=0)
	{
		return 0;
	}
    socket_wsa_init();
	return 1;
}

/*!
 *	@brief:		clean dynamic func and wsa socket
 *	@author:	xbw
 *	@date:		2014_4_12
 */
void s_socket_clean()
{
	socket_cleanup();
	socket_func_clean();
}

#ifdef SOCKET_FUNC_MAIN
#include <stdio.h>
int main()
{
   printf("%d\n",socket_func_init());
   printf("%d\n",d_htonl(10000));
   socket_func_clean();
   return 0;
}
#endif

//End
