#include "session.h"

#include <string.h>

#define COMMAND_LENGTH 32

command_proc command_proc_list[]=
{
	{"newconnect",NULL},
	{"ioredirect",NULL},
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
	char command[COMMAND_LENGTH+1]={0};
	while(socket_recv(s,command,COMMAND_LENGTH,0)>0)
	{
		//Get command handler
		command_handler hldr = get_command_handler(command);
		if(hldr==NULL)
		{
			//undefined command
			socket_send(s,COMMAND_ERROR,sizeof(COMMAND_ERROR),0);
		}
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
