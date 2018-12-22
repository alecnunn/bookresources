/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayerObj_GamePause_t.cpp: implementation of the LogicLayerObj_GamePause_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogicLayerObj_GamePause_t.h"
#include "GFInclude/Systems/Systems_t.h"
#include "GFInclude/Systems/ResourceSys_t.h"
#include "GFInclude/Systems/InputSys_t.h"
#include "GFInclude/Systems/TaskSys_t.h"
#include "GFInclude/Framework/FramePlayer_t.h"
#include "AVLayerObj_GamePause_t.h"
#include "GameInstanceIDs.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LogicLayerObj_GamePause_t::LogicLayerObj_GamePause_t()
{
	m_pSystems = 0;
	m_bExclusive = true;
}

LogicLayerObj_GamePause_t::~LogicLayerObj_GamePause_t()
{

}

bool LogicLayerObj_GamePause_t::Connect( Systems_t *pSystems )
{
	m_pSystems = pSystems;

	ResourceSys_t *pResourceSys = m_pSystems->GetResourceSys();
	m_pAVLayer = (AVLayerObj_GamePause_t *)pResourceSys->GetAVLayer( INSTANCE_ID_AVLAYER_GAMEPAUSE );

	return true;
}

void LogicLayerObj_GamePause_t::Disconnect()
{
}

void LogicLayerObj_GamePause_t::Update()
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
				{
					switch( m_pAVLayer->m_MenuBox.GetChoice() )
					{
					case 0:
						{
							// get the resource sys
							ResourceSys_t *pResourceSys = m_pSystems->GetResourceSys();

							// get task instance from resource sys
							Task_t *pTask = pResourceSys->GetTask( INSTANCE_ID_TASK_GAME );

							// we know it is a frame player
							FramePlayer_t *pFramePlayer = (FramePlayer_t *)pTask;

							// pushing layers
							AVLayer_t *pAVLayer;
							LogicLayer_t *pLogicLayer;

							pAVLayer = pResourceSys->GetAVLayer( INSTANCE_ID_AVLAYER_GAMEPAUSE );
							pFramePlayer->Post_AVLayerCommand( AVLayerCommand_t::REMOVE, pAVLayer );

							pLogicLayer = pResourceSys->GetLogicLayer( INSTANCE_ID_LOGICLAYER_GAMEPAUSE );
							pFramePlayer->Post_LogicLayerCommand( LogicLayerCommand_t::REMOVE, pLogicLayer );
						}
						break;
					case 1:
						{
							// get the resource sys
							ResourceSys_t *pResourceSys = m_pSystems->GetResourceSys();

							// get task instance from resource sys
							Task_t *pTask = pResourceSys->GetTask( INSTANCE_ID_TASK_GAME );

							m_pSystems->GetTaskSys()->Post_TaskCommand( TaskCommand_t::FRAMESYNC_REMOVE, pTask );

							// get task instance from resource sys
							pTask = pResourceSys->GetTask( INSTANCE_ID_TASK_SYSTEMCONFIG );

							// post this task to the task sys
							m_pSystems->GetTaskSys()->Post_TaskCommand( TaskCommand_t::FRAMESYNC_PUSH_BACK, pTask );
						}
						break;
					case 2:
						{
							// remove all tasks to exit
							m_pSystems->GetTaskSys()->Post_TaskCommand( TaskCommand_t::FRAMESYNC_REMOVE_ALL, 0 );
							m_pSystems->GetTaskSys()->Post_TaskCommand( TaskCommand_t::ASYNC_REMOVE_ALL, 0 );

						}
						break;
					}
				}
				break;
			case GF_KEYCODE_ESCAPE:
				{
					// get the resource sys
					ResourceSys_t *pResourceSys = m_pSystems->GetResourceSys();

					// get task instance from resource sys
					Task_t *pTask = pResourceSys->GetTask( INSTANCE_ID_TASK_GAME );

					// we know it is a frame player
					FramePlayer_t *pFramePlayer = (FramePlayer_t *)pTask;

					// pushing layers
					AVLayer_t *pAVLayer;
					LogicLayer_t *pLogicLayer;

					pAVLayer = pResourceSys->GetAVLayer( INSTANCE_ID_AVLAYER_GAMEPAUSE );
					pFramePlayer->Post_AVLayerCommand( AVLayerCommand_t::REMOVE, pAVLayer );

					pLogicLayer = pResourceSys->GetLogicLayer( INSTANCE_ID_LOGICLAYER_GAMEPAUSE );
					pFramePlayer->Post_LogicLayerCommand( LogicLayerCommand_t::REMOVE, pLogicLayer );
				}
				break;
			}
		}
	}
}

void LogicLayerObj_GamePause_t::SetExclusive( bool bEnable )
{
	m_bExclusive = bEnable;
}

bool LogicLayerObj_GamePause_t::IsExclusive()
{
	return m_bExclusive;
}

