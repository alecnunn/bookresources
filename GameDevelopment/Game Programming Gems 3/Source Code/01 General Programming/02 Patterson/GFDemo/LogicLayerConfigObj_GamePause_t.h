/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayerConfigObj_GamePause_t.h: interface for the LogicLayerConfigObj_GamePause_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICLAYERCONFIGOBJ_GAMEPAUSE_T_H__E528615B_65D9_485F_8991_BB0B4B36FDB2__INCLUDED_)
#define AFX_LOGICLAYERCONFIGOBJ_GAMEPAUSE_T_H__E528615B_65D9_485F_8991_BB0B4B36FDB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/LogicLayerConfig_t.h"

class LogicLayerConfigObj_GamePause_t : public LogicLayerConfig_t  
{
public:
	LogicLayerConfigObj_GamePause_t();
	virtual ~LogicLayerConfigObj_GamePause_t();

	virtual void Configure( LogicLayer_t *pLogicLayer );
};

#endif // !defined(AFX_LOGICLAYERCONFIGOBJ_GAMEPAUSE_T_H__E528615B_65D9_485F_8991_BB0B4B36FDB2__INCLUDED_)
