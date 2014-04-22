#include "../session.h"
#include "../config.h"

//in newconnect.c
extern SOCKET get_new_connect(SOCKET s,const char* command);

COMMAND_HANDLER_FUNC(reconnect)
{
    SOCKET ss = get_new_connect(ctx->s,command);
    if(ss==-1)return 1;
	socket_send(ctx->s,COMMAND_RETURN_TRUE,1,0);
	socket_close(ctx->s);
	session_context_set_socket(ctx,ss);
	return session_handle(ctx);
}
