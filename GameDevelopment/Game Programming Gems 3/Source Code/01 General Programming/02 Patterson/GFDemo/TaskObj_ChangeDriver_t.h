/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// TaskObj_ChangeDriver_t.h: interface for the TaskObj_ChangeDriver_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKOBJ_CHANGEDRIVER_T_H__D953BE1C_1D74_4A5F_ADC3_41192E362F81__INCLUDED_)
#define AFX_TASKOBJ_CHANGEDRIVER_T_H__D953BE1C_1D74_4A5F_ADC3_41192E362F81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/Task_t.h"

class TaskObj_ChangeDriver_t : public Task_t  
{
public:
	TaskObj_ChangeDriver_t();
	virtual ~TaskObj_ChangeDriver_t();

	virtual bool Connect( Systems_t *pSystems );
	virtual void Disconnect();

	virtual void Run();

	void SetVisualSysDriverID( int nVisualSysDriverID );
protected:
	Systems_t *m_pSystems;
	int m_nVisualSysDriverID;
};

#endif // !defined(AFX_TASKOBJ_CHANGEDRIVER_T_H__D953BE1C_1D74_4A5F_ADC3_41192E362F81__INCLUDED_)
