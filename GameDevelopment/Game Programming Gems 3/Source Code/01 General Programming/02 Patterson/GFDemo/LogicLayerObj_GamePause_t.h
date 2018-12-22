/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayerObj_GamePause_t.h: interface for the LogicLayerObj_GamePause_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICLAYEROBJ_GAMEPAUSE_T_H__8C2EBDCB_BE32_495A_93CC_3B05C8C2095E__INCLUDED_)
#define AFX_LOGICLAYEROBJ_GAMEPAUSE_T_H__8C2EBDCB_BE32_495A_93CC_3B05C8C2095E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/LogicLayer_t.h"
class Systems_t;
class AVLayerObj_GamePause_t;

class LogicLayerObj_GamePause_t : public LogicLayer_t  
{
public:
	LogicLayerObj_GamePause_t();
	virtual ~LogicLayerObj_GamePause_t();

	virtual bool Connect( Systems_t *pSystems );
	virtual void Disconnect();

	virtual void Update();

	virtual void SetExclusive( bool bEnable );
	virtual bool IsExclusive();
protected:
	Systems_t *m_pSystems;
	AVLayerObj_GamePause_t *m_pAVLayer;
	bool m_bExclusive;
};

#endif // !defined(AFX_LOGICLAYEROBJ_GAMEPAUSE_T_H__8C2EBDCB_BE32_495A_93CC_3B05C8C2095E__INCLUDED_)
