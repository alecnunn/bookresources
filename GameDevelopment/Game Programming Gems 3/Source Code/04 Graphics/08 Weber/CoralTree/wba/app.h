#ifndef WBA_APP_H
#define WBA_APP_H

/******************************************************************************

Coral Tree wba/app.h
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

#include <wba/env.h>

/***********************************************************************
class WBA_ApplicationId

Well-behaved application ID.
***********************************************************************/
class WBA_ApplicationId
{
	public:
		WBA_ApplicationId(void) : pid(0) {}
		WBA_ApplicationId(const GPL_String & n) : name(n), pid(osdGetPID()) {}
		~WBA_ApplicationId(void) {}

		GPL_Boolean operator==(const WBA_ApplicationId & i) const
		{
			return name == i.name && pid == i.pid;
		}

		inline const GPL_String & GetName(void) const { return name; }
		inline OSD_PID GetPID(void) const { return pid; }

		inline void SetName(const GPL_String & n) { name = n; }
		inline void SetPID(const OSD_PID p) { pid = p; }

	protected:
		GPL_String name;
		OSD_PID pid;
};

/***********************************************************************
class WBA_Application

Well-behaved application.
***********************************************************************/
class WBA_Application
{
	public:
		WBA_Application(WBA_Application *current);
		virtual ~WBA_Application(void);

		static WBA_Application * GetCurrentApplication(void) { return singleton; }

		inline int GetExitCode(void) const { return exitCode; }
		inline WBA_Environment *GetEnvironment(void) { return environment; }
		inline GPL_Logger *GetLogger(void) { return logger; }
		inline GPL_Boolean GetInputEchoMode(void) const { return inputEchoMode; }
		virtual const char *GetName(void) const { return (char *) GPL_BAD_ADDRESS; }
		const char *GetProgramName(void) const { return environment->GetProgramName().GetData(); }
		WBA_ApplicationId GetId(void) const { return WBA_ApplicationId(GetName()); }
		virtual const GPL_Version & GetVersion(void) const;
		virtual const char *GetBuildString(void) const;
		virtual const char *GetUsage(GPL_Boolean all = FALSE) const;
		virtual const char *DumpEnvironment(void) const;
		virtual const char *GetParameterUsage(void) const { return (char *) GPL_BAD_ADDRESS; }

		inline void SetExitCode(const int e) { exitCode = e; }
		void SetInputEchoMode(const GPL_Boolean m);

		virtual GPL_Boolean Prompt(const GPL_String & prompt,
									const GPL_String & initial,
									GPL_String & response,
									const GPL_Boolean hidden = FALSE);
		virtual GPL_Boolean Prompt(const GPL_String & prompt,
									const GPL_String & initial,
									const GPL_String & delimiter,
									GPL_List<GPL_String> & responses,
									const GPL_Boolean hidden = FALSE);
		virtual void Printf(char *format, ...);

		virtual int Main(void);
		virtual GPL_Boolean Initialize(void) { return TRUE; }
		virtual void Finalize(void) {}
		virtual void Exit(void);

	protected:
		int exitCode;
		WBA_Environment *environment;
		GPL_Logger *logger;
		GPL_Boolean inputEchoMode;
		GPL_Version version;
		GPL_String usage;
		GPL_String dump;
		static WBA_Application *singleton;

		virtual void Process(void) {}
		OSD_Output *GetStandardOutput(void);

	private:
		OSD_Output *standardOutput;
};

extern char *WBA__FILE__;
extern long WBA__LINE__;

inline void WBA_LOG_INLINE(const GPL_String &t, const char *f, ...) 
{
	va_list ap;
	va_start(ap, f);
	if(WBA_Application::GetCurrentApplication())
	{
		WBA_Application::GetCurrentApplication()->GetLogger()->VLog(WBA__FILE__,WBA__LINE__, t, f, ap);
	}
	va_end(ap);
}

inline void WBA_DLOG_INLINE(WBA_Delivery *d, const GPL_String &t, const char *f, ...) 
{
	va_list ap;
	va_start(ap, f);
	if(WBA_Application::GetCurrentApplication())
	{
		WBA_Application::GetCurrentApplication()->GetLogger()->DirectVLog(d,WBA__FILE__,WBA__LINE__, t, f, ap);
	}
	va_end(ap);
}

#endif /* WBA_APP_H */
