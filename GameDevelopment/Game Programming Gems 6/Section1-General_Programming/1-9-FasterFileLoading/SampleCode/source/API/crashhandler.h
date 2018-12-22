// ----------------------------------------------------------------------- //
//
// MODULE  : CrashHandler.h
//
// PURPOSE : Handle the creation of Mini Dump files when the app crashes
//
// Based on code from "Game Programming Complete" by Mike McShaffry.
//
// ----------------------------------------------------------------------- //


#ifndef _LTCRASH_HANDLER_H_
#define _LTCRASH_HANDLER_H_

#include <Tlhelp32.h>
#include "dbghelp.h"

#include <list>

class CrashHandler
{
public:
	CrashHandler(bool init);

protected:
	static CrashHandler *gpHandler;
	static LONG WINAPI Handler( struct _EXCEPTION_POINTERS *pExceptionInfo );

	_EXCEPTION_POINTERS *m_pExceptionInfo;
	time_t m_nTime;

	TCHAR m_szDumpPath[_MAX_PATH];
	LONG WriteMiniDump(_EXCEPTION_POINTERS *pExceptionInfo );

	virtual MINIDUMP_USER_STREAM_INFORMATION *GetUserStreamInfo() { return NULL; }
};

#endif
