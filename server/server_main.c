#include "session.h"
#include "config.h"
int server_main(int argc,char** argv)
{
	//Read config from raw_config
	read_config(&g_raw_config,&g_config);

	//init socket
	if(socket_init()<=0)return -1;
	while(1)
	{
		struct sockaddr sa;
		SOCKET s = -1;
		if( get_sockaddr_by_url(g_config.url,&sa,g_config.name,g_config.timewait)
			||	get_sockaddr_by_string(g_config.url,&sa))
		{
			s = tcp_connect(&sa,g_config.timeout);
		}
		if(s!=-1)
		{
			session_handle(s);
			socket_close(s);
		}
		else
		{
			time_wait(g_config.timewait);
		}
	}
	socket_clean();
	return 0;
}

int main(int argc, char** argv)
{
	return server_main(argc,argv);
}
