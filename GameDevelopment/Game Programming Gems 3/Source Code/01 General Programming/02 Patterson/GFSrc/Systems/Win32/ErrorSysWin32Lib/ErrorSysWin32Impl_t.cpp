/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// ErrorSysWin32Impl_t.cpp: implementation of the ErrorSysWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GFInclude/Systems/Win32/ErrorSysWin32Impl_t.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ErrorSysWin32Impl_t::ErrorSysWin32Impl_t()
{
	m_pSystems = 0;
}

ErrorSysWin32Impl_t::~ErrorSysWin32Impl_t()
{

}

bool ErrorSysWin32Impl_t::Init( Systems_t *pSystems )
{
	m_pSystems = pSystems;
	return true;
}

void ErrorSysWin32Impl_t::Shutdown()
{
}

void ErrorSysWin32Impl_t::Halt(char* pFormat, ...)
{
	va_list arg_list;
	char acOutStr[2048];
	
	va_start(arg_list,pFormat);
	vsprintf(acOutStr,pFormat, arg_list);
	va_end(arg_list);

	MessageBox( NULL, acOutStr, "Halt Error", MB_OK );
	abort();
}

