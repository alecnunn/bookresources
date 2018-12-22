
/******************************************************************************

Coral Tree osd/error.c
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
#include "osd.h"

OSD_Output osdGlobalOutput;

#if OSD_OSTYPE==OSD_UNIX
unsigned long osdGlobalOutputInitialFlags = OSD_OUT_STDERR;
#elif OSD_OSTYPE==OSD_WIN32
unsigned long osdGlobalOutputInitialFlags = OSD_OUT_STDERR;
#endif

/******************************************************************************
void	osdPrintWithLocation(long source,long type,char *message,char *file,long line)

	print given message, file name, and line number

	this function can be adjusted to react to messages of
		specific sources and types

	TODO: vararg this like printf

Keywords: debug, c, blackbox_system, system
******************************************************************************/
#if OSD_COMPILER==OSD_KR
void osdPrintWithLocation(source, type, message, file, line)
	long source;
	long type;
	char *message;
	char *file;
	long line;
#else
void osdPrintWithLocation(long source,long type,char *message,char *file,long line)
#endif
{
	OSD_ERROR_NUMBER errornum;
	static long initialized = 0;
	static char sourcename[3][20]=  { 	"SOFTWARE",
										"SYSTEM",
										"GENERAL" };
	static char tname[4][20]=		{ 	"LOG", 
										"WARNING", 
										"ERROR", 
										"FATAL" };

	static char title[OSD_MAX_PRINT_TITLE_LEN];
	static char string[OSD_MAX_PRINT_STRING_LEN];
	errornum = OSD_ERRNO;

	if(!initialized)
	{
		osdOutputOpen(&osdGlobalOutput);
		osdOutputSetFlags(&osdGlobalOutput,osdGlobalOutputInitialFlags);
	}

#if OSD_VERBOSITY==OSD_DEBUG
		sprintf(string,"%s FILE: %s LINE: %d ERRNUM: %d",
			message, file, line, errornum);
#elif OSD_VERBOSITY==OSD_NORMAL
		sprintf(string,"%s",message);
		/* SHUT UP COMPILER */
		file = file;
		line = line;
#elif OSD_VERBOSITY==OSD_SILENT
		/* SHUT UP COMPILER */
		file = file;
		line = line;
#else
	error OSD_VERBOSITY not specified, this should cause compile error
#endif

#if OSD_VERBOSITY!=OSD_SILENT
	sprintf(title,"OSD %s %s",sourcename[source],tname[type]);
	osdOutputPrintLine(&osdGlobalOutput,title,string);
#endif

	/* EXTRA BEHAVIOR */
	switch(type)
	{
		case OSD_LOG:
			/* NOOP */
			break;
		case OSD_WARN:
			/* NOOP */
			break;
		case OSD_ERROR:
			/* NOOP */
			break;
		case OSD_FATAL:
			exit(-1);
			break;
		default:
			osdOutputPrintLine(&osdGlobalOutput,"PRINT ERROR","invalid message type");
			exit(-2);
			break;
	}

}

