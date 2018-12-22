
/******************************************************************************

Coral Tree osd/output.h
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


#define OSD_OUT_FILE		1
#define OSD_OUT_STDOUT		2
#define OSD_OUT_STDERR		4
#define OSD_OUT_MESSAGEBOX	8

#define OSD_MAXOUTPUTFILENAMELENGTH 128

#define OSD_DEFAULTOUTPUTFILENAME	"OSD_OUT"

typedef struct _OSD_Output
{
	OSD_File			File;
	unsigned long		Flags;
	char				Filename[OSD_MAXOUTPUTFILENAMELENGTH];
	long				FileOpen;
#if OSD_OSTYPE==OSD_WIN32
	long				OwnConsole;
#endif
} OSD_Output;

#if OSD_COMPILER==OSD_KR
long osdOutputOpen();
long osdOutputClose();
long osdOutputSetFlags();
unsigned long osdOutputGetFlags();
long osdOutputSetFilename();
long osdOutputPrintLine();
long osdPrintf();
long osdVPrintf();
long osdOutputStr();
long osdOutputRaw();
#else
long osdOutputOpen(OSD_Output *output);
long osdOutputClose(OSD_Output *output);
long osdOutputSetFlags(OSD_Output *output, unsigned long flag_set);
unsigned long osdOutputGetFlags(OSD_Output *output);
long osdOutputSetFilename(OSD_Output *output, char *name);
long osdOutputPrintLine(OSD_Output *output, char *title,char *string);
long osdPrintf(OSD_Output *output,char *title, char *format, ...);
long osdVPrintf(OSD_Output *output,char *title, char *format, va_list args);
long osdOutputStr(long stderror,char *string);
long osdOutputRaw(long stderror,char *string, long length);
#endif


#ifdef __cplusplus
    }
#endif
