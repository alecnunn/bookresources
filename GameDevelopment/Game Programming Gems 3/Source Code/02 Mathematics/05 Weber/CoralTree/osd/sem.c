
/******************************************************************************

Coral Tree osd/sem.c
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
#if OSD_OS==OSD_LINUX
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
	int val;                    /* value for SETVAL */
	struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
	unsigned short int *array;  /* array for GETALL, SETALL */
	struct seminfo *__buf;      /* buffer for IPC_INFO */
};
#endif
#endif

/******************************************************************************
int osdSemaphoreCreate(OSD_Semaphore *semaphore, OSD_KEY key,
						uint32 size, OSD_Error *error);
******************************************************************************/
#if OSD_COMPILER==OSD_KR
int osdSemaphoreCreate(semaphore, key, size, error)
	OSD_Semaphore *semaphore;
	OSD_KEY key;
	uint32 size;
	OSD_Error *error;
#else /* OSD_COMPILER!=OSD_KR */
int osdSemaphoreCreate(OSD_Semaphore *semaphore, OSD_KEY key,
						uint32 size, OSD_Error *error)
#endif /* OSD_COMPILER!=OSD_KR */
{
#if OSD_OSTYPE==OSD_UNIX
	struct sembuf buf;
#else
	static char name[256];
#endif /* OSD_OSTYPE==OSD_UNIX */

	*error = OSD_ERROR_NONE;

#if OSD_OSTYPE==OSD_UNIX
	semaphore->args.key = key;
	semaphore->args.number = 1;
	semaphore->args.flags = 0777|IPC_CREAT|IPC_EXCL;
	semaphore->handle = semget(semaphore->args.key, semaphore->args.number,
								semaphore->args.flags);
	if (OSD_INVALID_HANDLE == semaphore->handle)
	{
		switch(OSD_ERRNO)
		{
		case EEXIST: /* Falls through... */
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
	buf.sem_num = 0;
	buf.sem_op = size;
	buf.sem_flg = SEM_UNDO;
	if (-1 == semop(semaphore->handle, &buf, 1))
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
	semaphore->args.lpsa = (LPSECURITY_ATTRIBUTES) NULL;
  	semaphore->args.cSemInitial = 0;
  	semaphore->args.cSemMax = size;
	sprintf(name, "%d", key);
  	semaphore->args.lpszSemName = (LPTSTR) name;
	semaphore->handle = CreateSemaphore(semaphore->args.lpsa,
										semaphore->args.cSemInitial,
										semaphore->args.cSemMax,
										semaphore->args.lpszSemName);
	if (OSD_INVALID_HANDLE == semaphore->handle)
	{
		CloseHandle(semaphore->handle);
		semaphore->handle = OSD_INVALID_HANDLE;
		*error = OSD_ERROR_OS_CREATE_FAIL;
		return -1;
	}
#endif /* OSD_OSTYPE!=OSD_UNIX */
	semaphore->key = key;
	return 0;
}

/******************************************************************************
int osdSemaphoreDestroy(OSD_Semaphore *semaphore, OSD_Error *error);
******************************************************************************/
#if OSD_COMPILER==OSD_KR
int osdSemaphoreDestroy(semaphore, error)
	OSD_Semaphore *semaphore;
	OSD_Error *error;
#else /* OSD_COMPILER!=OSD_KR */
int osdSemaphoreDestroy(OSD_Semaphore *semaphore, OSD_Error *error)
#endif /* OSD_COMPILER!=OSD_KR */
{
	int status;

	status = 0;
	*error = OSD_ERROR_NONE;

#if OSD_OSTYPE==OSD_UNIX
#if OSD_OS==OSD_LINUX
	status = semctl(semaphore->handle, 0, IPC_RMID, (union semun)0);
#else
	status = semctl(semaphore->handle, 0, IPC_RMID, 0);
#endif
	if (-1 == status)
	{
		switch(OSD_ERRNO)
		{
		case EACCES: /* Falls through... */
		case EFAULT: /* Falls through... */
		case EINVAL: /* Falls through... */
		case EPERM:
			*error = OSD_ERROR_OS_DESTROY_FAIL;
			break;
		default:
			*error = OSD_ERROR_UNKNOWN;
			break;
		}
	}
#else /* OSD_OSTYPE!=OSD_UNIX */
	CloseHandle(semaphore->handle);
#endif /* OSD_OSTYPE!=OSD_UNIX */
	return status;
}

/***********************************************************************
int osdSemaphoreIncrement(OSD_Semaphore *semaphore, OSD_Error *error);
***********************************************************************/
#if OSD_COMPILER==OSD_KR
int osdSemaphoreIncrement(semaphore, error)
	OSD_Semaphore *semaphore;
	OSD_Error *error;
#else /* OSD_COMPILER!=OSD_KR */
int osdSemaphoreIncrement(OSD_Semaphore *semaphore, OSD_Error *error)
#endif /* OSD_COMPILER!=OSD_KR */
{
#if OSD_OSTYPE==OSD_UNIX
	struct sembuf buf;
#endif /* OSD_OSTYPE==OSD_UNIX */

	*error = OSD_ERROR_NONE;

#if OSD_OSTYPE==OSD_UNIX
	buf.sem_num = 0;
	buf.sem_op = -1;
	buf.sem_flg |= SEM_UNDO;
	if (-1 == semop(semaphore->handle, &buf, 1))
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
	if (WAIT_OBJECT_0 != WaitForSingleObject(semaphore->handle, INFINITE))
	{
		return -1;
	}
#endif /* OSD_OSTYPE!=OSD_UNIX */
	return 0;
}

/***********************************************************************
int osdSemaphoreDecrement(OSD_Semaphore *semaphore, OSD_Error *error);
***********************************************************************/
#if OSD_COMPILER==OSD_KR
int osdSemaphoreDecrement(semaphore, error)
	OSD_Semaphore *semaphore;
	OSD_Error *error;
#else /* OSD_COMPILER!=OSD_KR */
int osdSemaphoreDecrement(OSD_Semaphore *semaphore, OSD_Error *error)
#endif /* OSD_COMPILER!=OSD_KR */
{
#if OSD_OSTYPE==OSD_UNIX
	struct sembuf buf;
#endif /* OSD_OSTYPE==OSD_UNIX */

	*error = OSD_ERROR_NONE;

#if OSD_OSTYPE==OSD_UNIX
	buf.sem_num = 0;
	buf.sem_op = 1;
	buf.sem_flg |= SEM_UNDO;
	if (-1 == semop(semaphore->handle, &buf, 1))
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
	if (!ReleaseSemaphore(semaphore->handle, 1, NULL))
	{
		return -1;
	}
#endif /* OSD_OSTYPE!=OSD_UNIX */
	return 0;
}

