// TaskObj_Intro_t.h: interface for the TaskObj_Intro_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKOBJ_INTRO_T_H__C1F5DEB7_66F7_4085_BDDE_490E3A93C7ED__INCLUDED_)
#define AFX_TASKOBJ_INTRO_T_H__C1F5DEB7_66F7_4085_BDDE_490E3A93C7ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/FramePlayer_t.h"

class TaskObj_Intro_t : public FramePlayer_t  
{
public:
	TaskObj_Intro_t();
	virtual ~TaskObj_Intro_t();

	virtual bool Connect( Systems_t *pSystems );
	virtual void Disconnect();

	virtual void Run();
};

#endif // !defined(AFX_TASKOBJ_INTRO_T_H__C1F5DEB7_66F7_4085_BDDE_490E3A93C7ED__INCLUDED_)
