#include "../session.h"

#include <stdlib.h>

COMMAND_HANDLER_FUNC(killyourself)
{
	exit(0);
	return 1;
}

static command_proc control_command_proc_list[]=
{
	{"killyourself",COMMAND_HANDLER(killyourself)},
	{NULL,NULL}
};


COMMAND_HANDLER_FUNC(control)
{
	return session_handle(s,control_command_proc_list);
}


