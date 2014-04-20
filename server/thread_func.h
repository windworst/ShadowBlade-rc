#pragma once

#include <windows.h>
#define THREAD_CALLBACK(func_name) _##func_name##_
#define THREAD_CALLBACK_FUNC(func_name) DWORD WINAPI _##func_name##_ (void* arg)

typedef  HANDLE thread_instance;
typedef void* thread_arg;
typedef DWORD WINAPI (*thread_callback)(thread_arg arg);

int  thread_create(thread_instance *t, thread_callback func, thread_arg arg); 
void thread_close(thread_instance *t);
int thread_join(thread_instance *t);

#define time_wait(x) Sleep(x)
