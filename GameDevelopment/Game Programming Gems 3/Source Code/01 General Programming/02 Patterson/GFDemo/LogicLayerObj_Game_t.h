/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayerObj_Game_t.h: interface for the LogicLayerObj_Game_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICLAYEROBJ_GAME_T_H__691467CC_22C6_447B_A905_D3952B009CB0__INCLUDED_)
#define AFX_LOGICLAYEROBJ_GAME_T_H__691467CC_22C6_447B_A905_D3952B009CB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/LogicLayer_t.h"
class Systems_t;
class AVLayerObj_Game_t;
class AVLayerObj_GameHud_t;

class LogicLayerObj_Game_t : public LogicLayer_t  
{
public:
	LogicLayerObj_Game_t();
	virtual ~LogicLayerObj_Game_t();

	virtual bool Connect( Systems_t *pSystems );
	virtual void Disconnect();

	virtual void Update();

	virtual void SetExclusive( bool bEnable );
	virtual bool IsExclusive();
protected:
	Systems_t *m_pSystems;
	AVLayerObj_Game_t *m_pAVLayer;
	AVLayerObj_GameHud_t *m_pAVLayerHud;
	bool m_bExclusive;
	bool m_bGameHud;
};

#endif // !defined(AFX_LOGICLAYEROBJ_GAME_T_H__691467CC_22C6_447B_A905_D3952B009CB0__INCLUDED_)
