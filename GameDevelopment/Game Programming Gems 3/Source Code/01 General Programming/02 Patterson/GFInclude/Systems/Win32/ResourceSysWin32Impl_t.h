/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// ResourceSysWin32Impl_t.h: interface for the ResourceSysWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCESYSWIN32IMPL_T_H__8907FCCF_0096_452A_9358_78217324F6BB__INCLUDED_)
#define AFX_RESOURCESYSWIN32IMPL_T_H__8907FCCF_0096_452A_9358_78217324F6BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Systems/ResourceSys_t.h"
#include "ResourceObjCollection_t.h"

class ResourceSysWin32Impl_t : public ResourceSys_t  
{
public:
	ResourceSysWin32Impl_t();
	virtual ~ResourceSysWin32Impl_t();

	virtual bool Init( Systems_t *pSystems );
	virtual void Shutdown();

	virtual void SetCollectionData( int *pCollData );
	virtual void StartCollectionLoad( int nID );
	virtual void DumpCollection( int nID );
	virtual ResourceSys_t::CollectionStatus_t GetCollectionStatus( int nID );

	virtual AVLayer_t * GetAVLayer( int nID );
	virtual LogicLayer_t * GetLogicLayer( int nID );
	virtual Task_t * GetTask( int nID );

protected:
	Systems_t *m_pSystems;

	void CreateCollections();
	void LoadType( ResourceObjTypeInfo_t &TypeInfo );
	void MapAVLayer( AVLayer_t *pAVLayer, int nInstanceID );
	void LoadAVLayers( ResourceObjFactoryInfo_t &FactoryInfo );
	void MapLogicLayer( LogicLayer_t *pLogicLayer, int nInstanceID );
	void LoadLogicLayers( ResourceObjFactoryInfo_t &FactoryInfo );
	void MapTask( Task_t *pTask, int nInstanceID );
	void LoadTasks( ResourceObjFactoryInfo_t &FactoryInfo );
	
	// instance maps
	typedef std::map<int,AVLayer_t *> IntToAVLayerPtrMap_t;
	IntToAVLayerPtrMap_t m_IntToAVLayerPtrMap;

	typedef std::map<int,LogicLayer_t *> IntToLogicLayerPtrMap_t;
	IntToLogicLayerPtrMap_t m_IntToLogicLayerPtrMap;

	typedef std::map<int,Task_t *> IntToTaskPtrMap_t;
	IntToTaskPtrMap_t m_IntToTaskPtrMap;

	// collection definitions
	int *m_pCollData;
	ResourceObjCollectionVector_t m_CollectionVector;

};

#endif // !defined(AFX_RESOURCESYSWIN32IMPL_T_H__8907FCCF_0096_452A_9358_78217324F6BB__INCLUDED_)
