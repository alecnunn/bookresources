/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// TaskSysWin32Impl_t1.h: interface for the TaskSysWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKSYSWIN32IMPL_T1_H__1E02E4D4_D6F5_4621_90F1_20BE1F058847__INCLUDED_)
#define AFX_TASKSYSWIN32IMPL_T1_H__1E02E4D4_D6F5_4621_90F1_20BE1F058847__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Systems/TaskSys_t.h"
#include "GFInclude/Framework/Task_t.h"

class TaskSysWin32Impl_t : public TaskSys_t  
{
public:
	TaskSysWin32Impl_t();
	virtual ~TaskSysWin32Impl_t();

	virtual bool Init( Systems_t *pSystems );
	virtual void Shutdown();

	virtual void Post_TaskCommand( TaskCommand_t::TaskCommandID_t nID, Task_t *pTask );
	virtual bool Process_TaskCommands();

	virtual void Run();

protected:
	Systems_t *m_pSystems;

	// holds the commands to our task system that are executed at the
	// top of the next run loop
	TaskCommandVector_t m_TaskCommandVector;
	// holds the active tasks to be run at frame sync
	TaskPtrList_t m_FrameSyncTaskPtrList;
	// holds the active tasks to be run async
	TaskPtrList_t m_AsyncTaskPtrList;
};

#endif // !defined(AFX_TASKSYSWIN32IMPL_T1_H__1E02E4D4_D6F5_4621_90F1_20BE1F058847__INCLUDED_)
