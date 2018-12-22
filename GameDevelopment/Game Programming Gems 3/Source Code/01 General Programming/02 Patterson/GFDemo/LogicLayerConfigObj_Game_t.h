/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayerConfigObj_Game_t.h: interface for the LogicLayerConfigObj_Game_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICLAYERCONFIGOBJ_GAME_T_H__4CE2F4B1_891F_4B66_B525_58225B1112CF__INCLUDED_)
#define AFX_LOGICLAYERCONFIGOBJ_GAME_T_H__4CE2F4B1_891F_4B66_B525_58225B1112CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/LogicLayerConfig_t.h"

class LogicLayerConfigObj_Game_t : public LogicLayerConfig_t  
{
public:
	LogicLayerConfigObj_Game_t();
	virtual ~LogicLayerConfigObj_Game_t();

	virtual void Configure( LogicLayer_t *pLogicLayer );
};

#endif // !defined(AFX_LOGICLAYERCONFIGOBJ_GAME_T_H__4CE2F4B1_891F_4B66_B525_58225B1112CF__INCLUDED_)
