/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// FactorySysWin32Impl_t.cpp: implementation of the FactorySysWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GFInclude/Systems/Systems_t.h"
#include "GFInclude/Systems/GameFactory_t.h"
#include "GFInclude/Systems/Win32/FactorySysWin32Impl_t.h"
#include "GFInclude/Systems/Win32/ResourceSysWin32Impl_t.h"
#include "GFInclude/Systems/Win32/TaskSysWin32Impl_t.h"
#include "GFInclude/Systems/Win32/WindowSysWin32Impl_t.h"
#include "GFInclude/Systems/Win32/FrameSysWin32Impl_t.h"
#include "GFInclude/Systems/Win32/InputSysWin32Impl_t.h"
#include "GFInclude/Systems/VisualSys_t.h"
#include "GFInclude/Systems/Win32/AudioSysDSoundImpl_t.h"
#include "GFInclude/Systems/Win32/NetworkSysWinsockImpl_t.h"

extern "C"
{
	// this function is exported from any visual system dll
	VisualSys_t *CreateDllVisualSys();
	// the following typedef defines a function pointer called CreateDllVisualSysFuncPtr_t
	// that points to a function with the same interface as CreateDllVisualSys
	typedef VisualSys_t *(*CreateDllVisualSysFuncPtr_t)();

	// this function is exported from any visual system dll
	void DeleteDllVisualSys( VisualSys_t *pVisualSys );
	// the following typedef defines a function pointer called DeleteDllVisualSysFuncPtr_t
	// that points to a function with the same interface as DeleteDllVisualSys
	typedef VisualSys_t *(*DeleteDllVisualSysFuncPtr_t)( VisualSys_t *pVisualSys );
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FactorySysWin32Impl_t::FactorySysWin32Impl_t()
{
	m_pSystems = 0;
	m_pGameFactory = 0;
   	m_hVisualSysDll = 0;

	// choose a visual sys driver to start with 
	//m_nVisualSysDriverID = DRIVER_ID_VISUAL_SYS_D3D7;
	m_nVisualSysDriverID = DRIVER_ID_VISUAL_SYS_D3D8;
	//m_nVisualSysDriverID = DRIVER_ID_VISUAL_SYS_OPENGL;
}

FactorySysWin32Impl_t::~FactorySysWin32Impl_t()
{

}

bool FactorySysWin32Impl_t::Init( Systems_t *pSystems )
{
	m_pSystems = pSystems;
	return true;
}

void FactorySysWin32Impl_t::Shutdown()
{
}

void FactorySysWin32Impl_t::RegisterGameFactory( GameFactory_t *pGameFactory )
{
	m_pGameFactory = pGameFactory;
}

void FactorySysWin32Impl_t::SetVisualSysDriverID( int nVisualSysDriverID )
{
	m_nVisualSysDriverID = nVisualSysDriverID;
}

int FactorySysWin32Impl_t::GetVisualSysDriverID()
{
	return m_nVisualSysDriverID;
}

AVLayer_t * FactorySysWin32Impl_t::CreateAVLayer( int nFactoryID )
{
	if( m_pGameFactory )
	{
		return m_pGameFactory->CreateAVLayer( nFactoryID );
	}
	return 0;
}

void FactorySysWin32Impl_t::ConfigAVLayer( AVLayer_t *pAVLayer, int nConfigID )
{
	if( m_pGameFactory )
	{
		m_pGameFactory->ConfigAVLayer( pAVLayer, nConfigID );
	}
}

void FactorySysWin32Impl_t::DeleteAVLayer( AVLayer_t *pAVLayer )
{
	if( m_pGameFactory )
	{
		m_pGameFactory->DeleteAVLayer( pAVLayer );
	}
}

LogicLayer_t * FactorySysWin32Impl_t::CreateLogicLayer( int nFactoryID )
{
	if( m_pGameFactory )
	{
		return m_pGameFactory->CreateLogicLayer( nFactoryID );
	}
	return 0;
}

void FactorySysWin32Impl_t::ConfigLogicLayer( LogicLayer_t *pLogicLayer, int nConfigID )
{
	if( m_pGameFactory )
	{
		m_pGameFactory->ConfigLogicLayer( pLogicLayer, nConfigID );
	}
}

void FactorySysWin32Impl_t::DeleteLogicLayer( LogicLayer_t *pLogicLayer )
{
	if( m_pGameFactory )
	{
		m_pGameFactory->DeleteLogicLayer( pLogicLayer );
	}
}

Task_t * FactorySysWin32Impl_t::CreateTask( int nFactoryID )
{
	if( m_pGameFactory )
	{
		return m_pGameFactory->CreateTask( nFactoryID );
	}
	return 0;
}

void FactorySysWin32Impl_t::ConfigTask( Task_t *pTask, int nConfigID )
{
	if( m_pGameFactory )
	{
		m_pGameFactory->ConfigTask( pTask, nConfigID, m_pSystems );
	}
}

void FactorySysWin32Impl_t::DeleteTask( Task_t *pTask )
{
	if( m_pGameFactory )
	{
		m_pGameFactory->DeleteTask( pTask );
	}
}

ResourceSys_t * FactorySysWin32Impl_t::CreateResourceSys()
{
	ResourceSys_t *pResourceSys = new ResourceSysWin32Impl_t;
	if( !pResourceSys->Init( m_pSystems ) )
	{
		delete pResourceSys;
		pResourceSys = 0;
	}
	return pResourceSys;
}

void FactorySysWin32Impl_t::DeleteResourceSys( ResourceSys_t *pResourceSys )
{
	pResourceSys->Shutdown();
	delete pResourceSys;
}

TaskSys_t * FactorySysWin32Impl_t::CreateTaskSys()
{
	TaskSys_t *pTaskSys = new TaskSysWin32Impl_t;
	if( !pTaskSys->Init( m_pSystems ) )
	{
		delete pTaskSys;
		pTaskSys = 0;
	}
	return pTaskSys;
}

void FactorySysWin32Impl_t::DeleteTaskSys( TaskSys_t *pTaskSys )
{
	pTaskSys->Shutdown();
	delete pTaskSys;
}

WindowSys_t * FactorySysWin32Impl_t::CreateWindowSys()
{
	WindowSys_t *pWindowSys = new WindowSysWin32Impl_t;
	pWindowSys->SetProcessInfo( m_pSystems->GetProcessInfo() );
	if( !pWindowSys->Init( m_pSystems ) )
	{
		delete pWindowSys;
		pWindowSys = 0;
	}
	return pWindowSys;
}

void FactorySysWin32Impl_t::DeleteWindowSys( WindowSys_t *pWindowSys )
{
	pWindowSys->Shutdown();
	delete pWindowSys;
}

FrameSys_t * FactorySysWin32Impl_t::CreateFrameSys()
{
	FrameSys_t *pFrameSys = new FrameSysWin32Impl_t;
	if( !pFrameSys->Init( m_pSystems ) )
	{
		delete pFrameSys;
		pFrameSys = 0;
	}
	return pFrameSys;
}

void FactorySysWin32Impl_t::DeleteFrameSys( FrameSys_t *pFrameSys )
{
	pFrameSys->Shutdown();
	delete pFrameSys;
}

InputSys_t * FactorySysWin32Impl_t::CreateInputSys()
{
	InputSys_t *pInputSys = new InputSysWin32Impl_t;
	if( !pInputSys->Init( m_pSystems ) )
	{
		delete pInputSys;
		pInputSys = 0;
	}
	return pInputSys;
}

void FactorySysWin32Impl_t::DeleteInputSys( InputSys_t *pInputSys )
{
	pInputSys->Shutdown();
	delete pInputSys;
}

bool FactorySysWin32Impl_t::LoadVisualSysDll( const char *pDllName )
{
	m_hVisualSysDll = LoadLibraryEx(pDllName,NULL,0);
	if(!m_hVisualSysDll)
	{
		MessageBox(NULL,
				   "Error loading up VisualSys Dll",
				   "Fatal Error",
				   MB_OK | MB_ICONERROR);

		return false;
	}

	//MessageBox(NULL, "Loaded up VisualSys Dll", "Info", MB_OK | MB_ICONINFORMATION);
	return true;
}

VisualSys_t * FactorySysWin32Impl_t::CreateVisualSys()
{
	switch( m_nVisualSysDriverID )
	{
	case DRIVER_ID_VISUAL_SYS_D3D7:
		{
		#ifdef _DEBUG
			if( !LoadVisualSysDll( "VisualSysD3D7DllD.dll" ) )
			{
				return 0;
			}
		#else
			if( !LoadVisualSysDll( "VisualSysD3D7Dll.dll" ) )
			{
				return 0;
			}
		#endif
		}
		break;
	case DRIVER_ID_VISUAL_SYS_D3D8:
		{
		#ifdef _DEBUG
			if( !LoadVisualSysDll( "VisualSysD3D8DllD.dll" ) )
			{
				return 0;
			}
		#else
			if( !LoadVisualSysDll( "VisualSysD3D8Dll.dll" ) )
			{
				return 0;
			}
		#endif
		}
		break;
	case DRIVER_ID_VISUAL_SYS_OPENGL:
		{
		#ifdef _DEBUG
			if( !LoadVisualSysDll( "VisualSysOpenGLDllD.dll" ) )
			{
				return 0;
			}
		#else
			if( !LoadVisualSysDll( "VisualSysOpenGLDll.dll" ) )
			{
				return 0;
			}
		#endif
		}
		break;
	default:
		return 0;
	}

	CreateDllVisualSysFuncPtr_t CreateVSysFuncPtr = 0;
	CreateVSysFuncPtr = (CreateDllVisualSysFuncPtr_t)GetProcAddress(m_hVisualSysDll, "CreateDllVisualSys");
	if( !CreateVSysFuncPtr )
	{
		return 0;
	}

	VisualSys_t *pVisualSys = CreateVSysFuncPtr();
	if( !pVisualSys )
	{
		return 0;
	}

	if( !pVisualSys->Init( m_pSystems ) )
	{
		delete pVisualSys;
		pVisualSys = 0;
	}

#if 0
	if( pVisualSys )
	{
		MessageBox(NULL, "VisualSys Created!", "Info", MB_OK | MB_ICONINFORMATION);
	}
#endif
	return pVisualSys;
}

void FactorySysWin32Impl_t::DeleteVisualSys( VisualSys_t *pVisualSys )
{
	if( pVisualSys )
	{
		pVisualSys->Shutdown();

		DeleteDllVisualSysFuncPtr_t DeleteVSysFuncPtr;
		DeleteVSysFuncPtr = (DeleteDllVisualSysFuncPtr_t)GetProcAddress(m_hVisualSysDll, "DeleteDllVisualSys");
		DeleteVSysFuncPtr( pVisualSys );
	}
}

AudioSys_t * FactorySysWin32Impl_t::CreateAudioSys()
{
	AudioSys_t *pAudioSys = new AudioSysDSoundImpl_t;
	if( !pAudioSys->Init( m_pSystems ) )
	{
		delete pAudioSys;
		pAudioSys = 0;
	}
	return pAudioSys;
}

void FactorySysWin32Impl_t::DeleteAudioSys( AudioSys_t *pAudioSys )
{
	pAudioSys->Shutdown();
	delete pAudioSys;
}

NetworkSys_t * FactorySysWin32Impl_t::CreateNetworkSys()
{
	NetworkSys_t *pNetworkSys = new NetworkSysWinsockImpl_t;
	if( !pNetworkSys->Init( m_pSystems ) )
	{
		delete pNetworkSys;
		pNetworkSys = 0;
	}
	return pNetworkSys;
}

void FactorySysWin32Impl_t::DeleteNetworkSys( NetworkSys_t *pNetworkSys )
{
	pNetworkSys->Shutdown();
	delete pNetworkSys;
}

