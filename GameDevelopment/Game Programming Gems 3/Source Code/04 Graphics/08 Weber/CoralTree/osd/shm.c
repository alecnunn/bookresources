
/******************************************************************************

Coral Tree osd/shm.c
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

/******************************************************************************
int osdSharedMemoryCreate(OSD_SharedMemory *sharedMemory, OSD_KEY key,
							uint32 size, OSD_Error *error);
******************************************************************************/
#if OSD_COMPILER==OSD_KR
int osdSharedMemoryCreate(sharedMemory, key, size, error)
	OSD_SharedMemory *sharedMemory;
	OSD_KEY key;
	uint32 size;
	OSD_Error *error;
#else /* OSD_COMPILER!=OSD_KR */
int osdSharedMemoryCreate(OSD_SharedMemory *sharedMemory, OSD_KEY key,
							uint32 size, OSD_Error *error)
#endif /* OSD_COMPILER!=OSD_KR */
{
#if OSD_OSTYPE==OSD_UNIX
	uint8 *address;
#else
	static char name[256];
#endif /* OSD_OSTYPE==OSD_UNIX */

	*error = OSD_ERROR_NONE;

#if OSD_OSTYPE==OSD_UNIX
	sharedMemory->args.key = key;
	sharedMemory->args.size = size;
	sharedMemory->args.flags = 0777|IPC_CREAT|IPC_EXCL;
	sharedMemory->handle = shmget(sharedMemory->args.key,
									sharedMemory->args.size,
									sharedMemory->args.flags);
	if (OSD_INVALID_HANDLE == sharedMemory->handle)
	{
		switch(OSD_ERRNO)
		{
		case EEXIST: /* Falls through... */
#if OSD_OS!=OSD_FBSD
		case EIDRM: /* Falls through... */
#endif
		case ENOSPC: /* Falls through... */
		case ENOENT: /* Falls through... */
		case EACCES: /* Falls through... */
		case ENOMEM:
			*error = OSD_ERROR_OS_CREATE_FAIL;
			break;
		default:
			*error = OSD_ERROR_UNKNOWN;
			break;
		}
		return -1;
	}
	address = (uint8 *)shmat(sharedMemory->handle, (void *) 0, 0);
	if (-1 == (int) address)
	{
		*error = OSD_ERROR_UNKNOWN;
		return -1;
	}
	osdMemset(address, 0, sharedMemory->args.size);
	if (-1 == shmdt((void *)address))
	{
		*error = OSD_ERROR_UNKNOWN;
		return -1;
	}
#else /* OSD_OSTYPE!=OSD_UNIX */
	sharedMemory->args.hFile = (HANDLE) 0xFFFFFFFF;
	sharedMemory->args.lpsa = (LPSECURITY_ATTRIBUTES) NULL;
	sharedMemory->args.fdwProtect = PAGE_READWRITE;
	sharedMemory->args.dwMaximumSizeHigh = 0;
	sharedMemory->args.dwMaximumSizeLow = size;
	sprintf(name, "%d", key);
	sharedMemory->args.lpszMapName = (LPTSTR) name;
	sharedMemory->handle = CreateFileMapping(sharedMemory->args.hFile,
											sharedMemory->args.lpsa,
											sharedMemory->args.fdwProtect,
											sharedMemory->args.dwMaximumSizeHigh,
											sharedMemory->args.dwMaximumSizeLow,
											sharedMemory->args.lpszMapName);
	if (OSD_INVALID_HANDLE == sharedMemory->handle)
	{
		CloseHandle(sharedMemory->handle);
		sharedMemory->handle = OSD_INVALID_HANDLE;
		*error = OSD_ERROR_OS_CREATE_FAIL;
		return -1;
	}
#endif /* OSD_OSTYPE!=OSD_UNIX */
	sharedMemory->key = key;
	return 0;
}

/******************************************************************************
int osdSharedMemoryDestroy(OSD_SharedMemory *sharedMemory, OSD_Error *error);
******************************************************************************/
#if OSD_COMPILER==OSD_KR
int osdSharedMemoryDestroy(sharedMemory, error)
	OSD_SharedMemory *sharedMemory;
	OSD_Error *error;
#else /* OSD_COMPILER!=OSD_KR */
int osdSharedMemoryDestroy(OSD_SharedMemory *sharedMemory, OSD_Error *error)
#endif /* OSD_COMPILER!=OSD_KR */
{
	int status;

	status = 0;
	*error = OSD_ERROR_NONE;

#if OSD_OSTYPE==OSD_UNIX
	status = shmctl(sharedMemory->handle, IPC_RMID, NULL);
	if (-1 == status)
	{
		switch(OSD_ERRNO)
		{
		case EACCES: /* Falls through... */
		case EFAULT: /* Falls through... */
		case EINVAL: /* Falls through... */
		case EPERM: /* Falls through... */
#if OSD_OS!=OSD_FBSD
		case EIDRM:
#endif
			*error = OSD_ERROR_OS_DESTROY_FAIL;
			break;
		default:
			*error = OSD_ERROR_UNKNOWN;
			break;
		}
	}
#else /* OSD_OSTYPE!=OSD_UNIX */
	CloseHandle(sharedMemory->handle);
#endif /* OSD_OSTYPE!=OSD_UNIX */
	return status;
}

/***********************************************************************
int osdSharedMemoryAttach(OSD_SharedMemory *sharedMemory, void **address,
							OSD_Error *error);
***********************************************************************/
#if OSD_COMPILER==OSD_KR
int osdSharedMemoryAttach(sharedMemory, address, error)
	OSD_SharedMemory *sharedMemory;
	void **address;
	OSD_Error *error;
#else /* OSD_COMPILER!=OSD_KR */
int osdSharedMemoryAttach(OSD_SharedMemory *sharedMemory, void **address,
							OSD_Error *error)
#endif /* OSD_COMPILER!=OSD_KR */
{
#if OSD_OSTYPE==OSD_UNIX
	*address = (void *) shmat(sharedMemory->handle, (void *) 0, 0);
	if (-1 == (int) *address)
	{
		switch(OSD_ERRNO)
		{
		/*TODO: Handle other values of errno. */
		default:
			*error = OSD_ERROR_UNKNOWN;
			break;
		}
		return -1;
	}
#else /* OSD_OSTYPE!=OSD_UNIX */
	*address = MapViewOfFile(sharedMemory->handle, FILE_MAP_WRITE, 0, 0, 0);
	if ((void *) 0 == *address)
	{
		return -1;
	}
#endif /* OSD_OSTYPE!=OSD_UNIX */
	return 0;
}

/***********************************************************************
int osdSharedMemoryDetach(OSD_SharedMemory *sharedMemory, void **address,
							OSD_Error *error);
***********************************************************************/
#if OSD_COMPILER==OSD_KR
int osdSharedMemoryDetach(sharedMemory, address, error)
	OSD_SharedMemory *sharedMemory;
	void *address;
	OSD_Error *error;
#else /* OSD_COMPILER!=OSD_KR */
int osdSharedMemoryDetach(OSD_SharedMemory *sharedMemory, void *address,
							OSD_Error *error)
#endif /* OSD_COMPILER!=OSD_KR */
{
#if OSD_OSTYPE==OSD_UNIX
	if (-1 == shmdt(address))
	{
		switch(OSD_ERRNO)
		{
		/*TODO: Handle other values of errno. */
		default:
			*error = OSD_ERROR_UNKNOWN;
			break;
		}
		return -1;
	}
#else /* OSD_OSTYPE!=OSD_UNIX */
	if (!UnmapViewOfFile(address))
	{
		return -1;
	}
#endif /* OSD_OSTYPE!=OSD_UNIX */
	return 0;
}

