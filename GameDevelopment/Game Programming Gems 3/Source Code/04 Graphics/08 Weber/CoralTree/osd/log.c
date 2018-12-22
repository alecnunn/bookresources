/******************************************************************************

Coral Tree osd/log.c
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

#if OSD_OSTYPE==OSD_WIN32
static OSD_HANDLE eventLogHandle = OSD_INVALID_HANDLE;
#endif

/***********************************************************************
int osdSystemLogInitialize(const char *ident);
***********************************************************************/
int osdSystemLogInitialize(const char *ident)
{
#if OSD_OSTYPE==OSD_WIN32
	eventLogHandle = OpenEventLog((LPTSTR) NULL, (LPTSTR) "Application");
	if (OSD_INVALID_HANDLE == eventLogHandle)
	{
		return -1;
	}
	return 0;
#elif OSD_OSTYPE==OSD_UNIX
	openlog(ident, LOG_CONS, LOG_USER);
	return 0;
#else
	return -1;
#endif
}

/***********************************************************************
int osdSystemLogFinalize(void);
***********************************************************************/
int osdSystemLogFinalize(void)
{
#if OSD_OSTYPE==OSD_WIN32
	if (!CloseEventLog(eventLogHandle))
	{
		return -1;
	}
	return 0;
#elif OSD_OSTYPE==OSD_UNIX
	closelog();
	return 0;
#else
	return -1;
#endif
}

/***********************************************************************
int osdSystemLogRecordEvent(const int eventType, const char *string)
***********************************************************************/
int osdSystemLogRecordEvent(const int eventType, const char *string)
{
#if OSD_OSTYPE==OSD_WIN32
	if (!ReportEvent(eventLogHandle, eventType, 0, 0, (PSID) NULL,
					1, 0, (LPTSTR *) &string, (LPVOID) NULL))
	{
		return -1;
	}
	return 0;
#elif OSD_OSTYPE==OSD_UNIX
	syslog(eventType, "%s", string);
	return 0;
#else
	return -1;
#endif
}
