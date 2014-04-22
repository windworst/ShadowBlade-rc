#include "../session.h"
#include "../config.h"

#include <string.h>
SOCKET get_new_connect(SOCKET s,const char* command)
{
    /*  recv string like:
	 *  123.45.67.89:1234			(no carry string)
	 *  hello@123.45.67.89:1234		(with carry string)
	 *
	 *	host_str point to "123.45.67.89:1234"
	 *	carry_str point to "@"
	 */
	const char *host_str = command;
	const char *carry_str = strstr(command,"@");
	int carry_len = 0;
	SOCKET ss = -1;
	struct sockaddr sa;
	if(carry_str!=NULL)
	{
		host_str = carry_str + 1;
		carry_len = carry_str - command;
	}

	if(host_str[0]=='.') //Connect myself
	{
		int t = sizeof(sa);
		if(socket_getpeername(s,&sa,&t)!=0)
		{
			socket_send(s,COMMAND_RETURN_FALSE,1,0);
			return -1;
		}
	}
	else
	{
		if(get_sockaddr_by_string(host_str,&sa)==NULL)
		{
			socket_send(s,COMMAND_RETURN_FALSE,1,0);
			return -1;
		}
	};
	ss = tcp_connect(&sa,g_config.timeout);
	if(ss==-1)
	{
		socket_send(s,COMMAND_RETURN_FALSE,1,0);
		return -1;
	}

	//verify session
	if(!session_verify(ss,g_config.name,g_config.passwd))
	{
		socket_close(ss);
		socket_send(s,COMMAND_RETURN_FALSE,1,0);
		return -1;
	}
	//send carry_str
	if(carry_len>0)
	{
		socket_send(ss,command,carry_len,0);
	}
	socket_send(s,COMMAND_RETURN_TRUE,1,0);
	return ss;
}
COMMAND_HANDLER_FUNC(newconnect)
{
    SOCKET ss = get_new_connect(s,command);
    if(ss!=-1)
	{
		thread_instance t;
		thread_create(t,THREAD_CALLBACK(session_handle_inthread),(void*)ss);
		thread_close(t);
	}
	return 1;
}
