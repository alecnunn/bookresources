/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// Task_t.h: interface for the Task_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASK_T_H__A5228A90_AD99_417E_BCD4_5C372DDD94CD__INCLUDED_)
#define AFX_TASK_T_H__A5228A90_AD99_417E_BCD4_5C372DDD94CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Systems_t;

class Task_t  
{
public:
	Task_t() {};
	virtual ~Task_t() {};

	virtual bool Connect( Systems_t *pSystems ) = 0;
	virtual void Disconnect() = 0;

	virtual void Run() = 0;
};

typedef std::list<Task_t *> TaskPtrList_t;

#endif // !defined(AFX_TASK_T_H__A5228A90_AD99_417E_BCD4_5C372DDD94CD__INCLUDED_)
