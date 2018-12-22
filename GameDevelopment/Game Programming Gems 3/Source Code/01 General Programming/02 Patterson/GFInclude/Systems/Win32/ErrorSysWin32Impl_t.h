/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// ErrorSysWin32Impl_t.h: interface for the ErrorSysWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERRORSYSWIN32IMPL_T_H__5F0F35B1_7A13_4AA6_85A3_10054ED64BC9__INCLUDED_)
#define AFX_ERRORSYSWIN32IMPL_T_H__5F0F35B1_7A13_4AA6_85A3_10054ED64BC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Systems/ErrorSys_t.h"

class ErrorSysWin32Impl_t : public ErrorSys_t  
{
public:
	ErrorSysWin32Impl_t();
	virtual ~ErrorSysWin32Impl_t();

	virtual bool Init( Systems_t *pSystems );
	virtual void Shutdown();

	virtual void Halt(char* pFormat, ...);
protected:
	Systems_t *m_pSystems;
};

#endif // !defined(AFX_ERRORSYSWIN32IMPL_T_H__5F0F35B1_7A13_4AA6_85A3_10054ED64BC9__INCLUDED_)
