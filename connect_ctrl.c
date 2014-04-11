#ifdef CONNECT_CTRL_WINDOWS

#include <winsock.h>

void socket_init()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2),&wsa);
}
 
 
long gethost(const char* name)
{
    long i = -1;
    if(name)
    {
        struct hostent *host = gethostbyname(name);
        if(host&&host->h_addr)
        {
            i = *(long *)(host->h_addr);
            return i;
        }
    }
    return i;
}
 
SOCKET tcp_connect(const char* host,int port,int time_wait)
{
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = gethost(host);
 
    if(sa.sin_addr.s_addr==-1)return -1;
    SOCKET s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(s==-1)return -1;
    unsigned long ok = 1;
    ioctlsocket(s,FIONBIO,&ok);
    connect(s,(struct sockaddr*)&sa,sizeof(sa));
 
    fd_set set;
    FD_ZERO(&set);
    FD_SET(s,&set);
 
    struct timeval tv;
    tv.tv_sec = time_wait;
    tv.tv_usec = 0;
    if(select(0,NULL,&set,NULL,&tv)<=0)
    {
        closesocket(s);
        return -1;
    }
    ok = 0;
    ioctlsocket(s,FIONBIO,&ok);
    return s;
}

#endif
