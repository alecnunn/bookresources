// ProcessInfoWin32Impl_t.h: interface for the ProcessInfoWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSINFOWIN32IMPL_T_H__7A3E78D7_0C5B_4AAF_B27D_17915CAC5279__INCLUDED_)
#define AFX_PROCESSINFOWIN32IMPL_T_H__7A3E78D7_0C5B_4AAF_B27D_17915CAC5279__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Systems/ProcessInfo_t.h"

class ProcessInfoWin32Impl_t : public ProcessInfo_t  
{
public:
	ProcessInfoWin32Impl_t();
	virtual ~ProcessInfoWin32Impl_t();

	HINSTANCE m_hInst;
	HINSTANCE m_hPrevInst;
	LPSTR     m_lpCmdLine;
	int	      m_nCmdShow;
};

#endif // !defined(AFX_PROCESSINFOWIN32IMPL_T_H__7A3E78D7_0C5B_4AAF_B27D_17915CAC5279__INCLUDED_)
