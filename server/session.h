#pragma once

#include "socket_ctrl.h"
#include "thread_func.h"

#define COMMAND_RETURN_TRUE		"#"
#define COMMAND_RETURN_FALSE	"!"

//Command Handle
typedef int (*command_handler)(SOCKET s);
#define COMMAND_HANDLER(func_name) _##func_name##_
#define COMMAND_HANDLER_FUNC(func_name) int _##func_name##_ (SOCKET s)

typedef struct
{
	const char* command;
	command_handler proc;
}command_proc;

extern command_proc command_proc_list[];

//Session Handle
int session_handle(SOCKET s);
THREAD_CALLBACK_FUNC(session_handle_inthread);

