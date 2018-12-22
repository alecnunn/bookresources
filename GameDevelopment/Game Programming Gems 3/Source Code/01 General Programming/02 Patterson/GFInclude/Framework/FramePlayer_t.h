/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// FramePlayer_t.h: interface for the FramePlayer_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRAMEPLAYER_T_H__9DA0B239_804F_4979_B550_3A2E0244A18D__INCLUDED_)
#define AFX_FRAMEPLAYER_T_H__9DA0B239_804F_4979_B550_3A2E0244A18D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/Task_t.h"
#include "GFInclude/Framework/AVLayer_t.h"
#include "GFInclude/Framework/AVLayerCommand_t.h"
#include "GFInclude/Framework/LogicLayer_t.h"
#include "GFInclude/Framework/LogicLayerCommand_t.h"

class FramePlayer_t : public Task_t  
{
public:
	FramePlayer_t();
	virtual ~FramePlayer_t();

	virtual bool Connect( Systems_t *pSystems );
	virtual void Disconnect();

	virtual void Run();

	void Post_AVLayerCommand( AVLayerCommand_t::AVLayerCommandID_t nID, AVLayer_t *pAVLayer );
	bool Process_AVLayerCommands();

	void Post_LogicLayerCommand( LogicLayerCommand_t::LogicLayerCommandID_t nID, LogicLayer_t *pLogicLayer );
	bool Process_LogicLayerCommands();

protected:
	void ManageLayers();

	Systems_t *m_pSystems;

	// holds the commands to our task system that are executed at the
	// top of the next run loop
	AVLayerCommandVector_t m_AVLayerCommandVector;
	// holds the active audio-visual layers
	AVLayerPtrList_t m_AVLayerPtrList;
	// holds the commands to our task system that are executed at the
	// top of the next run loop
	LogicLayerCommandVector_t m_LogicLayerCommandVector;
	// holds the active logic layers
	LogicLayerPtrList_t m_LogicLayerPtrList;
};

#endif // !defined(AFX_FRAMEPLAYER_T_H__9DA0B239_804F_4979_B550_3A2E0244A18D__INCLUDED_)
