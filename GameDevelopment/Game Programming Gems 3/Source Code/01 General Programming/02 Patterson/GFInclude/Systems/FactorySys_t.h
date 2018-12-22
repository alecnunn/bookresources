/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// FactorySys_t.h: interface for the FactorySys_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACTORYSYS_T_H__0C2F18CA_1E30_48F7_B8F1_AEE81AC2337E__INCLUDED_)
#define AFX_FACTORYSYS_T_H__0C2F18CA_1E30_48F7_B8F1_AEE81AC2337E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum {
	TYPE_ID_INVALID,
	TYPE_ID_AVLAYER,
	TYPE_ID_LOGICLAYER,
	TYPE_ID_TASK,
	TYPE_ID_COUNT
};

enum {
	DRIVER_ID_VISUAL_SYS_D3D7,
	DRIVER_ID_VISUAL_SYS_D3D8,
	DRIVER_ID_VISUAL_SYS_OPENGL,
};

class Systems_t;
class GameFactory_t;
class AVLayer_t;
class LogicLayer_t;
class Task_t;
class ResourceSys_t;
class TaskSys_t;
class WindowSys_t;
class FrameSys_t;
class InputSys_t;
class VisualSys_t;
class AudioSys_t;
class NetworkSys_t;

class FactorySys_t  
{
public:
	FactorySys_t() {};
	virtual ~FactorySys_t() {};

	virtual bool Init( Systems_t *pSystems ) = 0;
	virtual void Shutdown() = 0;

	virtual void RegisterGameFactory( GameFactory_t *pGameFactory ) = 0;
	virtual void SetVisualSysDriverID( int nVisualSysDriverID ) = 0;
	virtual int GetVisualSysDriverID() = 0;

	// factory ID objects
	virtual AVLayer_t * CreateAVLayer( int nFactoryID ) = 0;
	virtual void ConfigAVLayer( AVLayer_t *pAVLayer, int nConfigID ) = 0;
	virtual void DeleteAVLayer( AVLayer_t *pAVLayer ) = 0;

	virtual LogicLayer_t * CreateLogicLayer( int nFactoryID ) = 0;
	virtual void ConfigLogicLayer( LogicLayer_t *pLogicLayer, int nConfigID ) = 0;
	virtual void DeleteLogicLayer( LogicLayer_t *pLogicLayer ) = 0;

	virtual Task_t * CreateTask( int nFactoryID ) = 0;
	virtual void ConfigTask( Task_t *pTask, int nConfigID ) = 0;
	virtual void DeleteTask( Task_t *pTask ) = 0;

	// system objects
	virtual ResourceSys_t * CreateResourceSys() = 0;
	virtual void DeleteResourceSys( ResourceSys_t *pResourceSys ) = 0;
	virtual TaskSys_t * CreateTaskSys() = 0;
	virtual void DeleteTaskSys( TaskSys_t *pTaskSys ) = 0;
	virtual WindowSys_t * CreateWindowSys() = 0;
	virtual void DeleteWindowSys( WindowSys_t *pWindowSys ) = 0;
	virtual FrameSys_t * CreateFrameSys() = 0;
	virtual void DeleteFrameSys( FrameSys_t *pFrameSys ) = 0;
	virtual InputSys_t * CreateInputSys() = 0;
	virtual void DeleteInputSys( InputSys_t *pInputSys ) = 0;
	virtual VisualSys_t * CreateVisualSys() = 0;
	virtual void DeleteVisualSys( VisualSys_t *pVisualSys ) = 0;
	virtual AudioSys_t * CreateAudioSys() = 0;
	virtual void DeleteAudioSys( AudioSys_t *pAudioSys ) = 0;
	virtual NetworkSys_t * CreateNetworkSys() = 0;
	virtual void DeleteNetworkSys( NetworkSys_t *pNetworkSys ) = 0;
};

#endif // !defined(AFX_FACTORYSYS_T_H__0C2F18CA_1E30_48F7_B8F1_AEE81AC2337E__INCLUDED_)
