/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayerObj_SystemConfig_t.cpp: implementation of the LogicLayerObj_SystemConfig_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogicLayerObj_SystemConfig_t.h"
#include "GFInclude/Systems/Systems_t.h"
#include "GFInclude/Systems/FactorySys_t.h"
#include "GFInclude/Systems/ResourceSys_t.h"
#include "GFInclude/Systems/InputSys_t.h"
#include "GFInclude/Systems/TaskSys_t.h"
#include "AVLayerObj_SystemConfig_t.h"
#include "TaskObj_ChangeDriver_t.h"
#include "GameInstanceIDs.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LogicLayerObj_SystemConfig_t::LogicLayerObj_SystemConfig_t()
{
	m_pSystems = 0;
	m_bExclusive = true;
}

LogicLayerObj_SystemConfig_t::~LogicLayerObj_SystemConfig_t()
{

}

bool LogicLayerObj_SystemConfig_t::Connect( Systems_t *pSystems )
{
	m_pSystems = pSystems;

	ResourceSys_t *pResourceSys = m_pSystems->GetResourceSys();
	m_pAVLayer = (AVLayerObj_SystemConfig_t *)pResourceSys->GetAVLayer( INSTANCE_ID_AVLAYER_SYSTEMCONFIG );

	return true;
}

void LogicLayerObj_SystemConfig_t::Disconnect()
{
}

void LogicLayerObj_SystemConfig_t::Update()
{
	InputSys_t *pInputSys = m_pSystems->GetInputSys();

	KeyEvent_t KeyEvent;
	while( pInputSys->ReadKey( KeyEvent ) )
	{
		if( KeyEvent.m_nType == KeyEvent_t::EVENT_KEY_DOWN )
		{
			switch( KeyEvent.m_nKeyCode )
			{
			case GF_KEYCODE_UP:
				m_pAVLayer->m_MenuBox.PrevChoice();
				break;
			case GF_KEYCODE_DOWN:
				m_pAVLayer->m_MenuBox.NextChoice();
				break;
			case GF_KEYCODE_LEFT:
				m_pAVLayer->m_MenuBox.PrevChoice();
				break;
			case GF_KEYCODE_RIGHT:
				m_pAVLayer->m_MenuBox.NextChoice();
				break;
			case GF_KEYCODE_ENTER:
				if( m_pSystems->GetFactorySys()->GetVisualSysDriverID() != m_pAVLayer->m_MenuBox.GetChoice() )
				{
					// get task instance from resource sys
					Task_t *pTask = m_pSystems->GetResourceSys()->GetTask( INSTANCE_ID_TASK_CHANGE_DRIVER );
					TaskObj_ChangeDriver_t *pCDTask = (TaskObj_ChangeDriver_t *)pTask;
					pCDTask->SetVisualSysDriverID( m_pAVLayer->m_MenuBox.GetChoice() );
					// post this task to the task sys
					m_pSystems->GetTaskSys()->Post_TaskCommand( TaskCommand_t::ASYNC_PUSH_BACK, pTask );
				}
				break;
			case GF_KEYCODE_ESCAPE:
				{
					// get the resource sys
					ResourceSys_t *pResourceSys = m_pSystems->GetResourceSys();

					// get task instance from resource sys
					Task_t *pTask = pResourceSys->GetTask( INSTANCE_ID_TASK_SYSTEMCONFIG );

					m_pSystems->GetTaskSys()->Post_TaskCommand( TaskCommand_t::FRAMESYNC_REMOVE, pTask );

					// get task instance from resource sys
					pTask = pResourceSys->GetTask( INSTANCE_ID_TASK_GAME );

					// post this task to the task sys
					m_pSystems->GetTaskSys()->Post_TaskCommand( TaskCommand_t::FRAMESYNC_PUSH_BACK, pTask );
				}
				break;
			}
		}
	}
}

void LogicLayerObj_SystemConfig_t::SetExclusive( bool bEnable )
{
	m_bExclusive = bEnable;
}

bool LogicLayerObj_SystemConfig_t::IsExclusive()
{
	return m_bExclusive;
}

