/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// WindowSys_t.h: interface for the WindowSys_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINDOWSYS_T_H__A433B44B_A821_4D32_BB31_E53F2AA251E5__INCLUDED_)
#define AFX_WINDOWSYS_T_H__A433B44B_A821_4D32_BB31_E53F2AA251E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ProcessInfo_t;
class Systems_t;

class WindowSys_t  
{
public:
	WindowSys_t() {};
	virtual ~WindowSys_t() {};

	virtual void SetProcessInfo( ProcessInfo_t *pProcessInfo ) = 0;
	virtual void SetWindowInfo( int nWidth, int nHeight ) = 0;
	virtual void GetWindowInfo( int &nWidth, int &nHeight ) = 0;
	virtual bool Init( Systems_t *pSystems ) = 0;
	virtual void Shutdown() = 0;

	virtual void Process() = 0;
};

#endif // !defined(AFX_WINDOWSYS_T_H__A433B44B_A821_4D32_BB31_E53F2AA251E5__INCLUDED_)
