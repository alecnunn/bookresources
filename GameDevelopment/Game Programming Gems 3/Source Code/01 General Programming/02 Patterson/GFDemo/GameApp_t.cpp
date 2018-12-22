/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// GameApp_t.cpp: implementation of the GameApp_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GFInclude/Systems/Win32/SystemsWin32Impl_t.h"
#include "GFInclude/Systems/ResourceSys_t.h"
#include "GFInclude/Systems/FactorySys_t.h"
#include "GFInclude/Systems/TaskSys_t.h"
#include "GFInclude/Framework/Task_t.h"
#include "GFInclude/Framework/TaskCommand_t.h"

#include "GameApp_t.h"
#include "GameCollectionIDs.h"
#include "GameInstanceIDs.h"
#include "GameFactoryImpl_t.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GameApp_t::GameApp_t()
{
	m_pProcessInfo = 0;
	m_pGameFactory = 0;
}

GameApp_t::~GameApp_t()
{

}

void GameApp_t::SetProcessInfo( ProcessInfo_t *pProcessInfo )
{
	m_pProcessInfo = pProcessInfo;
}

void GameApp_t::Run()
{
	if( Init() )
	{
		m_pSystems->GetTaskSys()->Run();
	}
	// whether or not init worked we shutdown
	Shutdown();
}

bool GameApp_t::Init()
{
	// here is where we choose the systems implementation
	m_pSystems = new SystemsWin32Impl_t;
	if( !m_pSystems->Init( m_pProcessInfo ) ) return false;

	// create the factory for game specific objects
	m_pGameFactory = new GameFactoryImpl_t;
	// register our local game factory with the resource system
	m_pSystems->GetFactorySys()->RegisterGameFactory( m_pGameFactory );

	// get the resource sys
	ResourceSys_t *pResourceSys = m_pSystems->GetResourceSys();
	// assign our local game factory's collection data to the resource system
	pResourceSys->SetCollectionData( m_pGameFactory->GetCollectionData() );

	// start the collection load necessary for application startup
	pResourceSys->StartCollectionLoad( COLLECTION_ID_START_APP );

	// wait till the colleciton is loaded
	while( ResourceSys_t::COLLECTION_STATUS_LOADED != 
		   pResourceSys->GetCollectionStatus( COLLECTION_ID_START_APP ) )
	{
		// keep calling until loaded
	}

	// get start app task instance from resource sys
	Task_t *pTask = pResourceSys->GetTask( INSTANCE_ID_TASK_START_APP );

	// post this task to the task sys
	m_pSystems->GetTaskSys()->Post_TaskCommand( TaskCommand_t::ASYNC_PUSH_BACK, pTask );

	return true;
}

void GameApp_t::Shutdown()
{
	// delete any objects not already deleted
	if( m_pGameFactory )
	{
		delete m_pGameFactory;
	}

	if( m_pSystems )
	{
		m_pSystems->Shutdown();
	}
}
