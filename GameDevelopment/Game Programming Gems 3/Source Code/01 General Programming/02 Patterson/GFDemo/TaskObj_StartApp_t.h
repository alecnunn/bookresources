/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// TaskObj_StartApp_t.h: interface for the TaskObj_StartApp_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKOBJ_STARTAPP_T_H__FD5A9C3E_565C_4B09_8E89_BF99B8B1C1F4__INCLUDED_)
#define AFX_TASKOBJ_STARTAPP_T_H__FD5A9C3E_565C_4B09_8E89_BF99B8B1C1F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/Task_t.h"

class TaskObj_StartApp_t : public Task_t
{
public:
	TaskObj_StartApp_t();
	virtual ~TaskObj_StartApp_t();

	virtual bool Connect( Systems_t *pSystems );
	virtual void Disconnect();

	virtual void Run();
protected:
	Systems_t *m_pSystems;
};

#endif // !defined(AFX_TASKOBJ_STARTAPP_T_H__FD5A9C3E_565C_4B09_8E89_BF99B8B1C1F4__INCLUDED_)
