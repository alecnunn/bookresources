#ifndef OSD_SHM_H
#define OSD_SHM_H

/******************************************************************************

Coral Tree osd/shm.h
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
	int flags;
	int size;
#else /* OSD_OSTYPE!=OSD_UNIX */
	HANDLE hFile;
	LPSECURITY_ATTRIBUTES lpsa;
	DWORD fdwProtect;
	DWORD dwMaximumSizeHigh;
	DWORD dwMaximumSizeLow;
	LPTSTR lpszMapName;
#endif /* OSD_OSTYPE!=OSD_UNIX */
} OSD_SharedMemoryArgs;

typedef struct
{
	OSD_FILE_HANDLE handle;
	OSD_KEY key;
	OSD_SharedMemoryArgs args;
	int maximum;
	int minimum;
	/* .... Put more stuff here if needed. Must be common abstraction.  */
} OSD_SharedMemory;

#if OSD_COMPILER==OSD_KR
int osdSharedMemoryCreate();
int osdSharedMemoryDestroy();
int osdSharedMemoryAttach();
int osdSharedMemoryDetach();
#else /* OSD_COMPILER!=OSD_KR */
extern int osdSharedMemoryCreate(OSD_SharedMemory *sharedMemory, OSD_KEY key,
									uint32 size, OSD_Error *error);
extern int osdSharedMemoryDestroy(OSD_SharedMemory *sharedMemory,
									OSD_Error *error);
extern int osdSharedMemoryAttach(OSD_SharedMemory *sharedMemory,
									void **address, OSD_Error *error);
extern int osdSharedMemoryDetach(OSD_SharedMemory *sharedMemory,
									void *address, OSD_Error *error);
#endif /* OSD_COMPILER!=OSD_KR */


#ifdef __cplusplus
    }
#endif


#endif /* OSD_SHM_H */
