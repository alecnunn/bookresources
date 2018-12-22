/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// TaskCommand_t.h: interface for the TaskCommand_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKCOMMAND_T_H__6886B654_6F5E_4662_B954_B6D083FD5A54__INCLUDED_)
#define AFX_TASKCOMMAND_T_H__6886B654_6F5E_4662_B954_B6D083FD5A54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Task_t;

class TaskCommand_t  
{
public:
	enum TaskCommandID_t {
		INVALID,
		FRAMESYNC_PUSH_BACK,
		FRAMESYNC_PUSH_FRONT,
		FRAMESYNC_REMOVE,
		FRAMESYNC_REMOVE_ALL,
		ASYNC_PUSH_BACK,
		ASYNC_PUSH_FRONT,
		ASYNC_REMOVE,
		ASYNC_REMOVE_ALL,
	};

	TaskCommand_t( TaskCommandID_t nID, Task_t *pTask )
	{
		m_nID = nID;
		m_pTask = pTask;
	}
	virtual ~TaskCommand_t() {}

	TaskCommandID_t m_nID;
	Task_t *m_pTask;
};

typedef std::vector<TaskCommand_t> TaskCommandVector_t;

#endif // !defined(AFX_TASKCOMMAND_T_H__6886B654_6F5E_4662_B954_B6D083FD5A54__INCLUDED_)
