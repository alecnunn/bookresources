/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// ErrorSys_t.h: interface for the ErrorSys_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERRORSYS_T_H__A2E9555C_042A_4AF6_AAE6_978CAEC915B6__INCLUDED_)
#define AFX_ERRORSYS_T_H__A2E9555C_042A_4AF6_AAE6_978CAEC915B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Systems_t;

class ErrorSys_t  
{
public:
	ErrorSys_t() {};
	virtual ~ErrorSys_t() {};

	virtual bool Init( Systems_t *pSystems ) = 0;
	virtual void Shutdown() = 0;

	virtual void Halt(char* pFormat, ...) = 0;
};

#endif // !defined(AFX_ERRORSYS_T_H__A2E9555C_042A_4AF6_AAE6_978CAEC915B6__INCLUDED_)
