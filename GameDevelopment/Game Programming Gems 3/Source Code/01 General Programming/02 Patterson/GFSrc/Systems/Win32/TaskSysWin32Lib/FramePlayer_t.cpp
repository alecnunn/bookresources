/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// FramePlayer_t.cpp: implementation of the FramePlayer_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GFInclude/Framework/FramePlayer_t.h"
#include "GFInclude/Systems/Systems_t.h"
#include "GFInclude/Systems/VisualSys_t.h"
#include "GFInclude/Systems/AudioSys_t.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FramePlayer_t::FramePlayer_t()
{
	m_pSystems = 0;
}

FramePlayer_t::~FramePlayer_t()
{

}

bool FramePlayer_t::Connect( Systems_t *pSystems )
{
	m_pSystems = pSystems;
	return true;
}

void FramePlayer_t::Disconnect()
{
}

void FramePlayer_t::Post_AVLayerCommand( AVLayerCommand_t::AVLayerCommandID_t nID, AVLayer_t *pAVLayer )
{
	AVLayerCommand_t AVLC( nID, pAVLayer );
	m_AVLayerCommandVector.push_back( AVLC );
}

bool FramePlayer_t::Process_AVLayerCommands()
{
	// process any 
	AVLayerCommandVector_t::iterator iAVLCV;
	for( iAVLCV = m_AVLayerCommandVector.begin(); iAVLCV != m_AVLayerCommandVector.end(); ++iAVLCV )
	{
		switch( (*iAVLCV).m_nID )
		{
		case AVLayerCommand_t::PUSH_BACK:
			(*iAVLCV).m_pAVLayer->Connect( m_pSystems );
			m_AVLayerPtrList.push_back( (*iAVLCV).m_pAVLayer );
			break;
		case AVLayerCommand_t::PUSH_FRONT:
			(*iAVLCV).m_pAVLayer->Connect( m_pSystems );
			m_AVLayerPtrList.push_front( (*iAVLCV).m_pAVLayer );
			break;
		case AVLayerCommand_t::REMOVE:
			m_AVLayerPtrList.remove( (*iAVLCV).m_pAVLayer );
			(*iAVLCV).m_pAVLayer->Disconnect();
			break;
		case AVLayerCommand_t::REMOVE_ALL:
			{
				AVLayerPtrList_t::iterator iAVLPL;
				for( iAVLPL = m_AVLayerPtrList.begin(); iAVLPL != m_AVLayerPtrList.end(); ++iAVLPL )
				{
					AVLayer_t *pAVLayer = *iAVLPL;
					pAVLayer->Disconnect();
				}
				m_AVLayerPtrList.clear();
			}
			break;
		default:
			LOGCALL((m_pSystems->GetLogSys()->WriteOutput(LOGSYS_CHANNEL_AVLAYER,LOGSYS_DETAIL_ERROR,"Unknown AVLayer Command Was Skipped: %d\n",(*iAVLCV).m_nID));)
			break;
		}
	}

	// clear all of the commands
	m_AVLayerCommandVector.clear();

	// if no audio-visual layers exist, then we return false
	if( m_AVLayerPtrList.empty() )
	{
		return false;
	}
	return true;
}

void FramePlayer_t::Post_LogicLayerCommand( LogicLayerCommand_t::LogicLayerCommandID_t nID, LogicLayer_t *pLogicLayer )
{
	LogicLayerCommand_t LLC( nID, pLogicLayer );
	m_LogicLayerCommandVector.push_back( LLC );
}

bool FramePlayer_t::Process_LogicLayerCommands()
{
	// process any 
	LogicLayerCommandVector_t::iterator iLLCV;
	for( iLLCV = m_LogicLayerCommandVector.begin(); iLLCV != m_LogicLayerCommandVector.end(); ++iLLCV )
	{
		switch( (*iLLCV).m_nID )
		{
		case LogicLayerCommand_t::PUSH_BACK:
			(*iLLCV).m_pLogicLayer->Connect( m_pSystems );
			m_LogicLayerPtrList.push_back( (*iLLCV).m_pLogicLayer );
			break;
		case LogicLayerCommand_t::PUSH_FRONT:
			(*iLLCV).m_pLogicLayer->Connect( m_pSystems );
			m_LogicLayerPtrList.push_front( (*iLLCV).m_pLogicLayer );
			break;
		case AVLayerCommand_t::REMOVE:
			m_LogicLayerPtrList.remove( (*iLLCV).m_pLogicLayer );
			(*iLLCV).m_pLogicLayer->Disconnect();
			break;
		case AVLayerCommand_t::REMOVE_ALL:
			{
				LogicLayerPtrList_t::iterator iLLPL;
				for( iLLPL = m_LogicLayerPtrList.begin(); iLLPL != m_LogicLayerPtrList.end(); ++iLLPL )
				{
					LogicLayer_t *pLogicLayer = *iLLPL;
					pLogicLayer->Disconnect();
				}
				m_LogicLayerPtrList.clear();
			}
			break;
		default:
			LOGCALL((m_pSystems->GetLogSys()->WriteOutput(LOGSYS_CHANNEL_LOGICLAYER,LOGSYS_DETAIL_ERROR,"Unknown LogicLayer Command Was Skipped: %d\n",(*iLLCV).m_nID));)
			break;
		}
	}

	// clear all of the commands
	m_LogicLayerCommandVector.clear();

	// if no logic layers exist, then we return false
	if( m_LogicLayerPtrList.empty() )
	{
		return false;
	}
	return true;
}

void FramePlayer_t::Run()
{
	Process_AVLayerCommands();
	Process_LogicLayerCommands();

	// Update AV Layers
	AVLayerPtrList_t::iterator iAVLL;
	for( iAVLL = m_AVLayerPtrList.begin(); iAVLL != m_AVLayerPtrList.end(); ++iAVLL )
	{
		AVLayer_t *pAVL = *iAVLL;
		pAVL->Update();
	}

	// Begin Render Visual
	//
	if( m_pSystems->GetVisualSys()->BeginRender() )
	{

		AVLayerPtrList_t::iterator iAVLL;
		for( iAVLL = m_AVLayerPtrList.begin(); iAVLL != m_AVLayerPtrList.end(); ++iAVLL )
		{
			AVLayer_t *pAVL = *iAVLL;
			pAVL->RenderVisual();
		}

		m_pSystems->GetVisualSys()->EndRender();
	}
	//
	// End Render Visual

	// Begin Render Audio
	//
	if( m_pSystems->GetAudioSys()->BeginRender() )
	{

		AVLayerPtrList_t::iterator iAVLL;
		for( iAVLL = m_AVLayerPtrList.begin(); iAVLL != m_AVLayerPtrList.end(); ++iAVLL )
		{
			AVLayer_t *pAVL = *iAVLL;
			pAVL->RenderAudio();
		}

		m_pSystems->GetAudioSys()->EndRender();
	}
	//
	// End Render Audio

	// Update Logic Layers
	LogicLayerPtrList_t::reverse_iterator riLLL;
	for( riLLL = m_LogicLayerPtrList.rbegin(); riLLL != m_LogicLayerPtrList.rend(); ++riLLL )
	{
		LogicLayer_t *pLL = *riLLL;
		pLL->Update();
		if( pLL->IsExclusive() ) break;
	}
}

