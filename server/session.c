#include "session.h"
#include "config.h"
#include <string.h>
#include <stdlib.h>
#define COMMAND_LENGTH 512

extern COMMAND_HANDLER_FUNC(newconnect);
extern COMMAND_HANDLER_FUNC(ioredirect);
extern COMMAND_HANDLER_FUNC(reconnect);
extern COMMAND_HANDLER_FUNC(control);
extern COMMAND_HANDLER_FUNC(file);


command_proc command_proc_list[]=
{
	{"newconnect",  COMMAND_HANDLER(newconnect)},
	{"reconnect",   COMMAND_HANDLER(reconnect)},
	{"ioredirect",  COMMAND_HANDLER(ioredirect)},
	{"control",     COMMAND_HANDLER(control)},
	{"file",        COMMAND_HANDLER(file)},
	{NULL,          NULL}
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
int command_switcher(session_context *ctx,command_proc* proc_list,const char* command)
{
    command_proc* proc = get_command_proc(proc_list,command);
    if(proc==NULL)
    {
        //undefined command
        socket_send(ctx->s,COMMAND_RETURN_FALSE,1,0);
        return -1;
    }

    command+=strlen(proc->command);
    while(*command==':')++command;
    return proc->proc(ctx,command);
}

int session_handle(session_context *ctx)
{
	//Set KeepAlive
	set_keepalive(ctx->s,g_config.timeout);
	while(1)
	{
		if(session_recv(ctx)<=0)
		{
			break;
		}
        ctx->buffer[ctx->data_len-1]=0;
		if(command_switcher(ctx,command_proc_list,ctx->buffer)==0)
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
    session_context ctx;
    session_context_init(&ctx,SESSION_BUFFER_LEN);
    session_context_set_socket(&ctx,s);
	session_handle(&ctx);
	socket_close(s);
	session_context_clean(&ctx);
	return 0;
}
