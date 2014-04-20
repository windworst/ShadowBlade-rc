#include "socket_ctrl.h"

void set_keepalive(SOCKET s,int timeout)
{
	;
}

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
 * @brief:			get connect messsage from host and port
 * @author:			xbw
 * @date:			2014_4_12
 * @args:			host,port, sockaddr* sa
 * @return:			sockaddr *
 */
struct sockaddr* make_sockaddr(const char* host,int port,struct sockaddr* sa)
{
	struct sockaddr_in* sa_in = (struct sockaddr_in*)sa;
	sa_in->sin_family = AF_INET;
	sa_in->sin_port = socket_htons(port);
	sa_in->sin_addr.s_addr = gethost(host);
	if(sa_in->sin_addr.s_addr==-1)return NULL;
	return sa;
}

/*!
 * @brief:			get connect messsage from addr:port string
 * @author:			xbw
 * @date:			2014_4_12
 * @args:			string, sockaddr* sa
 * @return:			sockaddr *
 */
struct sockaddr* get_sockaddr_by_string(char* str,struct sockaddr* sa)
{
	char* port_str = strstr(str,":");
	if(port_str==NULL)
	{
		return NULL;
	}
	*port_str = '\0';
	int port;
	if(sscanf(port_str+1,"%d",&port)!=1 || (port<0||port>=65536))
	{
		*port_str = ':';
		return NULL;
	}
	struct sockaddr* result = make_sockaddr(str,port,sa);
	*port_str = ':';
	return result;
}

/*!
 * @brief:			get connect messsage from url page
 * @author:			xbw
 * @date:			2014_4_13
 * @args:			url,server name,timewait of connection,sockaddr
 * @return:			sockaddr *
 */
#define HTTP_HEAD "http://"
#define URL_LEN 256
#define RECV_BUFLEN 8192
struct sockaddr* get_sockaddr_by_url(char* url,char* name,int timewait,struct sockaddr* sa)
{
	//Connect HTTP server
	int port = 80;
	if(strncmp(url,HTTP_HEAD,sizeof(HTTP_HEAD)-1)!=0)return NULL;

	char url_buf[URL_LEN] = {0};
	memcpy(url_buf,url,URL_LEN);

	char * host = url_buf + sizeof(HTTP_HEAD)-1;
	char * port_str = strstr(host,":");
	char * file_str = strstr(host,"/");

	if(port_str)port_str[0] = '\0';
	if(file_str)file_str[0] = '\0';

	if(port_str!=NULL)
	{
		port = atoi(port_str+1);
		if(port<=0 || port >= 65536)
		{
			return NULL;
		}
	}

	//Send HTTP-get
	char sndstr[URL_LEN*2 + 30]={0};
	sprintf(sndstr,"GET /%s HTTP/1.1\nHost: %s:%d\n\n\n\n",file_str!=NULL?file_str+1:" ",host,port);
	SOCKET s = tcp_connect(make_sockaddr(host,port,sa),timewait);
	if(s==-1)return NULL;
	socket_send(s,sndstr,strlen(sndstr)+1,0);

	//Read page Data
	char recv_buf[RECV_BUFLEN+1]={0};
	int namelen = strlen(name);
	int length;
	while(length = socket_recv(s,recv_buf,RECV_BUFLEN,0),length>0)
	{
		recv_buf[length]='\0';

		//get string start & end;
		char *addr,*end;
		if(
				(addr = strstr(recv_buf,name),addr==NULL)
				|| addr[namelen] != '('
				||(end = strstr(addr,")"),end==NULL)
		  )
			continue;

		//if find
		addr += namelen+1;

		//find port value
		char *c = addr;
		while(c<end)
		{
			if(*c==':')break;
			++c;
		}
		*end = '\0';
		*c = '\0';
		if(
				c<end
				&& (port = atoi(c+1),(port<=0 || port>=65536))
		  )
		{
			continue;
		}

		//Get it
		struct sockaddr_in * sa_in = (struct sockaddr_in *)sa;
		sa_in->sin_addr.s_addr = gethost(addr);
		sa_in->sin_port = socket_htons(port);
		return sa;
	}
	return NULL;
}

/*!
 * @brief:		get connect sockfd by host,port.
 * @author:		xbw
 * @date:		2014_4_12
 * @args:		struct sockaddr* sa,connect timewait
 * @return:		connect sockfd, or -1 failed
 */
SOCKET tcp_connect(struct sockaddr* sa,int time_wait)
{
	//setting connect sockaddr
	if(sa==NULL)return -1;
	SOCKET s = socket_socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(s==-1)return -1;

	//setting NBlock
	unsigned long ok = 1;
	socket_ioctl(s,FIONBIO,&ok);
	socket_connect(s,sa,sizeof(*sa));

	//check connect socketfd usable
	fd_set set;
	FD_ZERO(&set);
	FD_SET(s,&set);

	struct timeval tv;
	tv.tv_sec = time_wait;
	tv.tv_usec = 0;
	if(socket_select(0,NULL,&set,NULL,&tv)<=0)
	{
		socket_close(s);
		return -1;
	}

	//Setting NBlock
	ok = 0;
	socket_ioctl(s,FIONBIO,&ok);
	return s;
}


/*!
 * @brief:		get listen sockfd by port.
 * @author:		xbw
 * @date:		2014_4_20
 * @args:		port
 * @return:	listen sockfd, or -1 failed
 */

SOCKET tcp_listen(long port)
{
	SOCKET s = socket_socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(s==-1)return -1;

	long ip_value = socket_htonl(INADDR_ANY);
	char   name[128]; 
	if(socket_gethostname(name,   128)==0)
	{ 
		ip_value = gethost(name);
	}
	struct sockaddr_in sa_in;
	sa_in.sin_family = AF_INET;
	sa_in.sin_port = socket_htons(port);
	sa_in.sin_addr.s_addr = ip_value;

	int opt=1;
	if( socket_setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt)!=0) //reuseaddr
	||socket_bind(s,(struct sockaddr*)&sa_in,sizeof(sa_in))!=0 
	|| socket_listen(s,5)!=0)
	{
		socket_close(s);
		s = -1;
	}

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
