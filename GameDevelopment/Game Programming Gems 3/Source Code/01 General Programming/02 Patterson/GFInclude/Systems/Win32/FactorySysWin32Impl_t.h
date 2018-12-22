/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// FactorySysWin32Impl_t.h: interface for the FactorySysWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACTORYSYSWIN32IMPL_T_H__EA74C60C_A078_4A82_AC3E_B68244240B4F__INCLUDED_)
#define AFX_FACTORYSYSWIN32IMPL_T_H__EA74C60C_A078_4A82_AC3E_B68244240B4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Systems/FactorySys_t.h"

class GameFactory_t;

class FactorySysWin32Impl_t : public FactorySys_t  
{
public:
	FactorySysWin32Impl_t();
	virtual ~FactorySysWin32Impl_t();

	virtual bool Init( Systems_t *pSystems );
	virtual void Shutdown();

	virtual void RegisterGameFactory( GameFactory_t *pGameFactory );
	virtual void SetVisualSysDriverID( int nVisualSysDriverID );
	virtual int GetVisualSysDriverID();

	virtual AVLayer_t * CreateAVLayer( int nFactoryID );
	virtual void ConfigAVLayer( AVLayer_t *pAVLayer, int nConfigID );
	virtual void DeleteAVLayer( AVLayer_t *pAVLayer );

	virtual LogicLayer_t * CreateLogicLayer( int nFactoryID );
	virtual void ConfigLogicLayer( LogicLayer_t *pLogicLayer, int nConfigID );
	virtual void DeleteLogicLayer( LogicLayer_t *pLogicLayer );

	virtual Task_t * CreateTask( int nFactoryID );
	virtual void ConfigTask( Task_t *pTask, int nConfigID );
	virtual void DeleteTask( Task_t *pTask );

	// system objects
	virtual ResourceSys_t * CreateResourceSys();
	virtual void DeleteResourceSys( ResourceSys_t *pResourceSys );
	virtual TaskSys_t * CreateTaskSys();
	virtual void DeleteTaskSys( TaskSys_t *pTaskSys );
	virtual WindowSys_t * CreateWindowSys();
	virtual void DeleteWindowSys( WindowSys_t *pWindowSys );
	virtual FrameSys_t * CreateFrameSys();
	virtual void DeleteFrameSys( FrameSys_t *pFrameSys );
	virtual InputSys_t * CreateInputSys();
	virtual void DeleteInputSys( InputSys_t *pInputSys );
	virtual VisualSys_t * CreateVisualSys();
	virtual void DeleteVisualSys( VisualSys_t *pVisualSys );
	virtual AudioSys_t * CreateAudioSys();
	virtual void DeleteAudioSys( AudioSys_t *pAudioSys );
	virtual NetworkSys_t * CreateNetworkSys();
	virtual void DeleteNetworkSys( NetworkSys_t *pNetworkSys );

protected:
	Systems_t *m_pSystems;

	GameFactory_t *m_pGameFactory;

	bool LoadVisualSysDll( const char *pDllName );
   	HMODULE m_hVisualSysDll;

	int m_nVisualSysDriverID;
};

#endif // !defined(AFX_FACTORYSYSWIN32IMPL_T_H__EA74C60C_A078_4A82_AC3E_B68244240B4F__INCLUDED_)
