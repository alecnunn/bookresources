// ----------------------------------------------------------------------- //
//
// MODULE  : CrashHandler.cpp
//
// PURPOSE : Handle the creation of Mini Dump files when the app crashes
//
// Based on code from "Game Programming Complete" by Mike McShaffry.
//
// ----------------------------------------------------------------------- //

#include "StdAfx.h"
#include <time.h>
#include "crashhandler.h"

// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
									CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
									CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
									CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
									);

CrashHandler *CrashHandler::gpHandler = NULL;

CrashHandler::CrashHandler(bool init)
{
	//init is not used.. it's only to please the runtime init of "CrashHandler crash(true);"

	// Detect if there is more than one CrashHandler.
	assert( !gpHandler );

	if (!gpHandler)
	{
		SetUnhandledExceptionFilter( Handler );
		gpHandler = this;
	}
}

LONG CrashHandler::Handler( _EXCEPTION_POINTERS *pExceptionInfo )
{
	int retval = EXCEPTION_CONTINUE_SEARCH;

	if (!gpHandler)
	{
		return retval;
	}

	return gpHandler->WriteMiniDump(pExceptionInfo);
}

LONG CrashHandler::WriteMiniDump(_EXCEPTION_POINTERS *pExceptionInfo )
{
    time( &m_nTime );

	int retval = EXCEPTION_CONTINUE_SEARCH;
	m_pExceptionInfo = pExceptionInfo;

	HMODULE hDll = NULL;

	hDll = ::LoadLibrary( "DBGHELP.DLL" );

	// Did we load it?
	if (hDll)
	{
		MINIDUMPWRITEDUMP pMiniDumpWriteDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
		if (pMiniDumpWriteDump)
		{
			// set the dump file name
			sprintf(m_szDumpPath, "term-%ld.dmp", m_nTime);

			// create the file
			HANDLE hFile = CreateFile( m_szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
											FILE_ATTRIBUTE_NORMAL, NULL );

			if (hFile!=INVALID_HANDLE_VALUE)
			{
				_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

				ExInfo.ThreadId = ::GetCurrentThreadId();
				ExInfo.ExceptionPointers = pExceptionInfo;
				ExInfo.ClientPointers = NULL;

				// write the dump
				BOOL bOK = pMiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, GetUserStreamInfo(), NULL );
				if (bOK)
				{
					retval = EXCEPTION_EXECUTE_HANDLER;
				}
				else
				{
					//Failed to save dump
				}
				CloseHandle(hFile);
			}
			else
			{
				//Failed to create dump
			}
		}
	}
	else
	{
		//DBGHELP.DLL not found
	}

	// kill the app
	TerminateProcess(GetCurrentProcess(), 0);

	return retval;
}
