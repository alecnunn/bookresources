/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// TimeSysWin32Impl_t.cpp: implementation of the TimeSysWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GFInclude/Systems/Win32/TimeSysWin32Impl_t.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TimeSysWin32Impl_t::TimeSysWin32Impl_t()
{
	m_pSystems = 0;
}

TimeSysWin32Impl_t::~TimeSysWin32Impl_t()
{

}

bool TimeSysWin32Impl_t::Init( Systems_t *pSystems )
{
	m_pSystems = pSystems;
	return true;
}

void TimeSysWin32Impl_t::Shutdown()
{
}


