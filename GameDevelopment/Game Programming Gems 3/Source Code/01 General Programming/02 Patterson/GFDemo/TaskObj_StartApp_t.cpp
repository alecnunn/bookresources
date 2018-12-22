/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// TaskObj_StartApp_t.cpp: implementation of the TaskObj_StartApp_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TaskObj_StartApp_t.h"
#include "GFInclude/Systems/Systems_t.h"
#include "GFInclude/Systems/ResourceSys_t.h"
#include "GFInclude/Systems/TaskSys_t.h"
#include "GFInclude/Systems/VisualSys_t.h"
#include "GFInclude/Framework/FramePlayer_t.h"
#include "GameCollectionIDs.h"
#include "GameInstanceIDs.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TaskObj_StartApp_t::TaskObj_StartApp_t()
{
	m_pSystems = 0;
}

TaskObj_StartApp_t::~TaskObj_StartApp_t()
{

}

bool TaskObj_StartApp_t::Connect( Systems_t *pSystems )
{
	m_pSystems = pSystems;
	return true;
}

void TaskObj_StartApp_t::Disconnect()
{
}

void TaskObj_StartApp_t::Run()
{
	LOGCALL((m_pSystems->GetLogSys()->WriteOutput(LOGSYS_CHANNEL_TASK,LOGSYS_DETAIL_HIGH,"Entered - TaskObj_StartApp_t::Run\n"));)

	// get the resource sys
	ResourceSys_t *pResourceSys = m_pSystems->GetResourceSys();

	// start the collection load necessary for the game tasks and layers
	pResourceSys->StartCollectionLoad( COLLECTION_ID_GAMEPACK_1 );

	// wait till the colleciton is loaded
	while( ResourceSys_t::COLLECTION_STATUS_LOADED != 
		   pResourceSys->GetCollectionStatus( COLLECTION_ID_GAMEPACK_1 ) )
	{
		// keep calling until loaded
	}

	m_pSystems->GetTaskSys()->Post_TaskCommand( TaskCommand_t::ASYNC_REMOVE, this );

	// get task instance from resource sys
	Task_t *pTask = pResourceSys->GetTask( INSTANCE_ID_TASK_INTRO );

	// post this task to the task sys
	m_pSystems->GetTaskSys()->Post_TaskCommand( TaskCommand_t::FRAMESYNC_PUSH_BACK, pTask );
}

