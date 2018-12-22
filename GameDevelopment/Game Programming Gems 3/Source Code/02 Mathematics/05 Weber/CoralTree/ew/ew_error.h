/******************************************************************************

Coral Tree ew/ew_error.h
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

/*******************************************************************************

							EW message handling

******************************************************************************/



#define EW_ERROR_USE_WBA	TRUE	// use WBA_Logger vs. an OSD_Output directly



// #define EW_PRINT(source,type,string)	ewPrint(source,type,__FILE__,__LINE__,EW_ERROR_USE_WBA,string)

inline void EW_PRINT(char *file,long line,long source,long type,char *format, ...)
	{
	va_list ap;
	va_start(ap,format);
	ewPrint(source,type,file,line,EW_ERROR_USE_WBA,format,ap);
//	ewPrint(source,type,GPL__FILE__,GPL__LINE__,EW_ERROR_USE_WBA,format,ap);
	va_end(ap);
	}



/* EW_PRINT sources */
#define EW_P_APP	0
#define EW_P_GFX	1
#define EW_P_WINDOW	2
#define EW_P_EVENT	3
#define EW_P_EW		4
#define EW_P_OS		5
#define EW_P_WIDGET	6

#define EW_APP		__FILE__,__LINE__,EW_P_APP		// application
#define EW_GFX		__FILE__,__LINE__,EW_P_GFX		// graphics abstraction
#define EW_WINDOW	__FILE__,__LINE__,EW_P_WINDOW	// window handling
#define EW_EVENT	__FILE__,__LINE__,EW_P_EVENT	// event handling
#define EW_EW		__FILE__,__LINE__,EW_P_EW		// other non-specific EW
#define EW_OS		__FILE__,__LINE__,EW_P_OS		// operating system
#define EW_WIDGET	__FILE__,__LINE__,EW_P_WIDGET	// widget related, like WDS

/* EW_PRINT types */
#define EW_LOG		0	/* verbose log updates */
#define EW_WARN		1	/* possible problem */
#define EW_ERROR	2	/* definite problem */
#define EW_FATAL	3	/* fatal problem */
#define EW_CRASH	4	/* intentional crash (program-controlled breakpoint) */



/*
#define DB(s)				EW_PRINT(EW_EW,EW_LOG,s)
#define SOFTWARE_ERROR(s)	EW_PRINT(EW_EW,EW_ERROR,s)
*/

/******************************************************************************
class	EW_Output

	not opened on construction so that Win32 has chance to enter window mode
	if console opens before a window, windows ops will block

Keywords:
******************************************************************************/
class EW_Output
	{
	private:

		OSD_Output	osd_output;

		long		open;

	public:

					EW_Output(void)
						{
						open=FALSE;
						Open();
						};

virtual				~EW_Output(void)
						{
						Close();
						};

	long			Open(void)
						{
						if(!open)
							{
							open=TRUE;
							return osdOutputOpen(&osd_output);
							}
						else
							return FALSE;
						};

	long			Close(void)
						{
						if(open)
							{
							open=FALSE;
							return osdOutputClose(&osd_output);
							}
						else
							return FALSE;
						};

	long			PrintLine(char *title,char *string)
						{
						Open();
						return osdOutputPrintLine(&osd_output,title,string);
						};

	unsigned long	GetFlags(void)
						{
						Open();
						return osdOutputGetFlags(&osd_output);
						};

	long			SetFlags(unsigned long flags)
						{
						Open();
						return osdOutputSetFlags(&osd_output,flags);
						};

	long			SetFilename(char *filename)
						{
						Open();
						return osdOutputSetFilename(&osd_output,filename);
						};
	};



extern EW_Output ewGlobalOutput;
