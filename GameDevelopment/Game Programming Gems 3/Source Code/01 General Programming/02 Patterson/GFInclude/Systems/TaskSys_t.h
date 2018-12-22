/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// TaskSys_t.h: interface for the TaskSys_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKSYS_T_H__ED90DAF8_4940_4694_89D7_3CC9FDE89972__INCLUDED_)
#define AFX_TASKSYS_T_H__ED90DAF8_4940_4694_89D7_3CC9FDE89972__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Systems_t;
class Task_t;
#include "GFInclude/Framework/TaskCommand_t.h"

/*!
	The TaskSys_t maintains the task system state information and provides
	task control operations.
*/

class TaskSys_t  
{
public:
	TaskSys_t() {};
	virtual ~TaskSys_t() {};

	virtual bool Init( Systems_t *pSystems ) = 0;
	virtual void Shutdown() = 0;

	virtual void Post_TaskCommand( TaskCommand_t::TaskCommandID_t nID, Task_t *pTask ) = 0;
	virtual bool Process_TaskCommands() = 0;

	virtual void Run() = 0;
};

#endif // !defined(AFX_TASKSYS_T_H__ED90DAF8_4940_4694_89D7_3CC9FDE89972__INCLUDED_)
