/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayerConfigObj_Intro_t.h: interface for the LogicLayerConfigObj_Intro_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICLAYERCONFIGOBJ_INTRO_T_H__04579CD5_269B_44E6_B257_80172C10DA66__INCLUDED_)
#define AFX_LOGICLAYERCONFIGOBJ_INTRO_T_H__04579CD5_269B_44E6_B257_80172C10DA66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/LogicLayerConfig_t.h"

class LogicLayerConfigObj_Intro_t : public LogicLayerConfig_t  
{
public:
	LogicLayerConfigObj_Intro_t();
	virtual ~LogicLayerConfigObj_Intro_t();

	virtual void Configure( LogicLayer_t *pLogicLayer );
};

#endif // !defined(AFX_LOGICLAYERCONFIGOBJ_INTRO_T_H__04579CD5_269B_44E6_B257_80172C10DA66__INCLUDED_)
