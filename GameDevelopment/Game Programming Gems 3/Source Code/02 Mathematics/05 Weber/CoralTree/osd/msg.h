#ifndef OSD_MSG_H
#define OSD_MSG_H

/******************************************************************************

Coral Tree osd/msg.h
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
#else /* OSD_OSTYPE!=OSD_UNIX */
	LPTSTR lpszPipeName;
	DWORD fdwOpenMode;
	DWORD fdwPipeMode;
	DWORD nMaxInstances;
	DWORD cbOutBuf;
	DWORD cbInBuf;
	DWORD dwTimeout;
	LPSECURITY_ATTRIBUTES lpsa;
#endif /* OSD_OSTYPE!=OSD_UNIX */
} OSD_MessageQueueArgs;

typedef struct
{
	OSD_FILE_HANDLE handle;
	OSD_KEY key;
	OSD_MessageQueueArgs args;
	/* .... Put more stuff here if needed. Must be common abstraction.  */
} OSD_MessageQueue;

#if OSD_COMPILER==OSD_KR
int osdMessageQueueCreate();
int osdMessageQueueDestroy();
int osdMessageQueueSend();
int osdMessageQueueReceive();
#else /* OSD_COMPILER!=OSD_KR */
extern int osdMessageQueueCreate(OSD_MessageQueue *messageQueue, OSD_KEY key,
									OSD_Error *error);
extern int osdMessageQueueDestroy(OSD_MessageQueue *messageQueue,
									OSD_Error *error);
extern int osdMessageQueueSend(OSD_MessageQueue *messageQueue,
								void *data, uint16 length,
								OSD_Error *error);
extern int osdMessageQueueReceive(OSD_MessageQueue *messageQueue,
									void *data, uint16 *length,
									OSD_Error *error);
#endif /* OSD_COMPILER!=OSD_KR */


#ifdef __cplusplus
    }
#endif


#endif /* OSD_MSG_H */
