/******************************************************************************

Coral Tree osd/error.h
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



#ifdef __cplusplus
    extern "C"
    {
#endif


#define OSD_MAX_PRINT_TITLE_LEN		200
#define OSD_MAX_PRINT_STRING_LEN	1000

typedef	uint32	OSD_Error;
#define OSD_ERROR_NONE				0
#define OSD_ERROR_OS_CREATE_FAIL	1
#define OSD_ERROR_OS_DESTROY_FAIL	2
#define OSD_ERROR_UNKNOWN			3

#if OSD_COMPILER==OSD_KR
void osdPrintWithLocation();
#else
void osdPrintWithLocation(long source,long type,char *message,char *file,long line);
#endif

#define osdPrint(source,type,string)	osdPrintWithLocation(source,type,string,__FILE__,__LINE__)

/* OSD_PRINT sources */
#define OSD_SOFTWARE	0	/* software related condition */
#define OSD_SYSTEM		1	/* system related condition */
#define OSD_GENERAL		2	/* non-specific OSD (specific is preferred) */

/* OSD_PRINT types */
#define OSD_LOG			0	/* verbose log updates */
#define OSD_WARN		1	/* possible problem */
#define OSD_ERROR		2	/* definite problem */
#define OSD_FATAL		3	/* fatal problem */


#ifdef __cplusplus
    }
#endif
