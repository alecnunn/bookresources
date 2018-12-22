#ifndef WBA_DAEMON_H
#define WBA_DAEMON_H

/******************************************************************************

Coral Tree wba/daemon.h
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

#include <wba/app.h>

/***********************************************************************
class WBA_Daemon

Well-behaved daemon.
***********************************************************************/
class WBA_Daemon : public WBA_Application
{

	public:
		WBA_Daemon(WBA_Daemon *current);
		virtual ~WBA_Daemon(void);

		virtual const char *GetName(void) const { return (char *) GPL_BAD_ADDRESS; }

		virtual int Main(void);
		virtual GPL_Boolean Initialize(void)
		{
			return WBA_Application::Initialize();
		}
		virtual void Finalize(void) { WBA_Application::Finalize(); }

		void SetContinueProcessing(const GPL_Boolean s)
		{
			continueProcessing = s;
		}
		GPL_Boolean GetContinueProcessing(void) { return continueProcessing; }

	protected:
		GPL_Boolean continueProcessing;

		virtual void Daemonize(void);
		virtual void Process(void) {}
};

#endif /* WBA_DAEMON_H */
