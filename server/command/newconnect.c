#include "../session.h"
#include "../config.h"

#define NEWCONNECT_BUFLEN 256

COMMAND_HANDLER_FUNC(newconnect)
{
	char recv_buf[NEWCONNECT_BUFLEN+1]={0};
	int nread = socket_recv(s,recv_buf,NEWCONNECT_BUFLEN,0);
	if(nread>0)
	{
		struct sockaddr sa;
		if(recv_buf[0]=='.') //Connect myself
		{
			int t = sizeof(sa);
			if(socket_getpeername(s,&sa,&t)!=0)
			{
				socket_send(s,COMMAND_FAILED,sizeof(COMMAND_FAILED),0);
				return 1;
			}
		}
		else
		{
			if(get_sockaddr_by_string(recv_buf,&sa)==NULL)
			{
				socket_send(s,COMMAND_FAILED,sizeof(COMMAND_FAILED),0);
				return 1;
			}
		}
		SOCKET ss = tcp_connect(&sa,g_config.timeout);
		if(ss==-1)
		{
			socket_send(s,COMMAND_FAILED,sizeof(COMMAND_FAILED),0);
			return 1;
		}
		socket_send(s,COMMAND_SUCCESS,sizeof(COMMAND_SUCCESS),0);
		NEWTHREAD_CREATE(THREAD_CALLBACK(session_handle_inthread),ss);
	}
	return 1;
}
