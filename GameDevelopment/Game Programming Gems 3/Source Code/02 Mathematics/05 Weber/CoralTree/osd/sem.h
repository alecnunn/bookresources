#ifndef OSD_SEM_H
#define OSD_SEM_H

/******************************************************************************

Coral Tree osd/sem.h
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

#include <osd/osd.h>


#ifdef __cplusplus
    extern "C"
    {
#endif


typedef struct
{
#if OSD_OSTYPE==OSD_UNIX
	OSD_KEY key;
	int	number;
	int flags;
#else /* OSD_OSTYPE!=OSD_UNIX */
	LPSECURITY_ATTRIBUTES lpsa;
	LONG cSemInitial;
	LONG cSemMax;
	LPTSTR lpszSemName;
#endif /* OSD_OSTYPE!=OSD_UNIX */
} OSD_SemaphoreArgs;

typedef struct
{
	OSD_FILE_HANDLE handle;
	OSD_KEY key;
	OSD_SemaphoreArgs args;
	int maximum;
	/* .... Put more stuff here if needed. Must be common abstraction.  */
} OSD_Semaphore;

#if OSD_COMPILER==OSD_KR
int osdSemaphoreCreate();
int osdSemaphoreDestroy();
int osdSemaphoreIncrement();
int osdSemaphoreDecrement();
#else /* OSD_COMPILER!=OSD_KR */
extern int osdSemaphoreCreate(OSD_Semaphore *semaphore, OSD_KEY key,
								uint32 size, OSD_Error *error);
extern int osdSemaphoreDestroy(OSD_Semaphore *semaphore, OSD_Error *error);
extern int osdSemaphoreIncrement(OSD_Semaphore *semaphore, OSD_Error *error);
extern int osdSemaphoreDecrement(OSD_Semaphore *semaphore, OSD_Error *error);
#endif /* OSD_COMPILER!=OSD_KR */


#ifdef __cplusplus
    }
#endif


#endif /* OSD_SEM_H */
