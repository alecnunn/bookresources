/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayerConfigObj_GameWin_t.h: interface for the LogicLayerConfigObj_GameWin_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICLAYERCONFIGOBJ_GAMEWIN_T_H__A3870752_DCA7_4F8F_9519_941DB56C9BEC__INCLUDED_)
#define AFX_LOGICLAYERCONFIGOBJ_GAMEWIN_T_H__A3870752_DCA7_4F8F_9519_941DB56C9BEC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/LogicLayerConfig_t.h"

class LogicLayerConfigObj_GameWin_t : public LogicLayerConfig_t  
{
public:
	LogicLayerConfigObj_GameWin_t();
	virtual ~LogicLayerConfigObj_GameWin_t();

	virtual void Configure( LogicLayer_t *pLogicLayer );
};

#endif // !defined(AFX_LOGICLAYERCONFIGOBJ_GAMEWIN_T_H__A3870752_DCA7_4F8F_9519_941DB56C9BEC__INCLUDED_)
