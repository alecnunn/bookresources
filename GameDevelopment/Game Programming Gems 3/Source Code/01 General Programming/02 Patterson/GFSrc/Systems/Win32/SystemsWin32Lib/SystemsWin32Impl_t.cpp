/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// SystemsWin32Impl_t.cpp: implementation of the SystemsWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GFInclude/Systems/Win32/SystemsWin32Impl_t.h"
#include "GFInclude/Systems/Win32/TimeSysWin32Impl_t.h"
#include "GFInclude/Systems/Win32/FactorySysWin32Impl_t.h"
#include "GFInclude/Systems/Win32/ResourceSysWin32Impl_t.h"
#include "GFInclude/Systems/Win32/TaskSysWin32Impl_t.h"
#include "GFInclude/Systems/Win32/LogSysWin32Impl_t.h"
#include "GFInclude/Systems/Win32/ErrorSysWin32Impl_t.h"
#include "GFInclude/Systems/WindowSys_t.h"
#include "GFInclude/Systems/FrameSys_t.h"
#include "GFInclude/Systems/InputSys_t.h"
#include "GFInclude/Systems/VisualSys_t.h"
#include "GFInclude/Systems/AudioSys_t.h"
#include "GFInclude/Systems/NetworkSys_t.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SystemsWin32Impl_t::SystemsWin32Impl_t()
{
	m_pProcessInfo = 0;
	m_pTimeSys = 0;
	m_pFactorySys = 0;
	m_pResourceSys = 0;
	m_pTaskSys = 0;
	m_pLogSys = 0;
	m_pErrorSys = 0;
	m_pWindowSys = 0;
	m_pFrameSys = 0;
	m_pInputSys = 0;
	m_pVisualSys = 0;
	m_pAudioSys = 0;
	m_pNetworkSys = 0;
}

SystemsWin32Impl_t::~SystemsWin32Impl_t()
{

}

bool SystemsWin32Impl_t::Init( ProcessInfo_t *pProcessInfo )
{
	m_pProcessInfo = pProcessInfo;

	m_pLogSys = new LogSysWin32Impl_t;
	if( !m_pLogSys->Init( this ) )
	{
		Shutdown();
		return false;
	}
	m_pErrorSys = new ErrorSysWin32Impl_t;
	if( !m_pErrorSys->Init( this ) )
	{
		Shutdown();
		return false;
	}
	m_pTimeSys = new TimeSysWin32Impl_t;
	if( !m_pTimeSys->Init( this ) )
	{
		Shutdown();
		return false;
	}
	m_pFactorySys = new FactorySysWin32Impl_t;
	if( !m_pFactorySys->Init( this ) )
	{
		Shutdown();
		return false;
	}

	// now that we have a factory system,
	// use the factory sys for the remaining systems

	if( !(m_pResourceSys = m_pFactorySys->CreateResourceSys()) )
	{
		Shutdown();
		return false;
	}

	// lets use the factory sys for the remaining systems
	if( !(m_pTaskSys = m_pFactorySys->CreateTaskSys()) )
	{
		// write log info and update error status
		Shutdown();
		return false;
	}

	if( !(m_pWindowSys = m_pFactorySys->CreateWindowSys()) )
	{
		// write log info and update error status
		// run error display task
		Shutdown();
		return false;
	}

	if( !(m_pFrameSys = m_pFactorySys->CreateFrameSys()) )
	{
		// write log info and update error status
		// run error display task
		Shutdown();
		return false;
	}

	if( !(m_pInputSys = m_pFactorySys->CreateInputSys()) )
	{
		// write log info and update error status
		// run error display task
		Shutdown();
		return false;
	}

	if( !(m_pVisualSys = m_pFactorySys->CreateVisualSys()) )
	{
		// write log info and update error status
		// run error display task
		Shutdown();
		return false;
	}

	if( !(m_pAudioSys = m_pFactorySys->CreateAudioSys()) )
	{
		// write log info and update error status
		// run error display task
		Shutdown();
		return false;
	}

	if( !(m_pNetworkSys = m_pFactorySys->CreateNetworkSys()) )
	{
		// write log info and update error status
		// run error display task
		Shutdown();
		return false;
	}

	return true;
}

void SystemsWin32Impl_t::Shutdown()
{
	if( m_pNetworkSys )
	{
		m_pFactorySys->DeleteNetworkSys( m_pNetworkSys );
		m_pNetworkSys = 0;
	}
	if( m_pAudioSys )
	{
		m_pFactorySys->DeleteAudioSys( m_pAudioSys );
		m_pAudioSys = 0;
	}
	if( m_pVisualSys )
	{
		m_pFactorySys->DeleteVisualSys( m_pVisualSys );
		m_pVisualSys = 0;
	}
	if( m_pInputSys )
	{
		m_pFactorySys->DeleteInputSys( m_pInputSys );
		m_pInputSys = 0;
	}
	if( m_pFrameSys )
	{
		m_pFactorySys->DeleteFrameSys( m_pFrameSys );
		m_pFrameSys = 0;
	}
	if( m_pWindowSys )
	{
		m_pFactorySys->DeleteWindowSys( m_pWindowSys );
		m_pWindowSys = 0;
	}
	if( m_pTaskSys )
	{
		m_pFactorySys->DeleteTaskSys( m_pTaskSys );
		m_pTaskSys = 0;
	}
	if( m_pResourceSys )
	{
		m_pResourceSys->Shutdown();
		delete m_pResourceSys;
		m_pResourceSys = 0;
	}
	if( m_pFactorySys )
	{
		m_pFactorySys->Shutdown();
		delete m_pFactorySys;
		m_pFactorySys = 0;
	}
	if( m_pTimeSys )
	{
		m_pTimeSys->Shutdown();
		delete m_pTimeSys;
		m_pTimeSys = 0;
	}
	if( m_pErrorSys )
	{
		m_pErrorSys->Shutdown();
		delete m_pErrorSys;
		m_pErrorSys = 0;
	}
	if( m_pLogSys )
	{
		m_pLogSys->Shutdown();
		delete m_pLogSys;
		m_pLogSys = 0;
	}
}

void SystemsWin32Impl_t::Process()
{
	if( m_pWindowSys )
	{
		m_pWindowSys->Process();
	}
}
