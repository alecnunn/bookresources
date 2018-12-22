/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayerConfigObj_GameEditor_t.h: interface for the LogicLayerConfigObj_GameEditor_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICLAYERCONFIGOBJ_GAMEEDITOR_T_H__2D0EB6D9_1D50_4984_A0F4_D382D2B8D177__INCLUDED_)
#define AFX_LOGICLAYERCONFIGOBJ_GAMEEDITOR_T_H__2D0EB6D9_1D50_4984_A0F4_D382D2B8D177__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/LogicLayerConfig_t.h"

class LogicLayerConfigObj_GameEditor_t : public LogicLayerConfig_t  
{
public:
	LogicLayerConfigObj_GameEditor_t();
	virtual ~LogicLayerConfigObj_GameEditor_t();

	virtual void Configure( LogicLayer_t *pLogicLayer );
};

#endif // !defined(AFX_LOGICLAYERCONFIGOBJ_GAMEEDITOR_T_H__2D0EB6D9_1D50_4984_A0F4_D382D2B8D177__INCLUDED_)
