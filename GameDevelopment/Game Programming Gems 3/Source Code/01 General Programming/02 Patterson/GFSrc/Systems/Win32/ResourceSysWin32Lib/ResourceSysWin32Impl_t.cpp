/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// ResourceSysWin32Impl_t.cpp: implementation of the ResourceSysWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GFInclude/Systems/Win32/ResourceSysWin32Impl_t.h"
//#include "TaskIDs.h"
#include "GFInclude/Systems/FactorySys_t.h"
//#include "FactoryIDs.h"
//#include "InstanceIDs.h"
#include "GFInclude/Systems/Systems_t.h"
#include "GFInclude/Framework/AVLayer_t.h"
#include "GFInclude/Framework/LogicLayer_t.h"
#include "GFInclude/Framework/Task_t.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ResourceSysWin32Impl_t::ResourceSysWin32Impl_t()
{
	m_pSystems = 0;
	m_pCollData = 0;
}

ResourceSysWin32Impl_t::~ResourceSysWin32Impl_t()
{

}

bool ResourceSysWin32Impl_t::Init( Systems_t *pSystems )
{
	m_pSystems = pSystems;
	return true;
}

void ResourceSysWin32Impl_t::Shutdown()
{
	IntToAVLayerPtrMap_t::iterator iAVLMap;
	for( iAVLMap = m_IntToAVLayerPtrMap.begin(); iAVLMap != m_IntToAVLayerPtrMap.end(); ++iAVLMap )
	{
		delete (*iAVLMap).second;
	}
	m_IntToAVLayerPtrMap.clear();

	IntToLogicLayerPtrMap_t::iterator iLLMap;
	for( iLLMap = m_IntToLogicLayerPtrMap.begin(); iLLMap != m_IntToLogicLayerPtrMap.end(); ++iLLMap )
	{
		delete (*iLLMap).second;
	}
	m_IntToLogicLayerPtrMap.clear();

	IntToTaskPtrMap_t::iterator iTaskMap;
	for( iTaskMap = m_IntToTaskPtrMap.begin(); iTaskMap != m_IntToTaskPtrMap.end(); ++iTaskMap )
	{
		delete (*iTaskMap).second;
	}
	m_IntToTaskPtrMap.clear();
}

void ResourceSysWin32Impl_t::CreateCollections()
{
	ResourceObjCollection_t Collection;
	ResourceObjTypeInfo_t TypeInfo;
	ResourceObjFactoryInfo_t FactoryInfo;
	ResourceObjInstanceInfo_t InstanceInfo;

	int nPos = 0;
	int bDone = false;
	while( !bDone )
	{
		switch( m_pCollData[nPos] )
		{
		case COLLECTION_TAG:
			nPos++;
			Collection.clear();
			break;
		case COLLECTION_END_TAG:
			nPos++;
			m_CollectionVector.push_back( Collection );
			break;
		case TYPE_TAG:
			nPos++;
			TypeInfo.clear();
			TypeInfo.m_nTypeID = m_pCollData[nPos++];
			break;
		case TYPE_END_TAG:
			nPos++;
			Collection.m_TypeInfoVector.push_back(TypeInfo);
			break;
		case FACTORY_TAG:
			nPos++;
			FactoryInfo.clear();
			FactoryInfo.m_nFactoryID = m_pCollData[nPos++];
			break;
		case FACTORY_END_TAG:
			nPos++;
			TypeInfo.m_FactoryInfoVector.push_back(FactoryInfo);
			break;
		case INSTANCE_TAG:
			nPos++;
			InstanceInfo.clear();
			InstanceInfo.m_nInstanceID = m_pCollData[nPos++];
			InstanceInfo.m_nConfigID = m_pCollData[nPos++];
			FactoryInfo.m_InstanceInfoVector.push_back(InstanceInfo);
			break;
		case END_TAG:
			bDone = true;
			break;
		default:
			HALTCALL((m_pSystems->GetErrorSys()->Halt("CollData parse error at %d\n", nPos));)
			break;
		}
	}
}

void ResourceSysWin32Impl_t::MapAVLayer( AVLayer_t *pAVLayer, int nInstanceID )
{
	if( m_IntToAVLayerPtrMap.find( nInstanceID ) != m_IntToAVLayerPtrMap.end() )
	{
		HALTCALL((m_pSystems->GetErrorSys()->Halt("Collision with ID %d in AVLayerPtrMap\n",nInstanceID));)
		return;
	}
	m_IntToAVLayerPtrMap[nInstanceID] = pAVLayer;
}

void ResourceSysWin32Impl_t::LoadAVLayers( ResourceObjFactoryInfo_t &FactoryInfo )
{
	int i;
	int nSize = FactoryInfo.m_InstanceInfoVector.size();

	for( i=0; i<nSize; ++i )
	{
		AVLayer_t *pAVLayer = m_pSystems->GetFactorySys()->CreateAVLayer( FactoryInfo.m_nFactoryID );
		m_pSystems->GetFactorySys()->ConfigAVLayer( pAVLayer, FactoryInfo.m_InstanceInfoVector[i].m_nConfigID );
		MapAVLayer( pAVLayer, FactoryInfo.m_InstanceInfoVector[i].m_nInstanceID );
	}
}

void ResourceSysWin32Impl_t::MapLogicLayer( LogicLayer_t *pLogicLayer, int nInstanceID )
{
	if( m_IntToLogicLayerPtrMap.find( nInstanceID ) != m_IntToLogicLayerPtrMap.end() )
	{
		HALTCALL((m_pSystems->GetErrorSys()->Halt("Collision with ID %d in LogicLayerPtrMap\n",nInstanceID));)
		return;
	}
	m_IntToLogicLayerPtrMap[nInstanceID] = pLogicLayer;
}

void ResourceSysWin32Impl_t::LoadLogicLayers( ResourceObjFactoryInfo_t &FactoryInfo )
{
	int i;
	int nSize = FactoryInfo.m_InstanceInfoVector.size();

	for( i=0; i<nSize; ++i )
	{
		LogicLayer_t *pLogicLayer = m_pSystems->GetFactorySys()->CreateLogicLayer( FactoryInfo.m_nFactoryID );
		m_pSystems->GetFactorySys()->ConfigLogicLayer( pLogicLayer, FactoryInfo.m_InstanceInfoVector[i].m_nConfigID );
		MapLogicLayer( pLogicLayer, FactoryInfo.m_InstanceInfoVector[i].m_nInstanceID );
	}
}

void ResourceSysWin32Impl_t::MapTask( Task_t *pTask, int nInstanceID )
{
	if( m_IntToTaskPtrMap.find( nInstanceID ) != m_IntToTaskPtrMap.end() )
	{
		HALTCALL((m_pSystems->GetErrorSys()->Halt("Collision with ID %d in TaskPtrMap\n",nInstanceID));)
		return;
	}
	m_IntToTaskPtrMap[nInstanceID] = pTask;
}

void ResourceSysWin32Impl_t::LoadTasks( ResourceObjFactoryInfo_t &FactoryInfo )
{
	int i;
	int nSize = FactoryInfo.m_InstanceInfoVector.size();

	for( i=0; i<nSize; ++i )
	{
		Task_t *pTask = m_pSystems->GetFactorySys()->CreateTask( FactoryInfo.m_nFactoryID );
		m_pSystems->GetFactorySys()->ConfigTask( pTask, FactoryInfo.m_InstanceInfoVector[i].m_nConfigID );
		MapTask( pTask, FactoryInfo.m_InstanceInfoVector[i].m_nInstanceID );
	}
}

void ResourceSysWin32Impl_t::LoadType( ResourceObjTypeInfo_t &TypeInfo )
{
	int i;
	int nSize = TypeInfo.m_FactoryInfoVector.size();

	switch( TypeInfo.m_nTypeID )
	{
	case TYPE_ID_AVLAYER:
		for( i=0; i<nSize; ++i )
		{
			LoadAVLayers( TypeInfo.m_FactoryInfoVector[i] );
		}
		break;
	case TYPE_ID_LOGICLAYER:
		for( i=0; i<nSize; ++i )
		{
			LoadLogicLayers( TypeInfo.m_FactoryInfoVector[i] );
		}
		break;
	case TYPE_ID_TASK:
		for( i=0; i<nSize; ++i )
		{
			LoadTasks( TypeInfo.m_FactoryInfoVector[i] );
		}
		break;
	default:
		break;
	}
}

void ResourceSysWin32Impl_t::SetCollectionData( int *pCollData )
{
	m_pCollData = pCollData;
	CreateCollections();
}

void ResourceSysWin32Impl_t::StartCollectionLoad( int nID )
{
	int nSize = m_CollectionVector[nID].m_TypeInfoVector.size();
	for( int i=0; i<nSize; ++i )
	{
		LoadType( m_CollectionVector[nID].m_TypeInfoVector[i] );
	}
}

void ResourceSysWin32Impl_t::DumpCollection( int nID )
{
}

ResourceSys_t::CollectionStatus_t ResourceSysWin32Impl_t::GetCollectionStatus( int nID )
{
	return ResourceSys_t::COLLECTION_STATUS_LOADED;
}

AVLayer_t * ResourceSysWin32Impl_t::GetAVLayer( int nID )
{
	IntToAVLayerPtrMap_t::iterator iMap;
	if( (iMap = m_IntToAVLayerPtrMap.find(nID)) != m_IntToAVLayerPtrMap.end() )
	{
		return (*iMap).second;
	}
	HALTCALL((m_pSystems->GetErrorSys()->Halt("AVLayer %d Not Found\n", nID));)
	return 0;
}

LogicLayer_t * ResourceSysWin32Impl_t::GetLogicLayer( int nID )
{
	IntToLogicLayerPtrMap_t::iterator iMap;
	if( (iMap = m_IntToLogicLayerPtrMap.find(nID)) != m_IntToLogicLayerPtrMap.end() )
	{
		return (*iMap).second;
	}
	HALTCALL((m_pSystems->GetErrorSys()->Halt("LogicLayer %d Not Found\n", nID));)
	return 0;
}

Task_t * ResourceSysWin32Impl_t::GetTask( int nID )
{
	IntToTaskPtrMap_t::iterator iMap;
	if( (iMap = m_IntToTaskPtrMap.find(nID)) != m_IntToTaskPtrMap.end() )
	{
		return (*iMap).second;
	}
	HALTCALL((m_pSystems->GetErrorSys()->Halt("Task %d Not Found\n", nID));)
	return 0;
}

