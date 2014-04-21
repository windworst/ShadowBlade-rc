#include "session.h"
#include "config.h"
#include <string.h>

#define COMMAND_LENGTH 512

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

command_proc* get_command_proc(command_proc* proc_list,const char* command)
{
	int i;
	for(i=0;proc_list[i].command!=NULL;++i)
	{
		if(strncmp(command,proc_list[i].command,strlen(proc_list[i].command))==0)
		{
			return &proc_list[i];
		}
	}
	return NULL;
}

//1 for success, 0 for failed, -1 command not found
int command_switcher(SOCKET s,command_proc* proc_list,const char* command)
{
    command_proc* proc = get_command_proc(proc_list,command);
    if(proc==NULL)
    {
        //undefined command
        socket_send(s,COMMAND_RETURN_FALSE,1,0);
        return -1;
    }

    command+=strlen(proc->command);
    while(*command==':')++command;
    return proc->proc(s,command);
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
		//Quit
		if(command_switcher(s,command_proc_list,command)==0)
        {
            return 1;
        }
	}
	//Socket cut down / Socket read error
	return 0;
}

//verify session before session_handle, now is null-function
int session_verify(SOCKET s,const char* name, const char* pass)
{
	return 1;
}

THREAD_CALLBACK_FUNC(session_handle_inthread)
{
	SOCKET s = (SOCKET)arg;
	session_handle(s);
	socket_close(s);
	return 0;
}
