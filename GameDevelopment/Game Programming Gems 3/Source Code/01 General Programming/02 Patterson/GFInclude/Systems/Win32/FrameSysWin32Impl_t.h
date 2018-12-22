// FrameSysWin32Impl_t.h: interface for the FrameSysWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRAMESYSWIN32IMPL_T_H__F155C51A_8351_4EFF_BADA_3DDE1924E2EB__INCLUDED_)
#define AFX_FRAMESYSWIN32IMPL_T_H__F155C51A_8351_4EFF_BADA_3DDE1924E2EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Systems_t;

#include "GFInclude/Systems/FrameSys_t.h"

class FrameSysWin32Impl_t : public FrameSys_t  
{
public:
	FrameSysWin32Impl_t();
	virtual ~FrameSysWin32Impl_t();

	virtual bool Init( Systems_t *pSystems );
	virtual void Shutdown();

	virtual bool StartFrameSync();
	virtual void EndFrameSync();
	virtual bool StartAsync();
	virtual void EndAsync();

protected:
	Systems_t *m_pSystems;
};

#endif // !defined(AFX_FRAMESYSWIN32IMPL_T_H__F155C51A_8351_4EFF_BADA_3DDE1924E2EB__INCLUDED_)
