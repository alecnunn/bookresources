/******************************************************************************

Coral Tree osd/time.c
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

#if OSD_COMPILER==OSD_KR
long osdTimeGet(osdtime)
	OSD_Time *osdtime;
#else
long osdTimeGet(OSD_Time *osdtime)
#endif
{
#if OSD_OSTYPE==OSD_UNIX
	static struct timeval timer;

	osdMemset(&timer, 0, sizeof(timer));
#if OSD_OS==OSD_SUNOS || OSD_OS==OSD_HPUX || OSD_OS==OSD_LINUX || OSD_OS==OSD_LYNX || OSD_OS==OSD_SCO || OSD_OS==OSD_FBSD
	if( gettimeofday(&timer,NULL) )
#else
	if( gettimeofday(&timer) )
#endif
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not get time of day");
		return FALSE;
	}

	osdTimeConvert(osdtime, &(timer.tv_sec));
	osdtime->millisecond = timer.tv_usec/1000;

	return TRUE;
	
#elif OSD_OSTYPE==OSD_WIN32 /* begin NT-Specific section */
	static SYSTEMTIME time_struct;

	osdMemset(&time_struct, 0, sizeof(time_struct));
	GetSystemTime(&time_struct);
	osdTimeConvert(osdtime, &time_struct);

	return TRUE;
#endif
}

#if OSD_COMPILER==OSD_KR
void osdTimeConvert(osdtime, systemtime)
	OSD_Time *osdtime;
	OSD_TIME *systemtime;
#else /* OSD_COMPILER!=OSD_KR */
void osdTimeConvert(OSD_Time *osdtime, OSD_TIME *systemtime)
#endif /* OSD_COMPILER!=OSD_KR */
{
#if OSD_OSTYPE==OSD_UNIX
	struct tm *time_struct;

	/* On Suns, Purify reports UMR errors on this call. Ignore them. */
	time_struct = gmtime(systemtime);

	osdtime->year = time_struct->tm_year + 1900;
	osdtime->month = time_struct->tm_mon + 1;
	osdtime->day = time_struct->tm_mday;
	osdtime->hour = time_struct->tm_hour;
	osdtime->minute = time_struct->tm_min;
	osdtime->second = time_struct->tm_sec;
	osdtime->millisecond = 0; /* Unix does not carry milli-seconds in time_t */
#elif OSD_OSTYPE==OSD_WIN32
	osdtime->year = systemtime->wYear;
	osdtime->month = (char)(systemtime->wMonth);
	osdtime->day = (char)(systemtime->wDay);
	osdtime->hour = (char)(systemtime->wHour);
	osdtime->minute = (char)(systemtime->wMinute);
	osdtime->second = (char)(systemtime->wSecond);
	osdtime->millisecond = systemtime->wMilliseconds;
#endif /* OSD_OSTYPE==OSD_WIN32 */
}


