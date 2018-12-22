/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayerConfigObj_SystemConfig_t.h: interface for the LogicLayerConfigObj_SystemConfig_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICLAYERCONFIGOBJ_SYSTEMCONFIG_T_H__4B101D3A_0AEC_42E4_8CDB_1EA0C32E50A8__INCLUDED_)
#define AFX_LOGICLAYERCONFIGOBJ_SYSTEMCONFIG_T_H__4B101D3A_0AEC_42E4_8CDB_1EA0C32E50A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/LogicLayerConfig_t.h"

class LogicLayerConfigObj_SystemConfig_t : public LogicLayerConfig_t  
{
public:
	LogicLayerConfigObj_SystemConfig_t();
	virtual ~LogicLayerConfigObj_SystemConfig_t();

	virtual void Configure( LogicLayer_t *pLogicLayer );
};

#endif // !defined(AFX_LOGICLAYERCONFIGOBJ_SYSTEMCONFIG_T_H__4B101D3A_0AEC_42E4_8CDB_1EA0C32E50A8__INCLUDED_)
