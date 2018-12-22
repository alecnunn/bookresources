/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// GameApp_t.h: interface for the GameApp_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEAPP_T_H__884ED06E_5304_410D_915F_38E4F521E939__INCLUDED_)
#define AFX_GAMEAPP_T_H__884ED06E_5304_410D_915F_38E4F521E939__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ProcessInfo_t;
class GameFactory_t;
class Systems_t;

class GameApp_t  
{
public:
	GameApp_t();
	virtual ~GameApp_t();

	void SetProcessInfo( ProcessInfo_t *pProcessInfo );
	void Run();
protected:
	bool Init();
	void Shutdown();

	ProcessInfo_t *m_pProcessInfo;
	GameFactory_t *m_pGameFactory;
	Systems_t *m_pSystems;
};

#endif // !defined(AFX_GAMEAPP_T_H__884ED06E_5304_410D_915F_38E4F521E939__INCLUDED_)
