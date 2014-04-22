#pragma once

#include "socket_ctrl.h"
#include "thread_func.h"

//Session Context

#define SESSION_BUFFER_LEN 8192
typedef struct
{
    SOCKET s;
    char *buffer;
    int buffer_len;
    int data_len;
}session_context;

#define session_context_init(ctx_ptr,len) \
{\
    (ctx_ptr)->s = 0;\
    (ctx_ptr)->buffer = (char*) malloc(sizeof(char)*(len));\
    (ctx_ptr)->buffer_len = (len);\
    (ctx_ptr)->data_len = 0;\
}

#define session_context_clean(ctx_ptr) \
{\
    free((ctx_ptr)->buffer);\
    (ctx_ptr)->s=0;\
    (ctx_ptr)->buffer = NULL;\
    (ctx_ptr)->buffer_len = 0;\
    (ctx_ptr)->data_len = 0;\
}

#define session_context_set_socket(ctx_ptr,sockfd)\
(\
    (ctx_ptr)->s = (sockfd)\
)

#define session_send(ctx_ptr) ((ctx_ptr)->data_len = socket_send((ctx_ptr)->s,(ctx_ptr)->buffer,(ctx_ptr)->buffer_len,0))
#define session_recv(ctx_ptr) ((ctx_ptr)->data_len = socket_recv((ctx_ptr)->s,(ctx_ptr)->buffer,(ctx_ptr)->buffer_len,0))

int session_handle(session_context *ctx);
int session_verify(SOCKET s,const char* name, const char* pass);

THREAD_CALLBACK_FUNC(session_handle_inthread);


//Command Handle

#define COMMAND_RETURN_TRUE		"#"
#define COMMAND_RETURN_FALSE	"!"
#define COMMAND_HANDLER(func_name) _##func_name##_
#define COMMAND_HANDLER_FUNC(func_name) int _##func_name##_ (session_context *ctx,const char* command)

typedef int (*command_handler)(session_context *ctx,const char* command);

typedef struct
{
	const char* command;
	command_handler proc;
}command_proc;

extern command_proc command_proc_list[];

command_proc* get_command_proc(command_proc* proc_list,const char* command);

int command_switcher(session_context *ctx,command_proc* proc_list,const char* command);


