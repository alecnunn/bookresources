
/******************************************************************************

Coral Tree osd/output.c
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



/******************************************************************************
long	osdOutputOpen(OSD_Output *output)

keywords: c, blackbox_system, file, debug, io
******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdOutputOpen(output)
	OSD_Output *output;
#else
long osdOutputOpen(OSD_Output *output)
#endif
{
	output->Flags = OSD_OUT_STDOUT;
	output->FileOpen = FALSE;
	strcpy(output->Filename,OSD_DEFAULTOUTPUTFILENAME);
#if OSD_OSTYPE==OSD_WIN32
	output->OwnConsole = FALSE;
	if((output->Flags & OSD_OUT_STDOUT)||(output->Flags & OSD_OUT_STDERR))
	{
		if(AllocConsole())
		{
			output->OwnConsole = TRUE;
		}
	}

#endif
	return TRUE;
}



/******************************************************************************
long	osdOutputClose(OSD_Output *output)

keywords: c, blackbox_system, file, debug, io
******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdOutputClose(output)
	OSD_Output *output;
#else
long osdOutputClose(OSD_Output *output)
#endif
{
	if(output->FileOpen)
	{
		osdFileClose(&output->File);
		output->FileOpen = FALSE;
	}
#if OSD_OSTYPE==OSD_WIN32
	//if(output->OwnConsole) FreeConsole();
#endif
	return TRUE;
}



/******************************************************************************
long	osdOutputSetFlags(OSD_Output *output, unsigned long flag_set)

keywords: c, blackbox_system, file, debug, io
******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdOutputSetFlags(output, flag_set)
	OSD_Output *output; 
	unsigned long flag_set;
#else
long osdOutputSetFlags(OSD_Output *output, unsigned long flag_set)
#endif
{
	output->Flags = flag_set;
	return TRUE;
}



/******************************************************************************
unsigned long osdOutputGetFlags(OSD_Output *output)

keywords: c, blackbox_system, file, debug, io
******************************************************************************/
#if OSD_COMPILER==OSD_KR
unsigned long osdOutputGetFlags(output)
	OSD_Output *output;
#else
unsigned long osdOutputGetFlags(OSD_Output *output)
#endif
{
	return output->Flags;
}



/******************************************************************************
long	osdOutputSetFilename(OSD_Output *output, char *name)

keywords: c, blackbox_system, file, debug, io
******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdOutputSetFilename(output, name)
	OSD_Output *output; 
	char *name;
#else
long osdOutputSetFilename(OSD_Output *output, char *name)
#endif
{
	strncpy(output->Filename,name,OSD_MAXOUTPUTFILENAMELENGTH);
	return TRUE;
}



#define OSD_PRINT_MAXLENGTH	1024

/******************************************************************************
long	osdOutputPrintLine(OSD_Output *output, char *title,char *string)

keywords: c, blackbox_system, file, debug, io, system
******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdOutputPrintLine(output, title, string)
	OSD_Output *output; 
	char *title;
	char *string;
#else
long osdOutputPrintLine(OSD_Output *output, char *title,char *string)
#endif
{
	return osdPrintf(output, title, "%s\n", string);
}

/******************************************************************************

******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdPrintf(output, title, format)
	OSD_Output *output;
	char *title;
	char *format;
#else
long osdPrintf(OSD_Output *output,char *title, char *format, ...)
#endif
{
	long rc = 0;
	va_list args;
	va_start(args, format);
	rc = osdVPrintf(output, title, format, args);
	va_end(args);
	return rc;
}

#if OSD_COMPILER==OSD_KR
long osdVPrintf(output, title, format, args)
	OSD_Output *output;
	char *title;
	char *format;
	va_list args;
#else
long osdVPrintf(OSD_Output *output,char *title, char *format, va_list args)
#endif
{
	char *string;
	char *message;

	string = osdVSPrintf(format,args);

	if (output->Flags & OSD_OUT_FILE)
	{
		if (!output->FileOpen) 
		{
			if (!osdFileOpen(&(output->File), output->Filename,
							OSD_FILE_READ | OSD_FILE_WRITE))
			{
				osdFree(string);
				return FALSE;
			}
			else
			{
				output->FileOpen = TRUE;
			}

			osdFileSetPointer(&(output->File), 0, OSD_FILE_END);
		}

		if (title)
		{
			osdFileWrite(&(output->File), title, strlen(title));
			osdFileWrite(&(output->File), ": ", 2);
		}
		if (string)
		{
			osdFileWrite(&(output->File), string, strlen(string));
		}
	}

	if ((output->Flags & OSD_OUT_STDOUT) || (output->Flags & OSD_OUT_STDERR))
	{
		if (title)
		{
			message = osdSPrintf("%s: %s", title, ((string) ? string : ""));
		}
		else
		{
			message = osdSPrintf("%s", ((string) ? string : ""));
		}

		if (output->Flags & OSD_OUT_STDERR)
		{
			osdOutputStr(TRUE, message);
		}
		if (output->Flags & OSD_OUT_STDOUT)
		{
			osdOutputStr(FALSE, message);
		}

		/***************************************************************
			Yes, it is slower to allocate and deallocate the message
			string each time through this function, but artificially
			limited output string lengths when you can not control
			the presence of a new line seemed less-than-optimal.
		***************************************************************/
		osdFree(message);
	}

	if (output->Flags & OSD_OUT_MESSAGEBOX)
	{
#if OSD_OSTYPE==OSD_WIN32
		if (title)
		{
			MessageBox(NULL, string, title, MB_OK|MB_ICONHAND|MB_SYSTEMMODAL);
		}
		else
		{
			MessageBox(NULL, string, "", MB_OK|MB_ICONHAND|MB_SYSTEMMODAL);
		}
#endif
	}

	osdFree(string);
	return TRUE;
}

/******************************************************************************
long	osdOutputRaw(long stderror,char *string)

Keywords:
******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdOutputStr(stderror,string)
	long stderror;
	char *string;
#else
long osdOutputStr(long stderror,char *string)
#endif
{
	return osdOutputRaw(stderror, string, strlen(string));
}



/******************************************************************************
long	osdOutputRaw(stderror,string,length)

******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdOutputRaw(stderror,string,length)
	long stderror;
	char *string;
	long length;
#else
long osdOutputRaw(long stderror,char *string, long length)
#endif
	{
#if OSD_OSTYPE==OSD_WIN32

	// finally the same as UNIX now
	FILE *file= (stderror)? stderr: stdout;

	HANDLE handle=GetStdHandle(stderror? STD_ERROR_HANDLE: STD_OUTPUT_HANDLE);

	unsigned long written;	// bytes actually written
	long data;				// Win32 reserved (void *)

	long c=0,sublen,code,bytes;
	char string2[32];

	while(c<length)
		{
		sublen=0;

		while(c+sublen<length && string[c+sublen]!=0 && string[c+sublen]!=0x1B)
			sublen++;

//		WriteConsole(handle,&string[c],sublen,&written,(void *)&data);
		fwrite((void *)&string[c],1,(unsigned int)sublen,file);
		fflush(file);

		if(string[c+sublen]==0x1B)
			{
			sublen++;
			code=0;
			bytes=0;
			sscanf(&string[c+sublen],"[%dm%n",&code,&bytes);
			sublen+=bytes;

			//* pass ANSI codes through, but follow with 5 backspaces
			while(string[c+sublen]=='\b')
				sublen++;
			sprintf(string2," [%dm\b\b\b\b",code);
			if(code>9)
				strcat(string2,"\b");
			strcat(string2,"     \b\b\b\b\b");
			fwrite((void *)string2,1,(unsigned int)strlen(string2),file);
			fflush(file);

			switch(code)
				{
				case 1:
					/* DARK CYAN */
					SetConsoleTextAttribute(handle,FOREGROUND_GREEN|FOREGROUND_BLUE);
					break;
				case 2:
					/* DARK RED */
					SetConsoleTextAttribute(handle,FOREGROUND_RED);
					break;
				case 30:
					/* BLACK */
					SetConsoleTextAttribute(handle,0);
					break;
				case 31:
					/* RED */
					SetConsoleTextAttribute(handle,FOREGROUND_INTENSITY|FOREGROUND_RED);
					break;
				case 32:
					/* GREEN */
					SetConsoleTextAttribute(handle,FOREGROUND_INTENSITY|FOREGROUND_GREEN);
					break;
				case 33:
					/* YELLOW */
					SetConsoleTextAttribute(handle,FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN);
					break;
				case 34:
					/* BLUE */
					SetConsoleTextAttribute(handle,FOREGROUND_INTENSITY|FOREGROUND_BLUE);
					break;
				case 35:
					/* MAGENTA */
					SetConsoleTextAttribute(handle,FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_BLUE);
					break;
				case 36:
					/* CYAN */
					SetConsoleTextAttribute(handle,FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_BLUE);
					break;
				default:
					/* WHITE */
					SetConsoleTextAttribute(handle,FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
					break;
				}
			}
		
		c+=sublen;
		}

#else

	FILE *file= (stderror)? stderr: stdout;
	fwrite((void *)string,1,(unsigned int)strlen(string),file);
	/*fprintf(file,string);*/
	fflush(file);

#endif

	return FALSE;
	}
