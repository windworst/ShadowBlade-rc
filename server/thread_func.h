#pragma once

#include <windows.h>
#define THREAD_CALLBACK_FUNC(func_name) DWORD WINAPI ##func_name##(void* arg)
#define NEWTHREAD_CREATE(func,arg) CloseHandle(CreateThread(NULL,0,(func),(void*)(arg),0,NULL))
