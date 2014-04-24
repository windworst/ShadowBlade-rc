#include "../session.h"

#include <stdio.h>
#include <stdlib.h>
#include "../file_func.h"

#define FILE_BUF_LEN 1024*1024

void handle_session_getfile(session_context* ctx,FILE_HANDLE fh)
{
    char *file_buf = (char*)malloc(sizeof(char)*FILE_BUF_LEN);
    int file_buf_len  = FILE_BUF_LEN;
    if(file_buf==NULL)return;
    while(session_recv(ctx)>0)
    {
        uint64_t offset = 0;
        uint64_t len = 0;
        uint64_t total = 0;

        if(sscanf(ctx->buffer,"%lld:%lld",&offset,&len)!=2 || len==0)
        {
            break;
        }
        file_seek(fh,offset,SEEK_SET);
        while(total<len)
        {
            int32_t nread = (int32_t)(file_buf_len > (len - total) ? (len - total) : file_buf_len);
            int32_t read_len = file_read(fh,file_buf,nread);
            int32_t net_read_len = socket_htonl(read_len);

            if(socket_send(ctx->s,COMMAND_RETURN_TRUE,1,0)<=0
            ||  socket_send(ctx->s,(char*)&net_read_len,sizeof(read_len),0)<=0
            || read_len==0
               )
            {
                break;
            }

            net_read_len = 0;
            while(net_read_len<read_len)
            {
                int32_t nsend = ctx->buffer_len > (read_len - net_read_len) ? (read_len - net_read_len) : ctx->buffer_len;
                if(socket_send(ctx->s,file_buf,nsend,0)<0)break;
                net_read_len += nsend;
            }
            total += read_len;
        }
    }
    free(file_buf);
}

void handle_session_putfile(session_context* ctx,FILE_HANDLE fh)
{
    char *file_buf = (char*)malloc(sizeof(char)*FILE_BUF_LEN);
    int file_buf_len  = FILE_BUF_LEN;
    if(file_buf==NULL)return;
    while(session_recv(ctx)>0)
    {
        uint64_t offset = 0;
        uint64_t len = 0;
        uint64_t total = 0;
        int32_t buf_nrecv = 0;

        if(sscanf(ctx->buffer,"%lld:%lld",&offset,&len)!=2 || len==0)
        {
            break;
        }
        file_seek(fh,offset,SEEK_SET);
        while(total<len)
        {
            int32_t nrecv = ctx->buffer_len > (file_buf_len - buf_nrecv) ? (file_buf_len - buf_nrecv) : ctx->buffer_len;
            int32_t read_len = socket_recv(ctx->s,file_buf+buf_nrecv,nrecv,0);
            buf_nrecv += read_len;
            total += read_len;

            //Write to Disk
            if(buf_nrecv==file_buf_len)
            {
                file_write(fh,file_buf,buf_nrecv);
                buf_nrecv = 0;
            }
        }
        if(buf_nrecv>0)
        {
            file_write(fh,file_buf,buf_nrecv);
            buf_nrecv = 0;
        }
        socket_send(ctx->s,COMMAND_RETURN_TRUE,1,0);
    }
    free(file_buf);
}

COMMAND_HANDLER_FUNC(ls)
{
    file_finddata_t ff;
    int fd = file_findfirst(command,&ff);
    if(fd==-1)
    {
        socket_send(ctx->s,COMMAND_RETURN_FALSE,1,0);
        return 1;
    }
    socket_send(ctx->s,COMMAND_RETURN_TRUE,1,0);
    do
    {
        uint64_t filesize = ff.size;
        sprintf(ctx->buffer,"%s|%lld|%d|%d|%d|%d\n",
                ff.name,filesize,ff.attrib,ff.time_access,ff.time_create,ff.time_write);
        session_send(ctx,strlen(ctx->buffer));
    }
    while(file_findnext(fd,&ff)==0);
    socket_send(ctx->s,"\n",1,0);
    file_findclose(fd);
	return 1;
}

COMMAND_HANDLER_FUNC(put)
{
    FILE_HANDLE fh = file_open(command,FILE_ACCESS_WRITE,FILE_OPEN_CREATE);
    if(fh==INVALID_FILEHANDLE_VALUE)
    {
        socket_send(ctx->s,COMMAND_RETURN_FALSE,1,0);
        return 1;
    }
    socket_send(ctx->s,COMMAND_RETURN_TRUE,1,0);
    handle_session_putfile(ctx,fh);
    socket_send(ctx->s,COMMAND_RETURN_FALSE,1,0);
    file_close(fh);
	return 1;
}

COMMAND_HANDLER_FUNC(get)
{
    FILE_HANDLE fh = file_open(command,FILE_ACCESS_READ,FILE_OPEN_EXIST);
    if(fh==INVALID_FILEHANDLE_VALUE)
    {
        socket_send(ctx->s,COMMAND_RETURN_FALSE,1,0);
        return 1;
    }
    socket_send(ctx->s,COMMAND_RETURN_TRUE,1,0);
    handle_session_getfile(ctx,fh);
    socket_send(ctx->s,COMMAND_RETURN_FALSE,1,0);
    file_close(fh);
	return 1;
}

static command_proc file_command_proc_list[]=
{
	{"ls",COMMAND_HANDLER(ls)},
	{"get",COMMAND_HANDLER(get)},
	{"put",COMMAND_HANDLER(put)},
	{NULL,NULL}
};


COMMAND_HANDLER_FUNC(file)
{
	return command_switcher(ctx,file_command_proc_list,command);
}
