#ifndef WBA_WBA_H
#define WBA_WBA_H

/******************************************************************************

Coral Tree wba/wba.h
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

#include <meta/platform.h>

#include <osd/osd.h>
#include <gpl/gpl.h>

// These are convenience macros for us lazy folks.
#ifdef WBA_APP
#undef WBA_APP
#endif
#define WBA_APP	WBA_Application::GetCurrentApplication()
#ifdef WBA_ENV
#undef WBA_ENV
#endif
#define WBA_ENV	(*(WBA_GLOBAL_ENV))
#ifdef WBA_ARGS
#undef WBA_ARGS
#endif
#define WBA_ARGS (*(WBA_ENV.GetArgumentTable()))
#ifdef WBA_LOGGER
#undef WBA_LOGGER
#endif
#define WBA_LOGGER GPL_LOGGER
#ifdef WBA_SETFILELINE
#undef WBA_SETFILELINE
#endif
#define WBA_SETFILELINE	{WBA__FILE__=(char*)__FILE__; WBA__LINE__=(long)__LINE__;}
// WARNING: WBA_LOG is not 'one-line' safe (if(exp) WBA_LOG(args); is invalid)
// so be sure to use parenthesis when necessary.  This cannot be fixed due to
// the vararg in WBA_LOG_INLINE and the inability of macros to handle varargs.
#ifdef WBA_LOG
#undef WBA_LOG
#endif
#define WBA_LOG	wbaLog
#ifdef WBA_DLOG
#undef WBA_DLOG
#endif
#define WBA_DLOG wbaDLog

#include <wba/argument.h>
#include <wba/env.h>
#include <wba/log.h>
#include <wba/executor.h>
#include <wba/app.h>
#include <wba/daemon.h>
#include <wba/lineint.h>
#include <wba/shell.h>

#endif /* WBA_WBA_H */
