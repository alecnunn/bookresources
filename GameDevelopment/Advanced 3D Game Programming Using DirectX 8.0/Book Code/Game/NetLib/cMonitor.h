// cMonitor.h: interface for the cMonitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMONITOR_H__4A115C4A_D1D4_11D3_AE4F_00E029031C67__INCLUDED_)
#define AFX_CMONITOR_H__4A115C4A_D1D4_11D3_AE4F_00E029031C67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>


class cMonitor
{
  HANDLE d_mutex;

public:
	cMonitor();
	virtual ~cMonitor();

  void MutexOn() const;
  void MutexOff() const;
};

#endif // !defined(AFX_CMONITOR_H__4A115C4A_D1D4_11D3_AE4F_00E029031C67__INCLUDED_)
