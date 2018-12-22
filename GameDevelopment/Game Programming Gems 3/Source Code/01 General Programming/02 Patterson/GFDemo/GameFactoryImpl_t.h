/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// GameFactoryImpl_t.h: interface for the GameFactoryImpl_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEFACTORYIMPL_T_H__B63D92FE_9DF6_4BEA_8032_3DC5B83C478A__INCLUDED_)
#define AFX_GAMEFACTORYIMPL_T_H__B63D92FE_9DF6_4BEA_8032_3DC5B83C478A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Systems/GameFactory_t.h"

class GameFactoryImpl_t : public GameFactory_t
{
public:
	GameFactoryImpl_t();
	virtual ~GameFactoryImpl_t();

	virtual int * GetCollectionData();

	virtual AVLayer_t * CreateAVLayer( int nFactoryID );
	virtual void ConfigAVLayer( AVLayer_t *pAVLayer, int nConfigID );
	virtual void DeleteAVLayer( AVLayer_t *pAVLayer );

	virtual LogicLayer_t * CreateLogicLayer( int nFactoryID );
	virtual void ConfigLogicLayer( LogicLayer_t *pLogicLayer, int nConfigID );
	virtual void DeleteLogicLayer( LogicLayer_t *pLogicLayer );

	virtual Task_t * CreateTask( int nFactoryID );
	virtual void ConfigTask( Task_t *pTask, int nConfigID, Systems_t *pSystems );
	virtual void DeleteTask( Task_t *pTask );
};

#endif // !defined(AFX_GAMEFACTORYIMPL_T_H__B63D92FE_9DF6_4BEA_8032_3DC5B83C478A__INCLUDED_)
