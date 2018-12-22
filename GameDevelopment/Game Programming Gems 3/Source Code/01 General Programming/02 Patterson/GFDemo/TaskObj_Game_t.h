// TaskObj_Game_t.h: interface for the TaskObj_Game_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKOBJ_GAME_T_H__77D9A446_8FE6_4C8F_918E_F21A2C0532AF__INCLUDED_)
#define AFX_TASKOBJ_GAME_T_H__77D9A446_8FE6_4C8F_918E_F21A2C0532AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/FramePlayer_t.h"

class TaskObj_Game_t : public FramePlayer_t  
{
public:
	TaskObj_Game_t();
	virtual ~TaskObj_Game_t();

	virtual bool Connect( Systems_t *pSystems );
	virtual void Disconnect();

	virtual void Run();
};

#endif // !defined(AFX_TASKOBJ_GAME_T_H__77D9A446_8FE6_4C8F_918E_F21A2C0532AF__INCLUDED_)
