#include "../session.h"

#include <stdio.h>

COMMAND_HANDLER_FUNC(ls)
{
	return 1;
}

static command_proc file_command_proc_list[]=
{
	{"ls",COMMAND_HANDLER(ls)},
	{NULL,NULL}
};


COMMAND_HANDLER_FUNC(file)
{
	return command_switcher(ctx,file_command_proc_list,command);
}
