#include "session.h"

#include "config.h"

#define COMMAND_LENGTH 32

extern COMMAND_HANDLER_FUNC(newconnect);
extern COMMAND_HANDLER_FUNC(ioredirect);
extern COMMAND_HANDLER_FUNC(reconnect);

command_proc command_proc_list[]=
{
	{"newconnect",COMMAND_HANDLER(newconnect)},
	{"reconnect",COMMAND_HANDLER(reconnect)},
	{"ioredirect",COMMAND_HANDLER(ioredirect)},
};

command_handler get_command_handler(const char* command)
{
	int i;
	for(i=0;i<sizeof(command_proc_list)/sizeof(*command_proc_list);++i)
	{
		if(strncmp(command,command_proc_list[i].command,strlen(command_proc_list[i].command))==0)
		{
			return command_proc_list[i].proc;
		}
	}
	return NULL;
}

int session_handle(SOCKET s)
{
	//Set KeepAlive
	set_keepalive(s,g_config.timeout);
	while(1)
	{
		char command[COMMAND_LENGTH+1]={0};
		if(socket_recv(s,command,COMMAND_LENGTH,0)<=0)
		{
			break;
		}
		//Get command handler
		command_handler hldr = get_command_handler(command);
		if(hldr==NULL)
		{
			//undefined command
			socket_send(s,COMMAND_REJECT,sizeof(COMMAND_REJECT),0);
			continue;
		}
		socket_send(s,COMMAND_ACCEPT,sizeof(COMMAND_ACCEPT),0);
		if(!hldr(s))
		{
			//command_handler cut down session
			return 1;
		}
	}
	//Socket cut down / Socket read error
	return 0;
}

THREAD_CALLBACK_FUNC(session_handle_inthread)
{
	SOCKET s = (SOCKET)arg;
	session_handle(s);
	socket_close(s);
	return 0;
}
