#ifndef OSD_LOG_H
#define OSD_LOG_H

/******************************************************************************

Coral Tree osd/log.h
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

#if OSD_OSTYPE==OSD_WIN32
#define	OSD_SYSTEMLOG_ERROR	EVENTLOG_ERROR_TYPE
#define	OSD_SYSTEMLOG_WARN	EVENTLOG_WARNING_TYPE
#define	OSD_SYSTEMLOG_INFO	EVENTLOG_INFORMATION_TYPE
#elif OSD_OSTYPE==OSD_UNIX
#define	OSD_SYSTEMLOG_ERROR	LOG_ERR
#define	OSD_SYSTEMLOG_WARN	LOG_WARNING
#define	OSD_SYSTEMLOG_INFO	LOG_INFO
#endif

#ifdef __cplusplus
    extern "C"
    {
#endif

extern int osdSystemLogInitialize(const char *ident);
extern int osdSystemLogFinalize(void);
extern int osdSystemLogRecordEvent(const int eventType, const char *string);

#ifdef __cplusplus
    }
#endif

#endif /* OSD_LOG_H */
