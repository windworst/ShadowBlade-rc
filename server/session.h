#pragma once

#include "socket_ctrl.h"
#include "thread_func.h"

#define COMMAND_SUCCESS "success"
#define COMMAND_FAILED  "failed"
#define COMMAND_ERROR	"error"
#define COMMAND_QUIT	"quit"

//Command Handle
typedef int (*command_handler)(SOCKET s);
#define COMMAND_HANDLER_FUNC(func_name) int ##func_name##(SOCKET s)

typedef struct
{
	const char* command;
	command_handler proc;
}command_proc;

extern command_proc command_proc_list[];

//Session Handle
int session_handle(SOCKET s);
THREAD_CALLBACK_FUNC(session_handle_inthread);

