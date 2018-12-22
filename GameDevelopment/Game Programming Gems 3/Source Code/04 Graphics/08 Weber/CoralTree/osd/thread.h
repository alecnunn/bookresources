
/******************************************************************************

Coral Tree osd/thread.h
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
This is a set of c functions to generalize thread functionality.

Since the CThread package is supported and it is non-preemptive, wise use
of the yield functions is necessary.  The 'hard' yield does a 'yield' on
any system.  A 'soft' yield only yield on non-preemptive systems (so code
will still be portable without forcing a premptive system to yield
unnecessarily).

The 'init' function should be called as early as possible in main because
under CThreads the point of execution of the 'main' thread goes back to
the start of main during initialization.

TODO: add thread support for as many other platforms as possible

CONTROL DEFINES:
  OSD_THREADS = 
	OSD_CTHREAD_PKG
	OSD_SGI_SPROC
	OSD_WIN32_THREADS
	OSD_PTHREADS
	OSD_NO_THREADS
******************************************************************************/

#if OSD_THREADS==OSD_CTHREAD_PKG
#include "cthread.h"
#elif OSD_THREADS==OSD_SGI_SPROC
#include <sys/types.h>
#include <sys/prctl.h>
#include <sys/wait.h>
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

#if OSD_COMPILER==OSD_KR
long osdThreadInit();
void *osdThreadSpawn();
void *osdThreadWait();
#else
long osdThreadInit();
void *osdThreadSpawn( void (*callback)(void *), void *argument );
void *osdThreadWait(void *tid,long block);
#endif

#if OSD_THREADS==OSD_CTHREAD_PKG
#define osdThreadExit			cthread_exit
#define osdThreadHardYield		cthread_yield
#define osdThreadSoftYield		cthread_yield
#elif OSD_THREADS==OSD_SGI_SPROC
#define osdThreadExit 			exit
#define osdThreadHardYield()	sginap(0)
#define osdThreadSoftYield()	
#elif OSD_THREADS==OSD_PTHREADS
#define osdThreadExit(x)		pthread_exit((void *)x)
#define osdThreadHardYield()	
#define osdThreadSoftYield()	
#elif OSD_THREADS==OSD_WIN32_THREADS
#define osdThreadExit			ExitThread
#define osdThreadHardYield()	Sleep(0)
#define osdThreadSoftYield()
#elif OSD_THREADS==OSD_NO_THREADS
#define osdThreadExit(x)		/* NOOP */
#define osdThreadHardYield()	/* NOOP */
#define osdThreadSoftYield()	/* NOOP */
#endif

#define osdThreadYield osdThreadHardYield

#ifdef __cplusplus
    }
#endif

