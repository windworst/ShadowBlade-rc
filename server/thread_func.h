#pragma once

#include <windows.h>
#define THREAD_CALLBACK_FUNC(func_name) DWORD WINAPI _##func_name##_ (void* arg)
#define NEWTHREAD_CREATE(func,arg) CloseHandle(CreateThread(NULL,0,(func),(void*)(arg),0,NULL))
