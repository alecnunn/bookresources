// FrameSysWin32Impl_t.cpp: implementation of the FrameSysWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GFInclude/Systems/Win32/FrameSysWin32Impl_t.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FrameSysWin32Impl_t::FrameSysWin32Impl_t()
{
	m_pSystems = 0;
}

FrameSysWin32Impl_t::~FrameSysWin32Impl_t()
{

}

bool FrameSysWin32Impl_t::Init( Systems_t *pSystems )
{
	m_pSystems = pSystems;
	return true;
}

void FrameSysWin32Impl_t::Shutdown()
{
}

bool FrameSysWin32Impl_t::StartFrameSync()
{
	return true;
}

void FrameSysWin32Impl_t::EndFrameSync()
{
}

bool FrameSysWin32Impl_t::StartAsync()
{
	return true;
}

void FrameSysWin32Impl_t::EndAsync()
{
}


