/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// TaskConfig_t.h: interface for the TaskConfig_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKCONFIG_T_H__0EDAB6B5_4AC1_4C4E_804A_B53FE9EF1821__INCLUDED_)
#define AFX_TASKCONFIG_T_H__0EDAB6B5_4AC1_4C4E_804A_B53FE9EF1821__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Task_t;
class Systems_t;

class TaskConfig_t  
{
public:
	TaskConfig_t() {};
	virtual ~TaskConfig_t() {};

	virtual void Configure( Task_t *pTask, Systems_t *pSystems ) = 0;
};

#endif // !defined(AFX_TASKCONFIG_T_H__0EDAB6B5_4AC1_4C4E_804A_B53FE9EF1821__INCLUDED_)
