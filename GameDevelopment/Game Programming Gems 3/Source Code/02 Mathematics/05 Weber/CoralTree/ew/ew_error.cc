/******************************************************************************

Coral Tree ew/ew_error.cc
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


#include"ew.h"



#define EW_RESET_ERRNO	TRUE	// reset errno to 0 after reading



/* for general external use */
EW_Output ewGlobalOutput;


/* OSDEP */
#if sun
	extern char *sys_errlist[];
#endif



/******************************************************************************
void	ewPrint(long source,long type,char *file,long line,long logger,
													char *format,va_list ap)

	print given formatted message, file name, and line number

	this function can be adjusted to react to messages of
		specific sources and types
		ie. EW_APP,EW_WARN could be handled differently than EW_WINDOW,EW_ERROR

	note that like printf(), 'format' can simply be a fixed message with
		no arguments to format

******************************************************************************/
/*PROTOTYPE*/
void ewPrint(long source,long type,char *file,long line,long logger,char *format,va_list ap)
	{
	const long types=5;

	static char sourcename[7][20]= 			{ "App", "Graphics", "Window", "Event", "EW", "OS", "Widget" };

	static long colorcode[types]=			{ EW_CODE_NORMAL,EW_CODE_CYAN,EW_CODE_RED,EW_CODE_YELLOW,EW_CODE_YELLOW };
	static char type_name[2][types][20]=	{ {"Log", "Warning", "Error", "Fatal Error", "Intentional Crash"},
												{"", "", "", " Fatal Error", " Intentional Crash"} };

	static char logger_code[types][20]=		{ "EW LOG", "EW WARNING", "EW ERROR", "EW ERROR", "EW ERROR" };

	char title[200];
	char string[10000];
	char error_message[256];

	unsigned long old_flags,new_flags;

	if(!logger)
		{
		old_flags=ewGlobalOutput.GetFlags();
		new_flags=old_flags;
		}

	if(logger)
		sprintf(title,"%s%s%s%s",ewColorCode(colorcode[type]),sourcename[source],type_name[logger][type],
																								ewColorCode(colorcode[0]));
	else
		sprintf(title,"EW %s %s",sourcename[source],type_name[logger][type]);

	error_message[0]=0;

	if(type!=EW_LOG)
		{
		#if EW_WL==EW_WIN32

			sprintf(error_message," Error %d",GetLastError());

		/* OSDEP */
		#elif sun

			if(errno)
				sprintf(error_message," errno %d: %s",errno,sys_errlist[errno]);

#if EW_RESET_ERRNO
			errno=0;
#endif

		#else

			if(errno)
				sprintf(error_message," errno %d: %s",errno,strerror(errno));

#if EW_RESET_ERRNO
			errno=0;
#endif

		#endif

		sprintf(string,"%s:%d \"%s\"%s",file,(int)line,format,error_message);
		}
	else
		{
		sprintf(string,"\"%s\"",format);
		}

	if(!logger && type<EW_ERROR)
		new_flags= (new_flags|OSD_OUT_MESSAGEBOX) ^ OSD_OUT_MESSAGEBOX;

//	printf("DEBUG-DEBUG: ``%s|%s''\n",title,string);

	if(logger)
		{
		char string2[10000];

		sprintf(string2,"%s %s",title,string);
//		WBA_LOG(logger_code[type%types],string2);

		//WBA_APP->GetLogger()->VLog(file,line,logger_code[type%types],string2,ap);
		GPL_LOGGER.VLog(file,line,logger_code[type%types],string2,ap);
//		WBA_APP->logger->VLog(file,line,logger_code[type%types],string2,ap);
		}
else
		{
		ewGlobalOutput.SetFlags(new_flags);
		ewGlobalOutput.PrintLine(title,string);
		ewGlobalOutput.SetFlags(old_flags);
		}

	if(type==EW_FATAL)
		ewExit(1);
	else if(type==EW_CRASH)
		EW_CAUSE_SEGMENTION_FAULT;
	}
