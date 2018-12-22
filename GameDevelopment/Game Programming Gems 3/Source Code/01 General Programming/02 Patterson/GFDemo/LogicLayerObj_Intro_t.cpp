/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayerObj_Intro_t.cpp: implementation of the LogicLayerObj_Intro_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogicLayerObj_Intro_t.h"
#include "GFInclude/Systems/Systems_t.h"
#include "GFInclude/Systems/ResourceSys_t.h"
#include "GFInclude/Systems/InputSys_t.h"
#include "GFInclude/Systems/TaskSys_t.h"
#include "GFInclude/Framework/FramePlayer_t.h"
#include "AVLayerObj_Intro_t.h"
#include "GameInstanceIDs.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LogicLayerObj_Intro_t::LogicLayerObj_Intro_t()
{
	m_pSystems = 0;
	m_bExclusive = true;
}

LogicLayerObj_Intro_t::~LogicLayerObj_Intro_t()
{

}

bool LogicLayerObj_Intro_t::Connect( Systems_t *pSystems )
{
	m_pSystems = pSystems;

	ResourceSys_t *pResourceSys = m_pSystems->GetResourceSys();
	m_pAVLayer = (AVLayerObj_Intro_t *)pResourceSys->GetAVLayer( INSTANCE_ID_AVLAYER_INTRO );

	return true;
}

void LogicLayerObj_Intro_t::Disconnect()
{
}

void LogicLayerObj_Intro_t::Update()
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
					Task_t *pTask = pResourceSys->GetTask( INSTANCE_ID_TASK_INTRO );

					// we know it is a frame player
					FramePlayer_t *pFramePlayer = (FramePlayer_t *)pTask;

					// pushing layers
					AVLayer_t *pAVLayer;
					LogicLayer_t *pLogicLayer;

					pAVLayer = pResourceSys->GetAVLayer( INSTANCE_ID_AVLAYER_STARTMENU );
					pFramePlayer->Post_AVLayerCommand( AVLayerCommand_t::PUSH_BACK, pAVLayer );

					pLogicLayer = pResourceSys->GetLogicLayer( INSTANCE_ID_LOGICLAYER_STARTMENU );
					pFramePlayer->Post_LogicLayerCommand( LogicLayerCommand_t::PUSH_BACK, pLogicLayer );
				}
				break;
			case GF_KEYCODE_ESCAPE:
				break;
			}
		}
	}

#if 0
	if( pInputSys->GetKeyState( GF_KEYCODE_DOWN ) )
	{
		m_pAVLayer->m_Box.m_fYPos += 1.0f;
	}
	if( pInputSys->GetKeyState( GF_KEYCODE_UP ) )
	{
		m_pAVLayer->m_Box.m_fYPos -= 1.0f;
	}
	if( pInputSys->GetKeyState( GF_KEYCODE_LEFT ) )
	{
		m_pAVLayer->m_Box.m_fXPos -= 1.0f;
	}
	if( pInputSys->GetKeyState( GF_KEYCODE_RIGHT ) )
	{
		m_pAVLayer->m_Box.m_fXPos += 1.0f;
	}
#endif
}

void LogicLayerObj_Intro_t::SetExclusive( bool bEnable )
{
	m_bExclusive = bEnable;
}

bool LogicLayerObj_Intro_t::IsExclusive()
{
	return m_bExclusive;
}

