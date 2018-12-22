#ifndef GPL_LOG_H
#define GPL_LOG_H

/******************************************************************************

Coral Tree gpl/log.h
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

#include <gpl/str.h>

#ifdef GPL_LOGGER
#undef GPL_LOGGER
#endif
#define GPL_LOGGER (*(GPL_GLOBAL_LOGGER))
#ifdef GPL_SETFILELINE
#undef GPL_SETFILELINE
#endif
#define GPL_SETFILELINE	{GPL__FILE__=(char*)__FILE__; GPL__LINE__=(long)__LINE__;}
// WARNING: GPL_LOG is not 'one-line' safe (if(exp) GPL_LOG(args); is invalid)
// so be sure to use parenthesis when necessary.  This cannot be fixed due to
// the vararg in GPL_LOG_INLINE and the inability of macros to handle varargs.
#ifdef GPL_LOG
#undef GPL_LOG
#endif
#define GPL_LOG	GPL_SETFILELINE;GPL_LOG_INLINE
#ifdef GPL_DLOG
#undef GPL_DLOG
#endif
#define GPL_DLOG GPL_SETFILELINE;GPL_DLOG_INLINE
#ifdef gplLog
#undef gplLog
#endif
#define gplLog GPL_LOG
#ifdef gplDLog
#undef gplDLog
#endif
#define gplDLog GPL_DLOG

extern long GPL_DELIVERY_OUTPUT_INDENT;

#define gplLogIndent()						GPL_DELIVERY_OUTPUT_INDENT+=4
#define gplLogUnIndent()					GPL_DELIVERY_OUTPUT_INDENT-=4


/***********************************************************************
class GPL_Delivery
***********************************************************************/
class GPL_Delivery
{
	public:
		GPL_Delivery(void) {}
		virtual ~GPL_Delivery(void) {}

		virtual	GPL_Boolean Deliver(const GPL_String & filename,
									const long linenum,
									const GPL_String & tname,
									const GPL_String & text);

		static GPL_String VerboseImage (const GPL_String & typeName,
										const GPL_String & text,
										const GPL_Time & time,
										const GPL_String & fileName,
										const long lineNumber,
										const char dateSeparator = '.',
										const char timeSeparator = ':');
		static GPL_String TerseImage (const GPL_String & typeName,
										const GPL_String & text);
};

/***********************************************************************
class GPL_DeliveryPrintf
***********************************************************************/
class GPL_DeliveryPrintf : public GPL_Delivery
{
	public:
		GPL_DeliveryPrintf(void) {}
		virtual ~GPL_DeliveryPrintf(void) {}

		virtual	GPL_Boolean Deliver(const GPL_String & filename,
									const long linenum,
									const GPL_String & tname,
									const GPL_String & text);

	private:
		GPL_Time now;
};

/******************************************************************************
class GPL_DeliveryFile
******************************************************************************/
class GPL_DeliveryFile : public GPL_Delivery
{
	public:
		GPL_DeliveryFile(const GPL_String & file_name,
							const GPL_Character dateSeparator = '.',
							const GPL_Character timeSeparator = ':')
		{
			fname = file_name;
			dsep = dateSeparator;
			tsep = timeSeparator;
			faulty = FALSE;
		}
		virtual	~GPL_DeliveryFile(void)
		{
			if (file.IsOpen())
			{
				file.Close();
			}
		}

		virtual	GPL_Boolean Deliver(const GPL_String & filename,
									const long linenum,
									const GPL_String & tname,
									const GPL_String & text);
	protected:
		GPL_Character dsep;
		GPL_Character tsep;
		GPL_Time now;

		virtual GPL_Boolean WriteString(const GPL_String & string);

	private:
		GPL_Boolean faulty;
		GPL_String fname;
		GPL_File file;
};

/******************************************************************************
class GPL_DeliveryFileTerse
******************************************************************************/
class GPL_DeliveryFileTerse : public GPL_DeliveryFile
{
	public:
		GPL_DeliveryFileTerse(const GPL_String & file_name)
			: GPL_DeliveryFile(file_name) {}
		virtual	~GPL_DeliveryFileTerse(void) {}

		virtual	GPL_Boolean Deliver(const GPL_String & filename,
									const long linenum,
									const GPL_String & tname,
									const GPL_String & text);
};

/******************************************************************************
class GPL_DeliveryOsdOutputTerse
******************************************************************************/
class GPL_DeliveryOsdOutputTerse : public GPL_Delivery
{
	public:
		GPL_DeliveryOsdOutputTerse(const char *f = (char *) 0)
			: fn((char *) f) {}
		virtual ~GPL_DeliveryOsdOutputTerse(void);

		virtual	GPL_Boolean Deliver(const GPL_String & filename,
									const long linenum,
									const GPL_String & tname,
									const GPL_String & text);

	protected:
		char *fn;
		OSD_File file;

		virtual GPL_Boolean WriteString(const GPL_String & string);
};

/******************************************************************************
class GPL_DeliveryOsdOutput
******************************************************************************/
class GPL_DeliveryOsdOutput : public GPL_DeliveryOsdOutputTerse
{
	public:
		GPL_DeliveryOsdOutput(void) {}
		virtual ~GPL_DeliveryOsdOutput(void) {}

		virtual	GPL_Boolean Deliver(const GPL_String & filename,
									const long linenum,
									const GPL_String & tname,
									const GPL_String & text);
	private:
		GPL_Time now;
};

/******************************************************************************
class GPL_DeliveryPrintfTerse
******************************************************************************/
class GPL_DeliveryPrintfTerse : public GPL_Delivery
{
	public:
		GPL_DeliveryPrintfTerse(void) {}
		virtual ~GPL_DeliveryPrintfTerse(void) {}

		virtual	GPL_Boolean Deliver(const GPL_String & filename,
									const long linenum,
									const GPL_String & tname,
									const GPL_String & text);
};

/***********************************************************************
class GPL_DeliveryNoop
***********************************************************************/
class GPL_DeliveryNoop : public GPL_Delivery
{
	public:
		GPL_DeliveryNoop(void) {}
		virtual ~GPL_DeliveryNoop(void) {}

		virtual	GPL_Boolean Deliver(const GPL_String & filename,
									const long linenum,
									const GPL_String & tname,
									const GPL_String & text)
		{
			GPL_String f;
			f = filename;
			long l;
			l = linenum;
			GPL_String t;
			t = tname;
			t = text;
			return TRUE;
		}
};

/***********************************************************************
class GPL_DeliveryExit
***********************************************************************/
class GPL_DeliveryExit : public GPL_Delivery
{
	public:
		GPL_DeliveryExit(const GPL_Boolean dump = FALSE) : produceCore(dump) {}
		virtual	~GPL_DeliveryExit(void) {}

		virtual	GPL_Boolean Deliver(const GPL_String & filename,
									const long linenum,
									const GPL_String & tname,
									const GPL_String & text)
		{
			GPL_String s;s=filename;s=tname;s=text;long l;l=linenum;
			if (produceCore)
			{
				abort();
			}
			else
			{
				exit(1); 
			}
			return FALSE;
		}

	protected:
		GPL_Boolean produceCore;
};

/***********************************************************************
class GPL_DeliverySystemLog
***********************************************************************/
class GPL_DeliverySystemLog : public GPL_Delivery
{
	public:
		GPL_DeliverySystemLog(const int e = OSD_SYSTEMLOG_ERROR,
								const char *i = (char *) 0);
		virtual ~GPL_DeliverySystemLog(void);

		virtual	GPL_Boolean Deliver(const GPL_String & filename,
									const long linenum,
									const GPL_String & tname,
									const GPL_String & text);

	private:
		int eventType;
		static unsigned long numberOfInstances;
};

typedef GPL_Array<GPL_String> GPL_StringArray;

/***********************************************************************
class GPL_Logger
***********************************************************************/
class GPL_Logger
{
	public:
		GPL_Logger(void);
		virtual	~GPL_Logger(void);

		GPL_Delivery *GetDelivery(const GPL_String & name) const;
		void SetDefaultDelivery(GPL_Delivery *delivery);
		void SetDelivery(const GPL_String & name, GPL_Delivery *delivery
										= (GPL_Delivery *) GPL_BAD_ADDRESS);
		void UnsetDelivery(const GPL_String & name);

		GPL_StringArray GetType(const GPL_String & tname) const;
		void SetType(const GPL_String & tname, const GPL_StringArray & dnames);
		void SetType(const GPL_String & tname, int deliveries = 0, ...);

		virtual void Log(const GPL_String & filename, const long linenum,
							const GPL_String & tname, const char *format,
							...);
		virtual void VLog(const GPL_String & filename, const long linenum,
							const GPL_String & tname, const char *format,
							va_list args);
		virtual void DirectLog(GPL_Delivery *delivery,
								const GPL_String & filename,
								const long linenum, const GPL_String & tname,
								const char *format, ...);
		virtual void DirectVLog(GPL_Delivery *delivery,
								const GPL_String & filename,
								const long linenum, const GPL_String & tname,
								const char *format, va_list args);

	protected:
		GPL_Pointer<GPL_Delivery> defaultDelivery;
		GPL_Map<GPL_String, GPL_StringArray> typeToDeliveryNames;
		GPL_Map<GPL_String, GPL_Pointer<GPL_Delivery> > deliveryNameToDelivery;

		void Merge(GPL_Logger *logger);
};

void gplLoggerAddModule(const char *module);
void gplHackPrint(long lvl, char *fmt, ...);

extern GPL_Logger *GPL_GLOBAL_LOGGER;
extern const char *GPL__FILE__;
extern long GPL__LINE__;
#define GPL_INIT_CLASSNAME(module)   module##_Init
#define GPL_INIT_BUILDID(module)     module##_BUILDID
#ifndef CORAL_BUILDID
#define CORAL_BUILDID "CoralBuildID improper build"
#endif
#define GPL_INIT_DECLARATION(module) extern char GPL_INIT_BUILDID(module)[];
#define GPL_INIT_DEFINITION(module) long GPL_INIT_CLASSNAME(module)::cnt = 0; \
           char GPL_INIT_BUILDID(module)[] = CORAL_BUILDID;
#define GPL_INIT_CONSTRUCT(module)				\
	public:										\
		static long cnt;						\
		GPL_INIT_CLASSNAME(module)(void)			\
		{										\
			gplHackPrint(2,"+%s[%d]\n", #module, cnt);\
			if(cnt++ == 0)						\
			{									\
				gplHackPrint(1,"initializing %s library\n",#module);\
				Initialize();					\
			}									\
		}										\
		virtual ~GPL_INIT_CLASSNAME(module)(void)			\
		{										\
			gplHackPrint(2,"-%s[%d]\n", #module, cnt-1);\
			if(--cnt == 0)						\
			{									\
				gplHackPrint(1,"finalizing %s library\n",#module);\
				Finalize();						\
			}									\
		}

#define GPL_INIT_METHOD(module)					\
	public:										\
virtual	void Initialize(void)						\
		{ 										\
			gplLoggerAddModule(#module); 		\
		}										\
virtual	void Finalize(void)							\
		{										\
		}
		
class GPL_INIT_CLASSNAME(GPL)
{
	GPL_INIT_CONSTRUCT(GPL);
	public:
		void Initialize(void)
		{
			GPL_GLOBAL_LOGGER = new GPL_Logger();
			gplLoggerAddModule("GPL");
		}
		void Finalize(void)
		{
			delete GPL_GLOBAL_LOGGER;
			GPL_GLOBAL_LOGGER = NULL;
		}
};
static GPL_INIT_CLASSNAME(GPL) GPL_INIT;
GPL_INIT_DECLARATION(GPL);

inline void GPL_LOG_INLINE(const GPL_String &t, const char *f, ...) 
{
	va_list ap;
	va_start(ap, f);
	GPL_LOGGER.VLog(GPL__FILE__,GPL__LINE__, t, f, ap);
	va_end(ap);
}

inline void GPL_DLOG_INLINE(GPL_Delivery *d, const GPL_String &t, const char *f, ...) 
{
	va_list ap;
	va_start(ap, f);
	GPL_LOGGER.DirectVLog(d,GPL__FILE__,GPL__LINE__, t, f, ap);
	va_end(ap);
}

#endif
