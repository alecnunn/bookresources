/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// TaskObj_ChangeDriver_t.cpp: implementation of the TaskObj_ChangeDriver_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TaskObj_ChangeDriver_t.h"
#include "GFInclude/Systems/Systems_t.h"
#include "GFInclude/Systems/FactorySys_t.h"
#include "GFInclude/Systems/TaskSys_t.h"
#include "GFInclude/Systems/VisualSys_t.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TaskObj_ChangeDriver_t::TaskObj_ChangeDriver_t()
{
	m_nVisualSysDriverID = DRIVER_ID_VISUAL_SYS_D3D8;
}

TaskObj_ChangeDriver_t::~TaskObj_ChangeDriver_t()
{

}

bool TaskObj_ChangeDriver_t::Connect( Systems_t *pSystems )
{
	m_pSystems = pSystems;
	return true;
}

void TaskObj_ChangeDriver_t::Disconnect()
{
}

void TaskObj_ChangeDriver_t::Run()
{
	LOGCALL((m_pSystems->GetLogSys()->WriteOutput(LOGSYS_CHANNEL_TASK,LOGSYS_DETAIL_HIGH,"Entered - TaskObj_ChangeDriver_t::Run\n"));)

	FactorySys_t *pFS = m_pSystems->GetFactorySys();
	pFS->DeleteVisualSys( m_pSystems->GetVisualSys() );
	pFS->SetVisualSysDriverID( m_nVisualSysDriverID );
	m_pSystems->SetVisualSys( pFS->CreateVisualSys() );
	
	m_pSystems->GetTaskSys()->Post_TaskCommand( TaskCommand_t::ASYNC_REMOVE, this );
}

void TaskObj_ChangeDriver_t::SetVisualSysDriverID( int nVisualSysDriverID )
{
	m_nVisualSysDriverID = nVisualSysDriverID;
}

