/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayerConfigObj_GameLose_t.h: interface for the LogicLayerConfigObj_GameLose_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICLAYERCONFIGOBJ_GAMELOSE_T_H__FA9940B6_0423_417D_ACCF_4EEAFD5983B0__INCLUDED_)
#define AFX_LOGICLAYERCONFIGOBJ_GAMELOSE_T_H__FA9940B6_0423_417D_ACCF_4EEAFD5983B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/LogicLayerConfig_t.h"

class LogicLayerConfigObj_GameLose_t : public LogicLayerConfig_t  
{
public:
	LogicLayerConfigObj_GameLose_t();
	virtual ~LogicLayerConfigObj_GameLose_t();

	virtual void Configure( LogicLayer_t *pLogicLayer );
};

#endif // !defined(AFX_LOGICLAYERCONFIGOBJ_GAMELOSE_T_H__FA9940B6_0423_417D_ACCF_4EEAFD5983B0__INCLUDED_)
