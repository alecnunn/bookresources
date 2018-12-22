/******************************************************************************

Coral Tree wba/daemon.cc
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

#include <wba/wba.h>

#if OSD_OSTYPE==OSD_UNIX
#include <sys/stat.h> // for umask()
#include <unistd.h> // for fork() and pid_t
#include <stdlib.h> // for exit()
#endif /* OSD_OSTYPE==OSD_UNIX */

WBA_Daemon::WBA_Daemon(WBA_Daemon *current)
	: WBA_Application(current), continueProcessing(TRUE)
{
}

WBA_Daemon::~WBA_Daemon(void)
{
}

int
WBA_Daemon::Main(void)
{
	// Note that daemonization occurs prior to initialization.
	// This is because initialization may consume resources and
	// this consumption may not gracefully span daemonization.
	// Thus, daemonization occurs first.  Derived classes which
	// which to specify their own Daemonize() should take care
	// to not reference anything initialized in Initialize(),
	// since those things would be uninitialized.
	Daemonize();
	if (Initialize())
	{
		while (continueProcessing)
		{
			Process();
		}

		Finalize();
	}

	return exitCode;
}

void
WBA_Daemon::Daemonize(void)
{
#if OSD_OSTYPE==OSD_UNIX
	register pid_t pid = ::fork(); 
	// Error.
	if (0 > pid)
	{
		exit(-1);
	}

	// Parent.
	else if (0 < pid)
	{
		exit(0);
	}

	setsid();

	chdir("/");

	umask(0);
#else
#if OSD_OSTYPE==OSD_WIN32
	//!! KLUDGE Massively bogus on NT!
#endif /* OSD_WIN32 */
#endif /* OSD_UNIX */
}
