/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
  Resources are held in this class for convenience.  The resource manager doesn't 
  hold references to any of the resources, that's the job of the ResourceRef object.
  When the last reference is gone, the resourceref object will call the resource manager
  to remove it from the pool

  Resources can be 'disposed' which means that they reduce themselves to the minimum
  needed to recreate them.

  Resource manager also groups resources for convenience when walking through a set of them.

  - cmaughan@nvidia.com
      
        
******************************************************************************/
#ifndef __NVRESOURCEMANAGER_H
#define __NVRESOURCEMANAGER_H

#include "singleton.h"
#include "nvdebug.h"
namespace nv_objects
{

class NVResourceManager;
class NVResourceRef;

typedef enum
{
	NVRT_TEXTURE = 0,
	NVRT_MATERIAL = 1,
	NVRT_RENDERSTYLE = 2
} NVBaseResourceType;

class NVBaseResource
{
public:

	NVBaseResource(NVBaseResourceType Type)
		: m_ResourceType(Type),
		m_dwRef(0),
		m_dwLockRef(0)
	{}

	virtual ~NVBaseResource();

	DWORD Lock()
	{
		m_dwLockRef++;
		return m_dwLockRef;
	}

	DWORD Unlock()
	{
		assert(m_dwLockRef > 0);
		DWORD dwLockRef = --m_dwLockRef;
		return dwLockRef;
	}

	virtual bool IsLocked() const { return m_dwLockRef > 0; }
	virtual bool Invoke() = 0;
	virtual bool Dispose() = 0;
	virtual bool IsDisposed() const = 0;
	virtual bool IsDeviceObject() const = 0;
	const NVBaseResourceType GetResourceType() const { return m_ResourceType; }


private:
	friend NVResourceManager;
	friend NVResourceRef;
	DWORD AddRef()
	{
		m_dwRef++;
		return m_dwRef;
	}

	DWORD Release()
	{
		DWORD dwRef = --m_dwRef;
		if (dwRef == 0)
		{
			delete this;
		}
		return dwRef;
	}


	DWORD m_dwRef;
	DWORD m_dwLockRef;
	NVBaseResourceType m_ResourceType;
	
};

class NVResourceRef
{
public:
	NVResourceRef()
		: m_pResource(NULL)
	{}

	NVResourceRef(NVBaseResource* pResource)
	{
		m_pResource = pResource;
		if (m_pResource)
		{
			m_pResource->AddRef();
		}
	}

	NVResourceRef(const NVResourceRef& rhs)
	{
		m_pResource = rhs.m_pResource;
		if (m_pResource)
		{
			m_pResource->AddRef();
		}
	}

	const NVResourceRef& operator = (const NVResourceRef& rhs)
	{
		SAFE_RELEASE(m_pResource);
		m_pResource = rhs.m_pResource;
		if (m_pResource)
		{
			m_pResource->AddRef();
		}
		return *this;
	}

	~NVResourceRef()
	{
		SAFE_RELEASE(m_pResource);
	}

	bool operator == (const NVResourceRef& rhs)
	{
		if (rhs.m_pResource != m_pResource)
			return false;

		return true;
	}

	bool operator != (const NVResourceRef& rhs)
	{
		if (rhs.m_pResource == m_pResource)
			return false;

		return true;
	}

	NVBaseResource* GetPointer()
	{
		if (!m_pResource)
			return NULL;

		if (m_pResource->IsDisposed())
		{
			if (m_pResource->Invoke())
				return m_pResource;
			else
				return NULL;
		}

		return m_pResource; 
	}

private:
	friend NVResourceManager;
	NVBaseResource* GetInternalPointer() const { return m_pResource; }

	NVBaseResource* m_pResource;
};

// For named resources
typedef std::map<std::string, NVBaseResource*> tResourceFromName;
typedef std::map<NVBaseResource*, std::string> tResourceNameFromResource;
typedef std::set<NVBaseResource*> tResourceSet;


class NVResourceManager : public Singleton <NVResourceManager>
{
public:
	NVResourceManager(LPDIRECT3DDEVICE8 pDevice)
		: m_pD3DDev(pDevice)
	{
		assert(pDevice);
		pDevice->AddRef();
	}

	~NVResourceManager()
	{
		NVASSERT(m_Resources.empty(), "NVResourceManager: " << m_Resources.size() << " resources left");
		SAFE_RELEASE(m_pD3DDev);
	}

	// Debug help routine
#ifdef _DEBUG
	std::string Debug_GetResourceInfo(NVBaseResource* pResource);
#endif

	void DisposeResources();
	void DisposeDeviceObjects();
	NVResourceRef FindResource(const char* pszResourceName);
	NVResourceRef AddResource(NVBaseResource* pResource, const char* pszName = NULL);

	NVResourceRef GetFirstMaterial();
	NVResourceRef GetNextMaterial();

	NVResourceRef GetFirstTexture();
	NVResourceRef GetNextTexture();

	NVResourceRef GetFirstRenderStyle();
	NVResourceRef GetNextRenderStyle();

private:
	friend NVBaseResource;
	bool RemoveResource(NVBaseResource* pResource);

	tResourceFromName m_ResourceFromName;
	tResourceNameFromResource m_ResourceNameFromResource;

	tResourceSet m_Resources;

	tResourceSet m_MaterialResources;
	tResourceSet m_RenderStyleResources;
	tResourceSet m_TextureResources;

	tResourceSet::iterator m_itrCurrentMaterial;
	tResourceSet::iterator m_itrCurrentRenderStyle;
	tResourceSet::iterator m_itrCurrentTexture;

	LPDIRECT3DDEVICE8 m_pD3DDev;

};

#define NVRESOURCEMANAGER (NVResourceManager::GetSingleton())
#ifdef _DEBUG
#define DEBUG_GETRESOURCEINFO(a) Debug_GetResourceInfo(a)
#else
#define DEBUG_GETRESOURCEINFO(a) ""
#endif


}; //nv_objects

#endif __NVRESOURCEMANAGER_H

