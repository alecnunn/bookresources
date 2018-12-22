/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayerConfigObj_GameConfig_t.h: interface for the LogicLayerConfigObj_GameConfig_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICLAYERCONFIGOBJ_GAMECONFIG_T_H__CD4BCE65_4C67_4AAB_AD75_216A2BFF4C8A__INCLUDED_)
#define AFX_LOGICLAYERCONFIGOBJ_GAMECONFIG_T_H__CD4BCE65_4C67_4AAB_AD75_216A2BFF4C8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/LogicLayerConfig_t.h"

class LogicLayerConfigObj_GameConfig_t : public LogicLayerConfig_t  
{
public:
	LogicLayerConfigObj_GameConfig_t();
	virtual ~LogicLayerConfigObj_GameConfig_t();

	virtual void Configure( LogicLayer_t *pLogicLayer );
};

#endif // !defined(AFX_LOGICLAYERCONFIGOBJ_GAMECONFIG_T_H__CD4BCE65_4C67_4AAB_AD75_216A2BFF4C8A__INCLUDED_)
