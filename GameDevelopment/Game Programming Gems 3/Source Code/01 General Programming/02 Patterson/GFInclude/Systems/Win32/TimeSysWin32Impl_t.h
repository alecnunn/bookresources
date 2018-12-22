/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// TimeSysWin32Impl_t.h: interface for the TimeSysWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMESYSWIN32IMPL_T_H__623B7C09_670C_4437_A1FD_5F79759FD16C__INCLUDED_)
#define AFX_TIMESYSWIN32IMPL_T_H__623B7C09_670C_4437_A1FD_5F79759FD16C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Systems/TimeSys_t.h"

class TimeSysWin32Impl_t : public TimeSys_t  
{
public:
	TimeSysWin32Impl_t();
	virtual ~TimeSysWin32Impl_t();

	virtual bool Init( Systems_t *pSystems );
	virtual void Shutdown();

protected:
	Systems_t *m_pSystems;
};

#endif // !defined(AFX_TIMESYSWIN32IMPL_T_H__623B7C09_670C_4437_A1FD_5F79759FD16C__INCLUDED_)
