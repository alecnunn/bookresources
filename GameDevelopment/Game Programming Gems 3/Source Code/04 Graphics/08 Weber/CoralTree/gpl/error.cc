
/******************************************************************************

Coral Tree gpl/error.cc
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
#include "gpl.h"

OSD_Output gplGlobalOutput;
#if OSD_OSTYPE==OSD_UNIX
unsigned long gplGlobalOutputInitialFlags = OSD_OUT_STDERR;
#elif OSD_OSTYPE==OSD_WIN32
unsigned long gplGlobalOutputInitialFlags = OSD_OUT_STDERR;
#endif

/******************************************************************************
void	gplPrintWithLocation(long source,long type,char *message,char *file,long line)

	print given message, file name, and line number

	this function can be adjusted to react to messages of
		specific sources and types

	TODO: vararg this like printf

Keywords: debug, c, blackbox_system, system
******************************************************************************/
void gplPrintWithLocation(long source,long type,char *message,char *file,long line)
{
	static long initialized = 0;
	static char sourcename[4][20]=  { 	"SOFTWARE",
										"SYSTEM",
										"USER",
										"GENERAL" };
	static char type_name[4][20]=	{ 	"LOG", 
										"WARNING", 
										"ERROR", 
										"FATAL" };

	static char title[GPL_MAX_PRINT_TITLE_LEN];
	static char string[GPL_MAX_PRINT_STRING_LEN];

	if(!initialized)
	{
		osdOutputOpen(&gplGlobalOutput);
		osdOutputSetFlags(&gplGlobalOutput,gplGlobalOutputInitialFlags);
	}

#if GPL_VERBOSITY==GPL_DEBUG
		sprintf(string,"%s FILE: %s LINE: %d ERRNUM: %d",
			message, file, line, OSD_ERRNO);
#elif GPL_VERBOSITY==GPL_NORMAL
		sprintf(string,"%s",message);
		/* SHUT UP COMPILER */
		file = file;
		line = line;
#elif GPL_VERBOSITY==GPL_SILENT
		/* SHUT UP COMPILER */
		file = file;
		line = line;
#else
	#error GPL_VERBOSITY not specified, this should cause compile error
#endif

#if GPL_VERBOSITY!=GPL_SILENT
	sprintf(title,"GPL %s %s",sourcename[source],type_name[type]);
	osdOutputPrintLine(&gplGlobalOutput,title,string);
#endif

	/* EXTRA BEHAVIOR */
	switch(type)
	{
		case GPL_VLOG:
			/* NOOP */
			break;
		case GPL_WARN:
			/* NOOP */
			break;
		case GPL_ERROR:
			/* NOOP */
			break;
		case GPL_FATAL:
			exit(-1);
			break;
		default:
			osdOutputPrintLine(&gplGlobalOutput,"PRINT ERROR","invalid message type");
			exit(-2);
			break;
	}

}

