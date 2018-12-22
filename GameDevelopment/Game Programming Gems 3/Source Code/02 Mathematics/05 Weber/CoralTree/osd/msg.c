
/******************************************************************************

Coral Tree osd/msg.c
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

#define OSD_MESSAGE_SIZE	0xFFF7
typedef struct
{
	int32 type;
	char data[OSD_MESSAGE_SIZE];
	uint16 length;
} OSD_Message;
static OSD_Message message;

/******************************************************************************
int osdMessageCreate(OSD_Message *messageQueue, OSD_KEY key, OSD_Error *error);
******************************************************************************/
#if OSD_COMPILER==OSD_KR
int osdMessageQueueCreate(messageQueue, key, error)
	OSD_MessageQueue *messageQueue;
	OSD_KEY key;
	OSD_Error error;
#else /* OSD_COMPILER!=OSD_KR */
int osdMessageQueueCreate(OSD_MessageQueue *messageQueue, OSD_KEY key,
							OSD_Error *error)
#endif /* OSD_COMPILER!=OSD_KR */
{
#if OSD_OSTYPE!=OSD_UNIX
	static char name[256];
#endif

	*error = OSD_ERROR_NONE;

#if OSD_OSTYPE==OSD_UNIX
	messageQueue->args.key = key;
	messageQueue->args.flags = IPC_CREAT;
	messageQueue->handle = msgget(messageQueue->args.key,
									messageQueue->args.flags);
	if (OSD_INVALID_HANDLE == messageQueue->handle)
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
#else /* OSD_OSTYPE!=OSD_UNIX */
	/* Windows requires a specific prefix for the pipe name. */
	sprintf(name, "\\\\.\\pipe\\%d", key);
	messageQueue->args.lpszPipeName = (LPTSTR) name;
	messageQueue->args.fdwOpenMode = PIPE_ACCESS_DUPLEX;
	messageQueue->args.fdwPipeMode = PIPE_TYPE_BYTE | PIPE_READMODE_BYTE;
	messageQueue->args.nMaxInstances = PIPE_UNLIMITED_INSTANCES;
	messageQueue->args.cbOutBuf = 0xFFFF;
	messageQueue->args.cbInBuf = 0xFFFF;
	messageQueue->args.dwTimeout = 0;
	messageQueue->args.lpsa = (LPSECURITY_ATTRIBUTES) NULL; 
	messageQueue->handle = CreateNamedPipe(messageQueue->args.lpszPipeName,
											messageQueue->args.fdwOpenMode,
											messageQueue->args.fdwPipeMode,
											messageQueue->args.nMaxInstances,
											messageQueue->args.cbOutBuf,
											messageQueue->args.cbInBuf,
											messageQueue->args.dwTimeout,
											messageQueue->args.lpsa);
	if (OSD_INVALID_HANDLE == messageQueue->handle)
	{
		CloseHandle(messageQueue->handle);
		messageQueue->handle = OSD_INVALID_HANDLE;
		*error = OSD_ERROR_OS_CREATE_FAIL;
		return -1;
	}
#endif /* OSD_OSTYPE!=OSD_UNIX */
	messageQueue->key = key;
	return 0;
} /* osdMessageCreate() */

/******************************************************************************
int osdMessageDestroy(OSD_Message *messageQueue, OSD_Error *error);
******************************************************************************/
#if OSD_COMPILER==OSD_KR
int osdMessageQueueDestroy(messageQueue, error)
	OSD_MessageQueue *messageQueue;
	OSD_Error *error;
#else /* OSD_COMPILER!=OSD_KR */
int osdMessageQueueDestroy(OSD_MessageQueue *messageQueue, OSD_Error *error)
#endif /* OSD_COMPILER!=OSD_KR */
{
	int status;

	status = 0;
	*error = OSD_ERROR_NONE;

#if OSD_OSTYPE==OSD_UNIX
	status = msgctl(messageQueue->handle, IPC_RMID, NULL);
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
		} /* switch */
	}
#else /* OSD_OSTYPE!=OSD_UNIX */
	CloseHandle(messageQueue->handle);
#endif /* OSD_OSTYPE!=OSD_UNIX */
	return status;
} /* osdMessageDestroy() */

/***********************************************************************
int osdMessageQueueSend(OSD_MessageQueue *messageQueue, void *data,
						uint16 length, OSD_Error *error);
***********************************************************************/
#if OSD_COMPILER==OSD_KR
int osdMessageQueueSend(messageQueue, data, length, error)
	OSD_MessageQueue *messageQueue;
	void *data;
	uint16 length;
	OSD_Error *error;
#else /* OSD_COMPILER!=OSD_KR */
int osdMessageQueueSend(OSD_MessageQueue *messageQueue, void *data,
						uint16 length, OSD_Error *error)
#endif /* OSD_COMPILER!=OSD_KR */
{
#if OSD_OSTYPE!=OSD_UNIX
	DWORD n;
#endif /* OSD_OSTYPE!=OSD_UNIX */
	*error = OSD_ERROR_NONE;

#if OSD_OSTYPE==OSD_UNIX
	message.type = 0;
	message.length = (length < OSD_MESSAGE_SIZE) ? length : OSD_MESSAGE_SIZE;
	osdMemcpy(message.data, data, message.length);
	msgsnd(messageQueue->handle, &message, sizeof(message), 0);
#else /* OSD_OSTYPE!=OSD_UNIX */
	if (!WriteFile(messageQueue->handle, data, length, &n, NULL) ||
		length != n)
	{
		return -1;
	}
#endif /* OSD_OSTYPE!=OSD_UNIX */
	return 0;
}

/***********************************************************************
int osdMessageQueueReceive(OSD_MessageQueue *messageQueue, void *data,
							uint16 *length, OSD_Error *error);
***********************************************************************/
#if OSD_COMPILER==OSD_KR
int osdMessageQueueReceive(messageQueue, data, length, error)
	OSD_MessageQueue *messageQueue;
	void *data;
	uint16 *length;
	OSD_Error *error;
#else /* OSD_COMPILER!=OSD_KR */
int osdMessageQueueReceive(OSD_MessageQueue *messageQueue, void *data,
							uint16 *length, OSD_Error *error)
#endif /* OSD_COMPILER!=OSD_KR */
{
#if OSD_OSTYPE==OSD_UNIX
	int n;
#else /* OSD_OSTYPE!=OSD_UNIX */
	DWORD numberOfBytesRead;
#endif /* OSD_OSTYPE!=OSD_UNIX */

	*error = OSD_ERROR_NONE;

#if OSD_OSTYPE==OSD_UNIX
	message.type = 0;
	message.length = OSD_MESSAGE_SIZE;
	n = msgrcv(messageQueue->handle, &message, sizeof(message),
				message.type, 0);
	if (-1 == n)
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
	*length = (n < *length) ? n : *length;
	osdMemcpy(data, message.data, *length);
#else /* OSD_OSTYPE!=OSD_UNIX */
	numberOfBytesRead = 0;
	if (!ReadFile(messageQueue->handle, data, OSD_MESSAGE_SIZE, &numberOfBytesRead, NULL))
	{
		return -1;
	}
	*length = (uint16) numberOfBytesRead;
#endif /* OSD_OSTYPE!=OSD_UNIX */
	return 0;
}

