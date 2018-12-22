/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayerConfigObj_StartMenu_t.h: interface for the LogicLayerConfigObj_StartMenu_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICLAYERCONFIGOBJ_STARTMENU_T_H__1C298C38_EB98_4397_981B_FFD2A8BC4C54__INCLUDED_)
#define AFX_LOGICLAYERCONFIGOBJ_STARTMENU_T_H__1C298C38_EB98_4397_981B_FFD2A8BC4C54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/LogicLayerConfig_t.h"

class LogicLayerConfigObj_StartMenu_t : public LogicLayerConfig_t  
{
public:
	LogicLayerConfigObj_StartMenu_t();
	virtual ~LogicLayerConfigObj_StartMenu_t();

	virtual void Configure( LogicLayer_t *pLogicLayer );
};

#endif // !defined(AFX_LOGICLAYERCONFIGOBJ_STARTMENU_T_H__1C298C38_EB98_4397_981B_FFD2A8BC4C54__INCLUDED_)
