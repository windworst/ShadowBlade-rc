#include "../session.h"
#include "../config.h"

#define NEWCONNECT_BUFLEN 256

#include <string.h>

COMMAND_HANDLER_FUNC(newconnect)
{
	char recv_buf[NEWCONNECT_BUFLEN+1]={0};
	int nread = socket_recv(s,recv_buf,NEWCONNECT_BUFLEN,0);
	int i;
	if(nread>0)
	{
		/*  recv string like:
		 *  123.45.67.89:1234			(no carry string)
		 *  hello@123.45.67.89:1234		(with carry string)
		 *
		 *	host_str point to "123.45.67.89:1234"
		 *	carry_str point to "@"
		 */
		char *host_str = recv_buf;
		char *carry_str = strstr(recv_buf,"@");
		int carry_len = 0;
		if(carry_str!=NULL)
		{
			host_str = carry_str + 1;
			carry_len = carry_str - recv_buf;
		}

		struct sockaddr sa;
		if(host_str[0]=='.') //Connect myself
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
			if(get_sockaddr_by_string(host_str,&sa)==NULL)
			{
				socket_send(s,COMMAND_RETURN_FALSE,1,0);
				return 1;
			}
		};
		SOCKET ss = tcp_connect(&sa,g_config.timeout);
		if(ss==-1)
		{
			socket_send(s,COMMAND_RETURN_FALSE,1,0);
			return 1;
		}

		//verify session
		if(!session_verify(ss,g_config.name,g_config.passwd))
		{
			socket_close(ss);
			socket_send(s,COMMAND_RETURN_FALSE,1,0);
			return 1;
		}
		//send carry_str
		if(carry_len>0)
		{
			socket_send(ss,recv_buf,carry_len,0);
		}
		socket_send(s,COMMAND_RETURN_TRUE,1,0);
		thread_instance t;
		thread_create(t,THREAD_CALLBACK(session_handle_inthread),(void*)ss);
		thread_close(t);
	}
	return 1;
}
