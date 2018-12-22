/******************************************************************************

Coral Tree osd/time.h
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

#define OSD_JANUARY		1
#define OSD_FEBRUARY	2
#define OSD_MARCH		3
#define OSD_APRIL		4
#define OSD_MAY			5
#define OSD_JUNE		6
#define OSD_JULY		7
#define OSD_AUGUST		8
#define OSD_SEPTEMBER	9
#define OSD_OCTOBER		10
#define OSD_NOVEMBER	11
#define OSD_DECEMBER	12

#define OSD_JAN			1
#define OSD_FEB			2
#define OSD_MAR			3
#define OSD_APR			4
#define OSD_JUN			6
#define OSD_JUL			7
#define OSD_AUG			8
#define OSD_SEP			9
#define OSD_OCT			10
#define OSD_NOV			11
#define OSD_DEC			12

typedef struct _OSD_Time
{
	long	year;
	char	month;
	char	day;
	char	hour;
	char 	minute;
	char	second;
	long 	millisecond;
} OSD_Time;


#ifdef __cplusplus
    extern "C"
    {
#endif


#if OSD_COMPILER==OSD_KR
long osdTimeGet();
void osdTimeConvert();
#else
long osdTimeGet(OSD_Time *osdtime);
void osdTimeConvert(OSD_Time *osdtime, OSD_TIME *systemtime);
#endif


#ifdef __cplusplus
    }
#endif
