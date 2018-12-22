/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// TaskConfigObj_GameConfig_t.h: interface for the TaskConfigObj_GameConfig_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKCONFIGOBJ_GAMECONFIG_T_H__88DE31BF_0E61_4A55_A0A2_E0CA063D0FB5__INCLUDED_)
#define AFX_TASKCONFIGOBJ_GAMECONFIG_T_H__88DE31BF_0E61_4A55_A0A2_E0CA063D0FB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/TaskConfig_t.h"

class TaskConfigObj_GameConfig_t : public TaskConfig_t  
{
public:
	TaskConfigObj_GameConfig_t();
	virtual ~TaskConfigObj_GameConfig_t();

	virtual void Configure( Task_t *pTask, Systems_t *pSystems );
};

#endif // !defined(AFX_TASKCONFIGOBJ_GAMECONFIG_T_H__88DE31BF_0E61_4A55_A0A2_E0CA063D0FB5__INCLUDED_)
