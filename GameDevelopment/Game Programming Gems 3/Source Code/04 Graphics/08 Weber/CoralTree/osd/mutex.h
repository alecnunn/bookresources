
/******************************************************************************

Coral Tree osd/mutex.h
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
/******************************************************************************
OSD_Mutex is a lightweight mutex, implemented with whatever lightweight
facility is avaiable on the supporting system.  Typically this is tied to
the thread support of the underlying system and is some form of spinlock.
******************************************************************************/

#if OSD_THREADS==OSD_CTHREAD_PKG
#include "cthread.h"
#elif OSD_THREADS==OSD_SGI_SPROC
#include <ulocks.h> 
#elif OSD_THREADS==OSD_PTHREADS
#include <pthread.h>
#elif OSD_THREADS==OSD_WIN32_THREADS || OSD_THREADS==OSD_NO_THREADS
#else
#define OSD_THREADS OSD_NO_THREADS
#endif


#ifdef __cplusplus
    extern "C"
    {
#endif


#if OSD_THREADS==OSD_CTHREAD_PKG
	typedef mutex_t OSD_Mutex;
#elif OSD_THREADS==OSD_SGI_SPROC
	extern usptr_t  *osdArena;
	typedef ulock_t OSD_Mutex;
#elif OSD_THREADS==OSD_PTHREADS
	typedef pthread_mutex_t	OSD_Mutex;
#elif OSD_THREADS==OSD_WIN32_THREADS
	typedef HANDLE	OSD_Mutex;
#elif OSD_THREADS==OSD_NO_THREADS
	typedef char	OSD_Mutex; /* a dummy value */
#endif


#if OSD_COMPILER==OSD_KR
long 		osdMutexCreate();
void 		osdMutexDestroy();
long		osdMutexLock();
long		osdMutexUnlock();
#else
long 		osdMutexCreate(OSD_Mutex *mutex);
void 		osdMutexDestroy(OSD_Mutex *mutex);
long		osdMutexLock(OSD_Mutex *mutex);
long		osdMutexUnlock(OSD_Mutex *mutex);
#endif


#ifdef __cplusplus
    }
#endif
