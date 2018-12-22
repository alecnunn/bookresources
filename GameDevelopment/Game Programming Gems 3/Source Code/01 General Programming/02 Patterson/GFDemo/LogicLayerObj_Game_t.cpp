/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayerObj_Game_t.cpp: implementation of the LogicLayerObj_Game_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogicLayerObj_Game_t.h"
#include "GFInclude/Systems/Systems_t.h"
#include "GFInclude/Systems/ResourceSys_t.h"
#include "GFInclude/Systems/InputSys_t.h"
#include "GFInclude/Framework/FramePlayer_t.h"
#include "AVLayerObj_Game_t.h"
#include "AVLayerObj_GameHud_t.h"
#include "GameInstanceIDs.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LogicLayerObj_Game_t::LogicLayerObj_Game_t()
{
	m_pSystems = 0;
	m_bExclusive = true;
	m_bGameHud = false;
}

LogicLayerObj_Game_t::~LogicLayerObj_Game_t()
{

}

bool LogicLayerObj_Game_t::Connect( Systems_t *pSystems )
{
	m_pSystems = pSystems;

	ResourceSys_t *pResourceSys = m_pSystems->GetResourceSys();
	m_pAVLayer = (AVLayerObj_Game_t *)pResourceSys->GetAVLayer( INSTANCE_ID_AVLAYER_GAME );
	m_pAVLayerHud = (AVLayerObj_GameHud_t *)pResourceSys->GetAVLayer( INSTANCE_ID_AVLAYER_GAMEHUD );

	return true;
}

void LogicLayerObj_Game_t::Disconnect()
{
}

void LogicLayerObj_Game_t::Update()
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
				break;
			case GF_KEYCODE_DOWN:
				break;
			case GF_KEYCODE_LEFT:
				break;
			case GF_KEYCODE_RIGHT:
				break;
			case GF_KEYCODE_ENTER:
				{
					// get the resource sys
					ResourceSys_t *pResourceSys = m_pSystems->GetResourceSys();

					// get task instance from resource sys
					Task_t *pTask = pResourceSys->GetTask( INSTANCE_ID_TASK_GAME );

					// we know it is a frame player
					FramePlayer_t *pFramePlayer = (FramePlayer_t *)pTask;

					// pushing/removing layers
					AVLayer_t *pAVLayer;

					pAVLayer = pResourceSys->GetAVLayer( INSTANCE_ID_AVLAYER_GAMEHUD );
					if( !m_bGameHud )
					{
						pFramePlayer->Post_AVLayerCommand( AVLayerCommand_t::PUSH_BACK, pAVLayer );
						m_bGameHud = true;
					} else {
						pFramePlayer->Post_AVLayerCommand( AVLayerCommand_t::REMOVE, pAVLayer );
						m_bGameHud = false;
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
					pFramePlayer->Post_AVLayerCommand( AVLayerCommand_t::PUSH_BACK, pAVLayer );

					pLogicLayer = pResourceSys->GetLogicLayer( INSTANCE_ID_LOGICLAYER_GAMEPAUSE );
					pFramePlayer->Post_LogicLayerCommand( LogicLayerCommand_t::PUSH_BACK, pLogicLayer );
				}
				break;
			}
		}
	}
}

void LogicLayerObj_Game_t::SetExclusive( bool bEnable )
{
	m_bExclusive = bEnable;
}

bool LogicLayerObj_Game_t::IsExclusive()
{
	return m_bExclusive;
}

