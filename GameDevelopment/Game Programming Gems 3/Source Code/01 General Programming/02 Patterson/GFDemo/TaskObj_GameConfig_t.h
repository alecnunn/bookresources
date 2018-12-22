// TaskObj_GameConfig_t.h: interface for the TaskObj_GameConfig_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKOBJ_GAMECONFIG_T_H__922B22E6_3C05_401D_9B87_3AB4326D8675__INCLUDED_)
#define AFX_TASKOBJ_GAMECONFIG_T_H__922B22E6_3C05_401D_9B87_3AB4326D8675__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/FramePlayer_t.h"

class TaskObj_GameConfig_t : public FramePlayer_t  
{
public:
	TaskObj_GameConfig_t();
	virtual ~TaskObj_GameConfig_t();

	virtual bool Connect( Systems_t *pSystems );
	virtual void Disconnect();

	virtual void Run();
};

#endif // !defined(AFX_TASKOBJ_GAMECONFIG_T_H__922B22E6_3C05_401D_9B87_3AB4326D8675__INCLUDED_)
