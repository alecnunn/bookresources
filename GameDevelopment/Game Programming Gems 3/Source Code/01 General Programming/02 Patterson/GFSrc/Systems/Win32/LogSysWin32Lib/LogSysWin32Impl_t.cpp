/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogSysWin32Impl_t.cpp: implementation of the LogSysWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GFInclude/Systems/Win32/LogSysWin32Impl_t.h"

#define ENABLE_WIN32CONSOLE_WINDOW 1

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LogSysWin32Impl_t::LogSysWin32Impl_t()
{
	m_pSystems = 0;
	m_nOutputMode = LOGSYS_OUTPUTMODE_WIN32CONSOLE;
	m_nChannelMask = 0xffff; // enable all channels
	m_nDetailLevel = LOGSYS_DETAIL_HIGH;
}

LogSysWin32Impl_t::~LogSysWin32Impl_t()
{

}

bool LogSysWin32Impl_t::Init( Systems_t *pSystems )
{
	m_pSystems = pSystems;

#if ENABLE_WIN32CONSOLE_WINDOW == 1
	AllocConsole();
	m_hOutputWin = GetStdHandle(STD_OUTPUT_HANDLE);
	m_hInputWin = GetStdHandle(STD_INPUT_HANDLE);

	DWORD cCharsWritten;
	String_t StartString("Win32 Console Started\n");
	WriteConsole(m_hOutputWin, StartString.c_str(), StartString.size(), &cCharsWritten, NULL);
#endif

	return true;
}

void LogSysWin32Impl_t::Shutdown()
{
#if ENABLE_WIN32CONSOLE_WINDOW == 1
	FreeConsole();
#endif
}

void LogSysWin32Impl_t::SetOutputMode(int nOutputMode)
{
	m_nOutputMode = nOutputMode;
}

int LogSysWin32Impl_t::GetOutputMode()
{
	return m_nOutputMode;
}

void LogSysWin32Impl_t::SetChannelMask(int nChannelMask)
{
	m_nChannelMask = nChannelMask;
}

int LogSysWin32Impl_t::GetChannelMask()
{
	return m_nChannelMask;
}

void LogSysWin32Impl_t::SetDetailLevel(int nDetailLevel)
{
	m_nDetailLevel = nDetailLevel;
}

int LogSysWin32Impl_t::GetDetailLevel()
{
	return m_nDetailLevel;
}

void LogSysWin32Impl_t::WriteOutput(int nChannelMask, int nDetailLevel, char* pFormat, ...)
{
	va_list arg_list;
	char acOutStr[2048];
	
	va_start(arg_list,pFormat);
	vsprintf(acOutStr,pFormat, arg_list);
	va_end(arg_list);

	// if nChannel is enabled and nDetail is above threshold, output the data
	if( (m_nChannelMask & nChannelMask) && (m_nDetailLevel >= nDetailLevel) )
	{
		switch( m_nOutputMode )
		{
		case LOGSYS_OUTPUTMODE_NONE:
			// nothing needed here
			break;
		case LOGSYS_OUTPUTMODE_WIN32CONSOLE:
			{
				#if ENABLE_WIN32CONSOLE_WINDOW == 1
				DWORD cCharsWritten;
				WriteConsole(m_hOutputWin, acOutStr, strlen(acOutStr), &cCharsWritten, NULL);
				#endif
			}
			break;
		case LOGSYS_OUTPUTMODE_FILE:
			// haven't done this yet
			break;
		default:
			break;
		}
	}

	// if output mode is set to file
}
