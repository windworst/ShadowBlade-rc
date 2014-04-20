#include "thread_func.h"

/*!
 *	@brief:		create a thread
 *	@author:	xbw
 *	@date:		2014_4_20
 *	@args:		thread_instance* , callback, callback-arg
 *	@return:	success  =1,failed =0;
 */
int  thread_create(thread_instance *t, thread_callback func, thread_arg arg)
{
	//too lazy to check null-ptr;
	*t = CreateThread(NULL,0,func,arg,0,NULL);
	if(*t==NULL)return 0;
	return 1;
}

/*!
 *	@brief:		close a thread
 *	@author:	xbw
 *	@date:		2014_4_20
 *	@args:		thread_instance* 
 */
void thread_close(thread_instance *t)
{
	//too lazy to check null-ptr;
	CloseHandle(*t);
}

/*!
 *	@brief:		join a thread,waiting for thread exit
 *	@author:	xbw
 *	@date:		2014_4_20
 *	@return:	 Windows API WaitForSingleObject's return.
 */
int thread_join(thread_instance *t)
{
	//too lazy to check null-ptr;
	return WaitForSingleObject(*t,INFINITE);
}