
/******************************************************************************

Coral Tree osd/thread.c
Copyright (C) 1998 Infinite Monkeys Incorporated

This program is free software; you can redistribute it and/or modify
it under the terms of the Artistic License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Artistic License for more details.

You should have received a copy of the Artistic License along with
this program (see meta/ART_LICE); if not, write to coral@imonk.com.

******************************************************************************/

#include "osd.h"

/******************************************************************************
long osdThreadInit()
keywords: blackbox_system, system, thread, c

******************************************************************************/
#if OSD_THREADS==OSD_PTHREADS
struct timespec osd_global_zero_time = {0,0};
#endif

long osdThreadInit()
{
#if OSD_THREADS==OSD_CTHREAD_PKG
	int n;
	n = cthread_init(1, NULL);
	if(n == T_ALREADY_INIT)
		return 1;
	else
		return 0;
#elif OSD_THREADS==OSD_SGI_SPROC
	return 1;
#elif OSD_THREADS==OSD_PTHREADS
	return 1;
#elif OSD_THREADS==OSD_WIN32_THREADS
	return 1;
#elif OSD_THREADS==OSD_NO_THREADS
	return 1;
#endif
}



/******************************************************************************
void *osdThreadSpawn( void (*callback)(void *), void *argument )
keywords: blackbox_system, system, thread, c

******************************************************************************/

#if OSD_THREADS==OSD_PTHREADS
struct osd_pthread_relayinfo
{
	void (*callback)(void *);
	void *argument;
};

void *osd_pthread_relay_callback(void *func_info)
{
	struct osd_pthread_relayinfo *relay_info;
	void (*callback)(void *);
	void *argument;
	relay_info = (struct osd_pthread_relayinfo *)func_info;
	callback = relay_info->callback;
	argument = relay_info->argument;
	osdFree(relay_info);
	callback(argument);
	return NULL;
}
#endif

#if OSD_COMPILER==OSD_KR
void *osdThreadSpawn( callback, argument )
	void (*callback)(); 
	void *argument;
#else
void *osdThreadSpawn( void (*callback)(void *), void *argument )
#endif
{
#if OSD_THREADS==OSD_CTHREAD_PKG
	cthread_t n;
#ifdef _cplusplus
	n = cthread_fork((any_t(*)(...))callback,(void *)argument,0);
#else
	n = cthread_fork((any_t(*)())callback,(void *)argument,0);
#endif
	if(n)
		return (void *)n;
	else
		return NULL;
#elif OSD_THREADS==OSD_SGI_SPROC
	int n;
	n = sproc(callback,PR_SADDR,argument);
	if(n == -1)
		return NULL;
	else
		return (void *)n;
#elif OSD_THREADS==OSD_PTHREADS
	int n;
	pthread_attr_t  attr;
	pthread_t   tid;
	struct osd_pthread_relayinfo *relay_info;
	relay_info = (struct osd_pthread_relayinfo *)osdMalloc(sizeof(struct osd_pthread_relayinfo));
	if(!relay_info)
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"out of memory");
	}
	relay_info->callback = callback;
	relay_info->argument = argument;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE);
/*	if ( n = pthread_create( &tid, &attr,&osd_pthread_relay_callback, (void *)relay_info) ) */
	if ( n = pthread_create( &tid, NULL,&osd_pthread_relay_callback, (void *)relay_info) ) 
	{
		tid = NULL;
	}

	pthread_attr_destroy(&attr);

	return (void *)tid;
#elif OSD_THREADS==OSD_WIN32_THREADS
	HANDLE h;
	DWORD dwThreadId;
	h = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE) callback,
		(LPVOID) argument,
		0,
		&dwThreadId);

	if(h)
		return (void *)h;
	else
		return NULL;

#elif OSD_THREADS==OSD_NO_THREADS

	return NULL;
		
#endif
}



/******************************************************************************
void	*osdThreadWait(void *tid,long block)

	wait on thread identified by tid (0 for any)
TODO: fix documentation...cannot wait on tid 0

	this tid identifer should only be used in OSD functions,
	not directly by applications

	return tid of process waited for

Keywords:
******************************************************************************/
#if OSD_COMPILER==OSD_KR
void *osdThreadWait(tid,block)
	void *tid;
	long block;
#else
void *osdThreadWait(void *tid,long block)
#endif
	{
#if OSD_THREADS == OSD_SGI_SPROC

	pid_t pid;
	int status;

	long wid=(long)tid;
	if(wid<1)
		wid= -1;

	/* POSIX child wait */
	pid=waitpid(wid, &status, block? NULL: WNOHANG);

	if(pid>0)
		return (void *)pid;
	else
		return NULL;

#elif OSD_THREADS == OSD_PTHREADS
	int n;
	if ( n = pthread_join( (pthread_t)tid, NULL ) ) 
	{
		return NULL;
	}
	else
	{
		return tid;
	}

#elif OSD_OSTYPE == OSD_WIN32

	/*TODO: This is a quick & dirty spinlock....replace*/
	HANDLE h;
	DWORD exit_code;
	h = (HANDLE)tid;
	while(TRUE)
	{
		if(GetExitCodeThread(h, &exit_code))
		{
			if(exit_code != STILL_ACTIVE)
			{
				break;
			}
		}
		else
		{
			osdPrint(OSD_SYSTEM,OSD_ERROR,"thread query failed");
			return NULL;
		}
	}

	return tid;
	
#else

	return NULL;

#endif
	}

