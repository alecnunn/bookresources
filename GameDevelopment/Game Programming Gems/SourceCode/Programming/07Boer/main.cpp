/* Copyright (C) James Boer, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) James Boer, 2000"
 */

#include <assert.h>
#include "ResourceManager.h"

///////////////////////////////////////////////////////////////////////////////
// Create an imaginary resource.  You can see it satisfies all the requirements
// of the resource type - being able to dispose and recreate it's data with
// no passed parameters, and being able to report the current size and state
// of its internal contents.  A real resource would most likely be reloading
// its data off a hard disk or CD.

class SomeResource : public BaseResource
{
public:
	void Clear()
	{
		m_iDataSize = 0;
		m_pData = 0;
	}

	bool Create(int iSize)
	{
		m_iDataSize = iSize;
		m_pData = new char[m_iDataSize];
		if(!m_pData)
		{
			m_iDataSize = 0;
			return false;
		}
		return true;
	}

	virtual void Dispose()
	{
		if(m_pData)
		{
			delete m_pData;
			m_pData = 0;
		}
	}

	virtual bool Recreate()
	{
		m_pData = new char[m_iDataSize];
		if(!m_pData)
		{
			m_iDataSize = 0;
			return false;
		}
		return true;
	}

	size_t GetSize()
	{
		if(m_pData)
			return m_iDataSize;
		return 0;
	}

	bool IsDisposed()
	{  return (m_pData) ? false : true;  }

protected:
	int		m_iDataSize;
	char*	m_pData;
};



/////////////////////////////////////////////////////////////////////////////
// Create a factory to automate the task of creating a resource and inserting 
// it into the resource manager

class ResourceFactory
{
public:
	void SetResManager(ResManager* pRM)
	{  m_pResManager = pRM;  }
	bool CreateResource(RHANDLE rhID, int iSize);

private:
	ResManager* m_pResManager;
};

bool ResourceFactory::CreateResource(RHANDLE rhID, int iSize)
{
	assert(m_pResManager);
	SomeResource* pRes = new SomeResource;
	if(!pRes)
		return false;
	
	if(!m_pResManager->ReserveMemory(iSize))
		return false;

	if(!pRes->Create(iSize))
		return false;

	if(!m_pResManager->InsertResource(rhID, pRes))
		return false;

	return true;
}

const int Resource1 = 1;
const int Resource2 = 2;
const int Resource3 = 3;
const int Resource4 = 4;
const int Resource5 = 5;

void main()
{
	// Create a resource manager and allow 100 bytes of storage space
	ResManager rm;
	rm.Create(100);

	ResourceFactory rf;
	rf.SetResManager(&rm);

	// Create five resources that will exceed the 100 bytes of storage space
	// The last two resources will force some of the previous resources to
	// be swapped out (it will call their Dispose() function).
	rf.CreateResource(Resource1, 20);
	rf.CreateResource(Resource2, 50);
	rf.CreateResource(Resource3, 10);
	rf.CreateResource(Resource4, 30);
	rf.CreateResource(Resource5, 40);

	// When we lock a resource, the manager assumes we're keeping a pointer
	// to the object to access it at will.  It will not allow the object
	// to be swapped out or destroyed until it's been unlocked again.
	rm.Lock(Resource1);

	// When we try to access all the resources, those that have been swapped
	// out before will be swapped back in (called Recreate()) and others will
	// be swapped out in their place.
	rm.GetResource(Resource2);
	rm.GetResource(Resource3);
	rm.GetResource(Resource4);
	rm.GetResource(Resource5);

	// Be sure to unlock all your resources, or the resource manager will
	// not be able to release your resource, and you will get a memory leak.
	rm.Unlock(Resource1);

	// We don't have to worry about freeing up the resources.  It's handled
	// automatically when the ResourceManager object is destroyed.

}

