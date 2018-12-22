/* Copyright (C) James Boer, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) James Boer, 2000"
 */
//=============================================================================
//
// 	ResourceManager.cpp - memory management resource manager
//
// 	Author:	 James Boer 
//
//=============================================================================

#include "ResourceManager.h"
#include <queue>

using namespace std;

// BaseResource implementation

void BaseResource::Clear()
{
	m_Priority = RES_MED_PRIORITY;
	m_nRefCount = 0;
	m_LastAccess = 0;
}


bool BaseResource::operator < (BaseResource& container)
{
	if(GetPriority() < container.GetPriority())
		return true;
	else if(GetPriority() > container.GetPriority())
		return false;
	else
	{
		if(m_LastAccess < container.GetLastAccess())
			return true;
		else if(m_LastAccess > container.GetLastAccess())
			return false;
		else
		{
			if(GetSize() < container.GetSize())
				return true;
			else
				return false;
		}
	}
	return false;
}


void ResManager::Clear()
{
	m_ResourceMap.clear();
	m_rhNextResHandle = INVALID_RHANDLE;
	m_nCurrentUsedMemory = 0;
	m_nMaximumMemory = 0;
	m_bResourceReserved = false;
	m_CurrentResource = m_ResourceMap.end();
}


bool ResManager::Create(UINT nMaxSize)
{
	Clear();
	SetMaximumMemory(nMaxSize);
	return true;
}


void ResManager::Destroy()
{
	for(ResMapItor itor = m_ResourceMap.begin(); itor != m_ResourceMap.end(); ++itor)
	{
		if(!((*itor).second)->IsLocked())
		{
			delete ((*itor).second);
		}
	}
	m_ResourceMap.clear();
	Clear();
}


bool ResManager::SetMaximumMemory(size_t nMem)
{
	m_nMaximumMemory = nMem;
	return CheckForOverallocation();
}


bool ResManager::ReserveMemory(size_t nMem)
{
	AddMemory(nMem);
	if(!CheckForOverallocation())
		return false;
	m_bResourceReserved = true;
	return true;
}


// object access
bool ResManager::InsertResource(RHANDLE* rhUniqueID, BaseResource* pResource)
{
	// Get the next unique ID for this catalog
	*rhUniqueID = GetNextResHandle();
	// Insert the resource into the current catalog's map
	m_ResourceMap.insert(ResMapPair(*rhUniqueID, pResource));
	// Get the memory and add it to the catalog total.  Note that we only have
	// to check for memory overallocation if we haven't preallocated memory
	if(!m_bResourceReserved)
	{
		AddMemory(pResource->GetSize());
		// check to see if any overallocation has taken place
		if(!CheckForOverallocation())
			return false;
	}
	else
		m_bResourceReserved = false;
	// return success
	return true;
}


// object access
bool ResManager::InsertResource(RHANDLE rhUniqueID, BaseResource* pResource)
{
	ResMapItor itor = m_ResourceMap.find(rhUniqueID);
	if(itor != m_ResourceMap.end())
		// ID has already been allocated as a resource
		return false;
	// Insert the resource into the current catalog's map
	m_ResourceMap.insert(ResMapPair(rhUniqueID, pResource));
	// Get the memory and add it to the catalog total.  Note that we only have
	// to check for memory overallocation if we haven't preallocated memory
	if(!m_bResourceReserved)
	{
		AddMemory(pResource->GetSize());
		// check to see if any overallocation has taken place
		if(!CheckForOverallocation())
			return false;
	}
	else
		m_bResourceReserved = false;

	// return the id to the user for their use and return success
	return true;
}


bool ResManager::RemoveResource(RHANDLE rhUniqueID)
{
	// try to find the resource with the specified id
	ResMapItor itor = m_ResourceMap.find(rhUniqueID);
	if(itor == m_ResourceMap.end())
		// Could not find resource to remove
		return false;
	// if the resource was found, check to see that it's not locked
	if(((*itor).second)->IsLocked())
		// Can't remove a locked resource
		return false;
	// Get the memory and subtract it from the manager total
	RemoveMemory(((*itor).second)->GetSize());
	// remove the requested resource
	m_ResourceMap.erase(itor);

	return true;
}


bool ResManager::RemoveResource(BaseResource* pResource)
{

	// try to find the resource with the specified resource
        ResMapItor itor;
	for(itor = m_ResourceMap.begin(); itor != m_ResourceMap.end(); ++itor)
	{
		if(itor->second == pResource)
			break;
	}
	if(itor == m_ResourceMap.end())
		// Could not find resource to remove. 
		return false;
	// if the resource was found, check to see that it's not locked
	if(itor->second->IsLocked())
		// Can't remove a locked resource
		return false;
	// Get the memory and subtract it from the manager total
	RemoveMemory(pResource->GetSize());
	// remove the requested resource
	m_ResourceMap.erase(itor);

	return true;
}



// Destroys an object
bool ResManager::DestroyResource(BaseResource* pResource)
{
	if(!RemoveResource(pResource))
		return false;
	delete pResource;
	return true;
}

bool ResManager::DestroyResource(RHANDLE rhUniqueID)
{
	BaseResource* pResource = GetResource(rhUniqueID);
	if(!RemoveResource(rhUniqueID))
		return false;
	delete pResource;
	return true;
}



BaseResource* ResManager::GetResource(RHANDLE rhUniqueID)
{
	ResMapItor itor = m_ResourceMap.find(rhUniqueID);

	if(itor == m_ResourceMap.end())
		return NULL;
	
	// you may need to add your own OS dependent method of getting
	// the current time to set your resource access time

	// set the current time as the last time the object was accessed
	itor->second->SetLastAccess(time(0));

	// recreate the object before giving it to the application
	if(itor->second->IsDisposed())
	{
		itor->second->Recreate();
		AddMemory(itor->second->GetSize());

		// check to see if any overallocation has taken place, but
		// make sure we don't swap out the same resource.
		Lock(rhUniqueID);
		CheckForOverallocation();
		Unlock(rhUniqueID);
	}

 
	// return the object pointer
	return itor->second;
}


BaseResource* ResManager::Lock(RHANDLE rhUniqueID)
{
	ResMapItor itor = m_ResourceMap.find(rhUniqueID);
	if(itor == m_ResourceMap.end())
		return NULL;
	
	// increment the object's count
	itor->second->SetReferenceCount(itor->second->GetReferenceCount() + 1);

	// recreate the object before giving it to the application
	if(itor->second->IsDisposed())
	{
		itor->second->Recreate();
		AddMemory(itor->second->GetSize());
		// check to see if any overallocation has taken place
		CheckForOverallocation();
	}

	// return the object pointer
	return itor->second;
}


int ResManager::Unlock(RHANDLE rhUniqueID)
{
	ResMapItor itor = m_ResourceMap.find(rhUniqueID);
	if(itor == m_ResourceMap.end())
		return -1;
	
	// decrement the object's count
	if(itor->second->GetReferenceCount() > 0)
		itor->second->SetReferenceCount(itor->second->GetReferenceCount() - 1);

	return itor->second->GetReferenceCount();
}

int ResManager::Unlock(BaseResource* pResource)
{
	RHANDLE rhResource = FindResourceHandle(pResource);
	if IS_INVALID_RHANDLE(rhResource)
		return -1;
	return Unlock(rhResource);
}


RHANDLE ResManager::FindResourceHandle(BaseResource* pResource)
{
	// try to find the resource with the specified resource
        ResMapItor itor;
	for(itor = m_ResourceMap.begin(); itor != m_ResourceMap.end(); ++itor)
	{
		if(itor->second == pResource)
			break;
	}
	if(itor == m_ResourceMap.end())
		return INVALID_RHANDLE;
	return itor->first;
}


bool ResManager::CheckForOverallocation()
{
	if(m_nCurrentUsedMemory > m_nMaximumMemory)
	{
		// Attempt to remove iMemToPurge bytes from the managed resource
		int iMemToPurge = m_nCurrentUsedMemory - m_nMaximumMemory;

		// create a temporary priority queue to store the managed items
		priority_queue<BaseResource*, vector<BaseResource*>, ptr_greater<BaseResource*> > PriQueue;

		// insert copies of all the resource pointers into the priority queue, but
		// exclude those that are current disposed or are locked
		for(ResMapItor itor = m_ResourceMap.begin(); itor != m_ResourceMap.end(); ++itor)
		{
			if(!itor->second->IsDisposed() && !itor->second->IsLocked())
				PriQueue.push(itor->second);
		}

		while((!PriQueue.empty()) && (m_nCurrentUsedMemory > m_nMaximumMemory))
		{
			UINT nDisposalSize = PriQueue.top()->GetSize();
			PriQueue.top()->Dispose();
			if(PriQueue.top()->IsDisposed())
				RemoveMemory(nDisposalSize);
			PriQueue.pop();
		}

		// If the resource queue is empty and we still have too much memory allocated,
		// then we return failure.  This could happen if too many resources were locked
		// or if a resource larger than the requested maximum memory was inserted.
		if(PriQueue.empty() && (m_nCurrentUsedMemory > m_nMaximumMemory))
			return false;
	}
	return true;
}



