/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// ResourceSys_t.h: interface for the ResourceSys_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCESYS_T_H__5E225B8F_D18C_4A59_8713_E79DD38A89D3__INCLUDED_)
#define AFX_RESOURCESYS_T_H__5E225B8F_D18C_4A59_8713_E79DD38A89D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// collection data tags
enum {
	END_TAG,
	COLLECTION_TAG,
	COLLECTION_END_TAG,
	TYPE_TAG,
	TYPE_END_TAG,
	FACTORY_TAG,
	FACTORY_END_TAG,
	INSTANCE_TAG,
};

/*
	This is the general outline of collection data

	COLLECTION_TAG,
		TYPE_TAG, nTypeID,
			FACTORY_TAG, nFactoryID,
				INSTANCE_TAG, nInstanceID, nConfigID,
			FACTORY_END_TAG,
		TYPE_END_TAG,
	COLLECTION_END_TAG,
	END_TAG,
*/

class Systems_t;

class AVLayer_t;
class LogicLayer_t;
class Task_t;

class ResourceSys_t  
{
public:
	ResourceSys_t() {};
	virtual ~ResourceSys_t() {};

	virtual bool Init( Systems_t *pSystems ) = 0;
	virtual void Shutdown() = 0;

	virtual void SetCollectionData( int *pCollData ) = 0;
	virtual void StartCollectionLoad( int nID ) = 0;
	virtual void DumpCollection( int nID ) = 0;

	enum CollectionStatus_t {
		COLLECTION_STATUS_ERROR,
		COLLECTION_STATUS_LOADING,
		COLLECTION_STATUS_LOADED,
		COLLECTION_STATUS_UNLOADED,
		COLLECTION_STATUS_UNKNOWN,
	};

	virtual CollectionStatus_t GetCollectionStatus( int nID ) = 0;

	virtual AVLayer_t * GetAVLayer( int nID ) = 0;
	virtual LogicLayer_t * GetLogicLayer( int nID ) = 0;
	virtual Task_t * GetTask( int nID ) = 0;
};

#endif // !defined(AFX_RESOURCESYS_T_H__5E225B8F_D18C_4A59_8713_E79DD38A89D3__INCLUDED_)
