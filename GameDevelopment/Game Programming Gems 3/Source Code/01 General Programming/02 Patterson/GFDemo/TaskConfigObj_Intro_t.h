/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// TaskConfigObj_Intro_t.h: interface for the TaskConfigObj_Intro_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKCONFIGOBJ_INTRO_T_H__79CDB117_EACC_4239_A88C_FDE971D5BDE8__INCLUDED_)
#define AFX_TASKCONFIGOBJ_INTRO_T_H__79CDB117_EACC_4239_A88C_FDE971D5BDE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/TaskConfig_t.h"

class TaskConfigObj_Intro_t : public TaskConfig_t  
{
public:
	TaskConfigObj_Intro_t();
	virtual ~TaskConfigObj_Intro_t();

	virtual void Configure( Task_t *pTask, Systems_t *pSystems );
};

#endif // !defined(AFX_TASKCONFIGOBJ_INTRO_T_H__79CDB117_EACC_4239_A88C_FDE971D5BDE8__INCLUDED_)
