#include "../session.h"
#include "../config.h"

#define RECONNECT_BUFLEN 8192

COMMAND_HANDLER_FUNC(reconnect)
{
	char recv_buf[RECONNECT_BUFLEN+1]={0};
	int nread = socket_recv(s,recv_buf,RECONNECT_BUFLEN,0);
	if(nread>0)
	{
		struct sockaddr sa;
		if(get_sockaddr_by_string(recv_buf,&sa)==NULL)
		{
			socket_send(s,COMMAND_FAILED,sizeof(COMMAND_FAILED),0);
			return 1;
		}
		SOCKET ss = tcp_connect(&sa,g_config.timeout);
		if(ss==-1)
		{
			socket_send(s,COMMAND_FAILED,sizeof(COMMAND_FAILED),0);
			return 1;
		}
		socket_send(s,COMMAND_SUCCESS,sizeof(COMMAND_SUCCESS),0);
		socket_close(s);
		return session_handle(ss);
	}
	return 1;
}