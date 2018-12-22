/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogSysWin32Impl_t.h: interface for the LogSysWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGSYSWIN32IMPL_T_H__195107C4_F28B_44A9_B3B4_CEF2C3A22229__INCLUDED_)
#define AFX_LOGSYSWIN32IMPL_T_H__195107C4_F28B_44A9_B3B4_CEF2C3A22229__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Systems/LogSys_t.h"

class LogSysWin32Impl_t : public LogSys_t  
{
public:
	LogSysWin32Impl_t();
	virtual ~LogSysWin32Impl_t();

	virtual bool Init( Systems_t *pSystems );
	virtual void Shutdown();

	virtual void SetOutputMode(int nOutputMode);
	virtual int GetOutputMode();
	virtual void SetChannelMask(int nChannelMask);
	virtual int GetChannelMask();
	virtual void SetDetailLevel(int nDetailLevel);
	virtual int GetDetailLevel();

	virtual void WriteOutput(int nChannelMask, int nDetailLevel, char* pFormat, ...);
protected:
	Systems_t *m_pSystems;

	HANDLE m_hOutputWin;
	HANDLE m_hInputWin;

	int m_nOutputMode;
	int m_nChannelMask;
	int m_nDetailLevel;
};

#endif // !defined(AFX_LOGSYSWIN32IMPL_T_H__195107C4_F28B_44A9_B3B4_CEF2C3A22229__INCLUDED_)
