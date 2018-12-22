/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// TaskConfigObj_Game_t.h: interface for the TaskConfigObj_Game_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKCONFIGOBJ_GAME_T_H__4F59876D_A1F0_4093_8BC1_3C8B54D72CBB__INCLUDED_)
#define AFX_TASKCONFIGOBJ_GAME_T_H__4F59876D_A1F0_4093_8BC1_3C8B54D72CBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/TaskConfig_t.h"

class TaskConfigObj_Game_t : public TaskConfig_t  
{
public:
	TaskConfigObj_Game_t();
	virtual ~TaskConfigObj_Game_t();

	virtual void Configure( Task_t *pTask, Systems_t *pSystems );
};

#endif // !defined(AFX_TASKCONFIGOBJ_GAME_T_H__4F59876D_A1F0_4093_8BC1_3C8B54D72CBB__INCLUDED_)
