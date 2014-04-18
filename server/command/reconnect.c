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
		if(recv_buf[0]=='.') //Connect myself
		{
			int t = sizeof(sa);
			if(socket_getpeername(s,&sa,&t)!=0)
			{
				socket_send(s,COMMAND_RETURN_FALSE,1,0);
				return 1;
			}
		}
		else
		{
			if(get_sockaddr_by_string(recv_buf,&sa)==NULL)
			{
				socket_send(s,COMMAND_RETURN_FALSE,1,0);
				return 1;
			}
		}
		SOCKET ss = tcp_connect(&sa,g_config.timeout);
		if(ss==-1)
		{
			socket_send(s,COMMAND_RETURN_FALSE,1,0);
			return 1;
		}
		socket_send(s,COMMAND_RETURN_TRUE,1,0);
		socket_close(s);
		return session_handle(ss);
	}
	return 1;
}
