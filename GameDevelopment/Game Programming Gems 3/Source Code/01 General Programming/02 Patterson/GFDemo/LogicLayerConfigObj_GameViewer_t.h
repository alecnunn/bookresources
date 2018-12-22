/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayerConfigObj_GameViewer_t.h: interface for the LogicLayerConfigObj_GameViewer_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICLAYERCONFIGOBJ_GAMEVIEWER_T_H__BB82D033_7EC4_4AD7_A3BA_BE4F2CD96ADD__INCLUDED_)
#define AFX_LOGICLAYERCONFIGOBJ_GAMEVIEWER_T_H__BB82D033_7EC4_4AD7_A3BA_BE4F2CD96ADD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/LogicLayerConfig_t.h"

class LogicLayerConfigObj_GameViewer_t : public LogicLayerConfig_t  
{
public:
	LogicLayerConfigObj_GameViewer_t();
	virtual ~LogicLayerConfigObj_GameViewer_t();

	virtual void Configure( LogicLayer_t *pLogicLayer );
};

#endif // !defined(AFX_LOGICLAYERCONFIGOBJ_GAMEVIEWER_T_H__BB82D033_7EC4_4AD7_A3BA_BE4F2CD96ADD__INCLUDED_)
