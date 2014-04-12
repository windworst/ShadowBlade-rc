#include "socket_ctrl.h"

/*!
 *	@brief:		get ipvalue(32bit-long) by hostname
 *	@author:	xbw
 *	@date:		2014_4_12
 *	@args:		hostname in const char* string,
 *	@return:	32bit-long ipvalue, or -1 failed
 */
long gethost(const char* name)
{
    long i = -1;
    if(name)
    {
        struct hostent *host = socket_gethostbyname(name);
        if(host&&host->h_addr)
        {
            i = *(long *)(host->h_addr);
            return i;
        }
    }
    return i;
}

/*!
 * @brief:		get connect sockfd by host,port.
 * @author:		xbw
 * @date:		2014_4_12
 * @args:		hostname in const char* string,connection port,connect timewait
 * @return:		connect sockfd, or -1 failed
 */
SOCKET tcp_connect(const char* host,int port,int time_wait)
{
	//setting connect sockaddr
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = socket_htons(port);
    sa.sin_addr.s_addr = gethost(host);
 
    if(sa.sin_addr.s_addr==-1)return -1;
    SOCKET s = socket_socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(s==-1)return -1;

	//setting NBlock
    unsigned long ok = 1;
    socket_ioctlsocket(s,FIONBIO,&ok);
    socket_connect(s,(struct sockaddr*)&sa,sizeof(sa));
 
	//check connect socketfd usable
    fd_set set;
    FD_ZERO(&set);
    FD_SET(s,&set);
 
    struct timeval tv;
    tv.tv_sec = time_wait;
    tv.tv_usec = 0;
    if(socket_select(0,NULL,&set,NULL,&tv)<=0)
    {
        socket_closesocket(s);
        return -1;
    }

	//Setting NBlock
    ok = 0;
    socket_ioctlsocket(s,FIONBIO,&ok);
    return s;
}

#ifdef SOCK_CTRL_MAIN
#include <stdio.h>

int main(int argc,char **argv)
{
	socket_init();
	SOCKET s = tcp_connect("g.cn",80,5);
	printf("socket-fd,value:%d\n",s);
	char buf[8192]={0};
	char send_str[]="GET /\r\n";
	socket_send(s,send_str,sizeof(send_str),0);
	int nrecv = socket_recv(s,buf,8192,0);
	printf("recv:%d\n%s\n",nrecv,buf);
	socket_clean();
	return 0;
}

#endif
