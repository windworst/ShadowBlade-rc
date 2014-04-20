	#include "session.h"

#include "config.h"

#define COMMAND_LENGTH 32

extern COMMAND_HANDLER_FUNC(newconnect);
extern COMMAND_HANDLER_FUNC(ioredirect);
extern COMMAND_HANDLER_FUNC(reconnect);
extern COMMAND_HANDLER_FUNC(control);


command_proc command_proc_list[]=
{
	{"newconnect",COMMAND_HANDLER(newconnect)},
	{"reconnect",COMMAND_HANDLER(reconnect)},
	{"ioredirect",COMMAND_HANDLER(ioredirect)},
	{"control",COMMAND_HANDLER(control)},
	{NULL,NULL}
};

command_handler get_command_handler(command_proc* proc_list,const char* command)
{
	int i;
	for(i=0;proc_list[i].command!=NULL;++i)
	{
		if(strncmp(command,proc_list[i].command,strlen(proc_list[i].command))==0)
		{
			return proc_list[i].proc;
		}
	}
	return NULL;
}

int session_handle(SOCKET s,command_proc* proc_list)
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
		command_handler hldr = get_command_handler(proc_list,command);
		if(hldr==NULL)
		{
			//undefined command
			socket_send(s,COMMAND_RETURN_FALSE,1,0);
			continue;
		}
		socket_send(s,COMMAND_RETURN_TRUE,1,0);
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
	session_handle(s,command_proc_list);
	socket_close(s);
	return 0;
}
