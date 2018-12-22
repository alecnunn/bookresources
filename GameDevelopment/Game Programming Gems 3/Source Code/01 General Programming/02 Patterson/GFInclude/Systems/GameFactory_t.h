/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// GameFactory_t.h: interface for the GameFactory_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEFACTORY_T_H__3594DEC4_1819_4D94_AAFC_32A760EE341E__INCLUDED_)
#define AFX_GAMEFACTORY_T_H__3594DEC4_1819_4D94_AAFC_32A760EE341E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AVLayer_t;
class LogicLayer_t;
class Task_t;
class Systems_t;

class GameFactory_t  
{
public:
	GameFactory_t() {}
	virtual ~GameFactory_t() {}

	virtual int * GetCollectionData() = 0;

	virtual AVLayer_t * CreateAVLayer( int nFactoryID ) = 0;
	virtual void ConfigAVLayer( AVLayer_t *pAVLayer, int nConfigID ) = 0;
	virtual void DeleteAVLayer( AVLayer_t *pAVLayer ) = 0;

	virtual LogicLayer_t * CreateLogicLayer( int nFactoryID ) = 0;
	virtual void ConfigLogicLayer( LogicLayer_t *pLogicLayer, int nConfigID ) = 0;
	virtual void DeleteLogicLayer( LogicLayer_t *pLogicLayer ) = 0;

	virtual Task_t * CreateTask( int nFactoryID ) = 0;
	virtual void ConfigTask( Task_t *pTask, int nConfigID, Systems_t *pSystems ) = 0;
	virtual void DeleteTask( Task_t *pTask ) = 0;
};

#endif // !defined(AFX_GAMEFACTORY_T_H__3594DEC4_1819_4D94_AAFC_32A760EE341E__INCLUDED_)
