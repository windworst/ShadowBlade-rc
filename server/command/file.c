#include "../session.h"

#include <stdio.h>
#include <stdlib.h>
#include "../file_func.h"

#define FILE_BUF_LEN 1024*1024
#define FILE_PATH_LEN 2048

#ifdef SERVER_DEBUG
#define FILE_DEBUG
#endif

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

            #ifdef FILE_DEBUG
            printf("getfile:read_len:%d\n",read_len);
            #endif // FILE_DEBUG

            if(socket_send(ctx->s,(char*)&net_read_len,sizeof(read_len),0)<=0)
            {
                return;
            }

            if(read_len<=0)
            {
                break;
            }

            net_read_len = 0;
            while(net_read_len<read_len)
            {
                int32_t nsend = ctx->buffer_len > (read_len - net_read_len) ? (read_len - net_read_len) : ctx->buffer_len;
                if(socket_send(ctx->s,file_buf,nsend,0)<0)
                {
                    return;
                }
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
        int32_t is_write_error = 0;

        if(sscanf(ctx->buffer,"%lld:%lld",&offset,&len)!=2 || len==0)
        {
            break;
        }
        file_seek(fh,offset,SEEK_SET);
        while(total<len && !is_write_error)
        {
            int32_t nrecv = ctx->buffer_len > (file_buf_len - buf_nrecv) ? (file_buf_len - buf_nrecv) : ctx->buffer_len;
            int32_t read_len = socket_recv(ctx->s,file_buf+buf_nrecv,nrecv,0);
            if(read_len<=0)
            {
                return;
            }
            buf_nrecv += read_len;
            total += read_len;

            //Write to Disk
            if(buf_nrecv==file_buf_len)
            {
                if(file_write(fh,file_buf,buf_nrecv)<=0)
                {
                    is_write_error = 1;
                }
                buf_nrecv = 0;
            }
        }
        if(buf_nrecv>0)
        {
            if(file_write(fh,file_buf,buf_nrecv)<=0)
            {
                is_write_error = 1;
            }
            buf_nrecv = 0;
        }
        socket_send(ctx->s,is_write_error?COMMAND_RETURN_FALSE:COMMAND_RETURN_TRUE,1,0);
    }
    socket_send(ctx->s,COMMAND_RETURN_FALSE,1,0);
    free(file_buf);
}

enum TRAVER_OPERATE
{
    TRAVER_LSTREE = 1,
    TRAVER_REMOVE = 2,
};
int travesal_dir(session_context* ctx,char* path,int path_len,int operation_code)
{
    struct _finddata_t ff;
    FILE_HANDLE fd = INVALID_FILEHANDLE_VALUE;
    int count = 0;

    if(path_len>=FILE_PATH_LEN)return 0;

	while(path_len-1>0 && (path[path_len-1]=='/'||path[path_len-1]=='\\'))
	{
		--path_len;
	}

	memcpy(path+path_len,"/*",3);
    fd = _findfirst(path,&ff);
    path[path_len] = '\0';

	if(fd==INVALID_FILEHANDLE_VALUE)
    {
        #ifdef FILE_DEBUG
        printf("travesal_dir(%s,%d):\n\t failed find path=\"%s\"\n",__FILE__,__LINE__,path);
        #endif // FILE_DEBUG
        return 0;
    }

	do
	{
	    int name_len = strlen(ff.name);

		if(	strncmp(ff.name,".",name_len)==0
				|| strncmp(ff.name,"..",name_len)==0)
		{
			continue;
		}

		if(count==0 && operation_code!=TRAVER_REMOVE)
		{
			socket_send(ctx->s,"{",1,0);
		}
		++count;

        if(operation_code!=TRAVER_REMOVE)
        {
            char buffer[FILE_PATH_LEN]={0};
            uint64_t filesize = ff.size;
            sprintf(buffer,"<%s|%lld|%d|%d|%d|%d>\n",
                ff.name,filesize,ff.attrib,ff.time_access,ff.time_create,ff.time_write);
            socket_send(ctx->s,buffer,strlen(buffer),0);
        }

        if(path_len+name_len+1>=FILE_PATH_LEN)return 0;
        path[path_len] = '/';
        memcpy(path+path_len+1,ff.name,name_len+1);

        if( (ff.attrib&_A_SUBDIR) && ((operation_code==TRAVER_LSTREE) || (operation_code==TRAVER_REMOVE)) )
        {
            #ifdef FILE_DEBUG
            printf("travesal_dir(%s,%d):\n\t into path=\"%s\"\n",__FILE__,__LINE__,path);
            #endif // FILE_DEBUG
            count += travesal_dir(ctx,path,path_len+name_len+1,operation_code);
        }

        if(operation_code==TRAVER_REMOVE)
        {
            #ifdef FILE_DEBUG
            printf("travesal_dir(%s,%d):\n\t remove path=\"%s\"\n",__FILE__,__LINE__,path);
            #endif // FILE_DEBUG
            if(ff.attrib&_A_SUBDIR)
            {
                dir_rmdir(path);
            }
            else
            {
                file_remove(path);
            }
        }

        path[path_len] = '\0';
	}
	while(_findnext(fd,&ff)==0);

	dir_findclose(fd);

	if(count!=0 && operation_code!=TRAVER_REMOVE)
	{
		socket_send(ctx->s,"}",1,0);
	}

	if(operation_code==TRAVER_REMOVE)
    {
        dir_rmdir(path);
    }

	return count;
}

COMMAND_HANDLER_FUNC(ls)
{
    int operation_code = 0;
    if(command[0]=='|')
    {
        operation_code = TRAVER_LSTREE;
        ++command;
    }

    socket_send(ctx->s,COMMAND_RETURN_TRUE,1,0);
    travesal_dir(ctx,(char*)command,strlen(command),operation_code);

	return 1;
}

COMMAND_HANDLER_FUNC(put)
{
    FILE_HANDLE fh = file_open(command,O_WRONLY|O_CREAT|O_BINARY);
    if(fh==INVALID_FILEHANDLE_VALUE)
    {
        socket_send(ctx->s,COMMAND_RETURN_FALSE,1,0);
        return 1;
    }
    socket_send(ctx->s,COMMAND_RETURN_TRUE,1,0);
    handle_session_putfile(ctx,fh);
    file_close(fh);
	return 1;
}

COMMAND_HANDLER_FUNC(get)
{
    FILE_HANDLE fh = file_open(command,O_RDONLY|O_BINARY);
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

COMMAND_HANDLER_FUNC(rm)
{
    int ret = file_remove(command)==0;
    if(!ret)
    {
        ret = travesal_dir(ctx,(char*)command,strlen(command),TRAVER_REMOVE)>0;
    }
    socket_send(ctx->s,ret?COMMAND_RETURN_TRUE:COMMAND_RETURN_FALSE,1,0);
    return 1;
}

COMMAND_HANDLER_FUNC(mkdir)
{
    int ret = dir_mkdir(command)==0;
    socket_send(ctx->s,ret?COMMAND_RETURN_TRUE:COMMAND_RETURN_FALSE,1,0);
    return 1;
}

static command_proc file_command_proc_list[]=
{
	{"ls",COMMAND_HANDLER(ls)},
	{"get",COMMAND_HANDLER(get)},
	{"put",COMMAND_HANDLER(put)},
	{"rm",COMMAND_HANDLER(rm)},
	{"mkdir",COMMAND_HANDLER(mkdir)},
	{NULL,NULL}
};


COMMAND_HANDLER_FUNC(file)
{
	return command_switcher(ctx,file_command_proc_list,command);
}
