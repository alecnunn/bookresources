// cThread.h: interface for the cThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CTHREAD_H__4A115C4B_D1D4_11D3_AE4F_00E029031C67__INCLUDED_)
#define AFX_CTHREAD_H__4A115C4B_D1D4_11D3_AE4F_00E029031C67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#include "cMonitor.h"


class cThread : public cMonitor 
{
private:
  HANDLE  d_threadHandle;
	DWORD   d_threadID;
  bool    d_bIsRunning;

protected:
  char d_errorBuffer[ 1000 ];

public:
	cThread();
	virtual ~cThread();

  void Begin();
  void End();
  bool IsRunning();

  virtual DWORD ThreadProc();


  void        SetError( const char *pBuffer );
  const char  *Error();
};

#endif // !defined(AFX_CTHREAD_H__4A115C4B_D1D4_11D3_AE4F_00E029031C67__INCLUDED_)
