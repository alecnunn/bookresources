
/******************************************************************************

Coral Tree osd/mutex.c
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

#if OSD_THREADS==OSD_CTHREAD_PKG
#elif OSD_THREADS==OSD_SGI_SPROC
usptr_t *osdArena = NULL;
#define ARENA_FILENAME	"./mutex_arena"
#endif

/******************************************************************************
long osdMutexCreate(OSD_Mutex *mutex)
keywords: c, blackbox_system, semaphore, thread, system

******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdMutexCreate(mutex)
	OSD_Mutex *mutex;
#else
long osdMutexCreate(OSD_Mutex *mutex)
#endif
{
#if OSD_THREADS==OSD_CTHREAD_PKG
	RESULT result;
	result = mutex_alloc(mutex, 0);
	if(result != T_SUCCEED)
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not allocate mutex");
		return FALSE;
	}
	return TRUE;
#elif OSD_THREADS==OSD_SGI_SPROC 
	if(!osdArena)
	{
		osdArena = usinit(ARENA_FILENAME);
		if(!osdArena)
		{
			osdPrint(OSD_SYSTEM,OSD_ERROR,"arena initialization failed");
			return FALSE;
		}
	}
	
	*mutex = usnewlock(osdArena);
	if(!(*mutex))
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not create mutex");
		return FALSE;
	}
	return TRUE;
#elif OSD_THREADS==OSD_PTHREADS
	int n;
	n = pthread_mutex_init(mutex, NULL);
	if(n)
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not create mutex");
		return FALSE;
	}
	return TRUE;
#elif OSD_THREADS==OSD_WIN32_THREADS
	*mutex = CreateMutex(NULL,FALSE,NULL);
	if(!(*mutex))
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not create mutex");
		return FALSE;
	}
	return TRUE;
#elif OSD_THREADS==OSD_NO_THREADS
	osdPrint(OSD_SOFTWARE,OSD_WARN,"threads not supported");
	mutex = mutex; /* shut up compiler */
	return FALSE;
#endif
}

/******************************************************************************
void osdMutexDestroy(OSD_Mutex *mutex)
keywords: c, blackbox_system, semaphore, thread, system

******************************************************************************/
#if OSD_COMPILER==OSD_KR
void osdMutexDestroy(mutex)
	OSD_Mutex *mutex;
#else
void osdMutexDestroy(OSD_Mutex *mutex)
#endif
{
#if OSD_THREADS==OSD_CTHREAD_PKG
	mutex_free(*mutex);
#elif OSD_THREAD==OSD_PTHREADS
	int n;
	n = pthread_mutex_destroy(mutex, NULL);
	if(n)
	{
	}
	return TRUE;
#else
	mutex = mutex; /* shut up compiler */
#endif
}

/******************************************************************************
long osdMutexLock(OSD_Mutex *mutex)
keywords: c, blackbox_system, semaphore, thread, system

******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdMutexLock(mutex)
	OSD_Mutex *mutex;
#else
long osdMutexLock(OSD_Mutex *mutex)
#endif
{
#if OSD_THREADS==OSD_CTHREAD_PKG
	mutex_lock(*mutex);
	return TRUE;
#elif OSD_THREADS==OSD_SGI_SPROC 
	int n;
	n = ussetlock(*mutex);
	if(!n)
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not lock mutex");
		return FALSE;
	}
	return TRUE;
#elif OSD_THREADS==OSD_PTHREADS
	int n;
	n = pthread_mutex_lock(mutex);
	if(n)
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not lock mutex");
		return FALSE;
	}
	return TRUE;
#elif OSD_THREADS==OSD_WIN32_THREADS
	DWORD n;
	n = WaitForSingleObject(*mutex,INFINITE);
	if(n != WAIT_OBJECT_0)
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not lock mutex");
		return FALSE;
	}
	return TRUE;
#elif OSD_THREADS==OSD_NO_THREADS
	osdPrint(OSD_SOFTWARE,OSD_WARN,"threads not supported");
	mutex = mutex; /* shut up compiler */
	return FALSE;
#endif
}

/******************************************************************************
long osdMutexUnlock(OSD_Mutex *mutex)
keywords: c, blackbox_system, semaphore, thread, system

******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdMutexUnlock(mutex)
	OSD_Mutex *mutex;
#else
long osdMutexUnlock(OSD_Mutex *mutex)
#endif
{
#if OSD_THREADS==OSD_CTHREAD_PKG
	mutex_unlock(*mutex);
	return TRUE;
#elif OSD_THREADS==OSD_SGI_SPROC 
	int n;
	n = usunsetlock(*mutex);
	if(n)
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not unlock mutex");
		return FALSE;
	}
	return TRUE;
#elif OSD_THREADS==OSD_PTHREADS
	int n;
	n = pthread_mutex_unlock(mutex);
	if(n)
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not lock mutex");
		return FALSE;
	}
	return TRUE;
#elif OSD_THREADS==OSD_WIN32_THREADS
	if( !ReleaseMutex(*mutex) )
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not unlock mutex");
		return FALSE;
	}
	return TRUE;
#elif OSD_THREADS==OSD_NO_THREADS
	osdPrint(OSD_SOFTWARE,OSD_WARN,"threads not supported");
	mutex = mutex; /* shut up compiler */
	return FALSE;
#endif
}


