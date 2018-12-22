/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
  - cmaughan@nvidia.com
      
        
******************************************************************************/
#ifndef __NVRESOURCEMANAGER_H
#define __NVRESOURCEMANAGER_H

#include "singleton.h"
#include "nvdebug.h"
namespace nv_objects
{

typedef DWORD NVResourceID;
static DWORD NVINVALID_RESOURCEID = 0xFFFFFFFF;

class NVBaseResource
{
public:
	typedef enum
	{
		NVRT_TEXTURE = 0,
		NVRT_MATERIAL = 1
	} NVBaseResourceType;

	NVBaseResource(NVBaseResourceType Type)
		: m_ResourceType(Type),
		m_dwRef(0)
	{}

	virtual ~NVBaseResource()
	{}

	LONG Lock()
	{
		m_dwRef++;
		return m_dwRef;
	}

	LONG Unlock()
	{
		assert(m_dwRef > 0);
		ULONG m_dwRef = --m_dwRef;
		return m_dwRef;
	}

	ULONG GetRefCount() const { return m_dwRef; }
	virtual bool IsLocked() const { return m_dwRef > 0; }

	virtual bool Invoke() = 0;
	virtual bool Dispose() = 0;
	virtual bool IsDisposed() const = 0;
	virtual bool IsDeviceObject() const = 0;
	const NVBaseResourceType GetResourceType() const { return m_ResourceType; }


protected:
	DWORD m_dwRef;
	NVBaseResourceType m_ResourceType;
	
};

// For resources with ID's only
typedef std::map<NVResourceID, NVBaseResource*> tResourceFromID;

// For named resources
typedef std::map<std::string, NVResourceID> tResourceIDFromName;
typedef std::map<NVResourceID, std::string> tResourceNameFromID;
typedef std::set<NVResourceID> tResourceIDSet;

class NVResourceManager : public Singleton <NVResourceManager>
{
public:
	NVResourceManager(LPDIRECT3DDEVICE8 pDevice)
		: m_pD3DDev(pDevice),
		m_CurrentID(0)
	{
		assert(pDevice);
		pDevice->AddRef();

	}

	virtual ~NVResourceManager()
	{
		DISPDBG(3, "NVResourceManager: " << m_ResourceFromID.size() << " resources left");

		DestroyResources();

		SAFE_RELEASE(m_pD3DDev);
	}

	// Debug help routine
#ifdef _DEBUG
	std::string Debug_GetResourceInfo(NVResourceID ResourceID)
	{
		std::string strResourceType;
		std::string strResourceName = " (Unknown)";

		tResourceFromID::const_iterator itrResource = m_ResourceFromID.find(ResourceID);
		if (itrResource != m_ResourceFromID.end())
		{
			switch(itrResource->second->GetResourceType())
			{
				case NVBaseResource::NVRT_MATERIAL:
					strResourceType = "(Material)";
					break;
				case NVBaseResource::NVRT_TEXTURE:
					strResourceType = "(Texture)";
					break;
				default:
					break;
			}
			
			tResourceNameFromID::const_iterator itrName = m_ResourceNameFromID.find(ResourceID);
			if (itrName != m_ResourceNameFromID.end())
				strResourceName = " '" + itrName->second + "' " + strResourceType;
			else
				strResourceName = " " + strResourceType;
		}

		return strResourceName;
	}
#define DEBUG_GETRESOURCEINFO(a) Debug_GetResourceInfo(a)
#else
#define DEBUG_GETRESOURCEINFO(a) ""
#endif

	void DestroyResources()
	{
		DISPDBG(3, "NVResourceManager: DestroyResources: ");
		
		while (!m_ResourceFromID.empty())
		{
			tResourceFromID::iterator itrResource = m_ResourceFromID.begin();
			RemoveResource(itrResource->first);
		}
	}

	void DisposeResources()
	{
		DISPDBG(3, "NVResourceManager: DisposeResources");

		tResourceFromID::iterator itrResource = m_ResourceFromID.begin();
		while (itrResource != m_ResourceFromID.end())
		{
			bool bDisposed = itrResource->second->Dispose();
			if (bDisposed)
			{
				DISPDBG(3, "NVResourceManager: Disposed " << DEBUG_GETRESOURCEINFO(itrResource->first));
			}
			else
			{
				DISPDBG(3, "NVResourceManager: Did not dispose " << DEBUG_GETRESOURCEINFO(itrResource->first));
			}

			itrResource++;
		}
	}

	void DisposeDeviceObjects()
	{
		DISPDBG(3, "NVResourceManager: DisposeDeviceObjects");

		tResourceFromID::iterator itrResource = m_ResourceFromID.begin();
		while (itrResource != m_ResourceFromID.end())
		{
			if (itrResource->second->IsDeviceObject())
			{
				bool bDisposed = itrResource->second->Dispose();

				if (bDisposed)
				{
					DISPDBG(3, "NVResourceManager: Disposed " << DEBUG_GETRESOURCEINFO(itrResource->first));
				}
				else
				{
					DISPDBG(0, "NVResourceManager: Could not dispose " << DEBUG_GETRESOURCEINFO(itrResource->first) << " device object");
				}
			}

			itrResource++;
		}
	}

	NVBaseResource* FindResource(NVResourceID ResourceID)
	{
		tResourceFromID::const_iterator itrResource = m_ResourceFromID.find(ResourceID);
		if (itrResource != m_ResourceFromID.end())
		{
			if (itrResource->second->IsDisposed())
			{
				bool bSuceeded = itrResource->second->Invoke();

				if (!bSuceeded)
				{
					NVASSERT(bSuceeded, "Failed to invoke resource: " << DEBUG_GETRESOURCEINFO(itrResource->first));
				}
				else
				{
					DISPDBG(3, "NVResourceManager: Invoked " << DEBUG_GETRESOURCEINFO(itrResource->first));
				}
			}

			return (itrResource->second);
		}

		return NULL;
	}

	NVResourceID FindResourceID(const char* pszResourceName)
	{
		if (!pszResourceName || *pszResourceName == 0)
			return NVINVALID_RESOURCEID;

		tResourceIDFromName::const_iterator itrResource = m_ResourceIDFromName.find(pszResourceName);
		if (itrResource != m_ResourceIDFromName.end())
		{
			return itrResource->second;
		}

		return NVINVALID_RESOURCEID;
	}

	NVBaseResource* FindResource(const char* pszResourceName)
	{
		NVResourceID ThisID = FindResourceID(pszResourceName);
		if (ThisID != NVINVALID_RESOURCEID)
			return FindResource(ThisID);

		return NULL;
	}


	NVResourceID AddResource(NVBaseResource* pResource, const char* pszName = NULL)
	{
		assert(pResource);
#ifdef _DEBUG
		NVASSERT(FindResourceID(pszName) == NVINVALID_RESOURCEID, "Tried to add a named resource that existed!");
#endif
		switch(pResource->GetResourceType())
		{
			case NVBaseResource::NVRT_MATERIAL:
				m_MaterialResources.insert(m_CurrentID);
				break;
			case NVBaseResource::NVRT_TEXTURE:
				m_TextureResources.insert(m_CurrentID);
				break;
			default:
				break;
		}
		
		m_ResourceFromID[m_CurrentID] = pResource;
		if (pszName)
		{
			m_ResourceIDFromName[pszName] = m_CurrentID;
			m_ResourceNameFromID[m_CurrentID] = pszName;
		}

		DISPDBG(3, "NVResourceManager: Added: " << m_CurrentID << DEBUG_GETRESOURCEINFO(m_CurrentID));

		m_CurrentID++;
		return (m_CurrentID - 1);		
	}

	bool RemoveResource(NVResourceID ResourceID)
	{
		tResourceFromID::iterator itrResources = m_ResourceFromID.find(ResourceID);
		
		DISPDBG(3, "NVResourceManager: Removed: " << itrResources->first << DEBUG_GETRESOURCEINFO(ResourceID));
		
		if (itrResources != m_ResourceFromID.end())
		{
			// Handle resource type lists
			switch(itrResources->second->GetResourceType())
			{
				case NVBaseResource::NVRT_MATERIAL:
					m_MaterialResources.erase(m_CurrentID);
					break;
				case NVBaseResource::NVRT_TEXTURE:
					m_TextureResources.erase(m_CurrentID);
					break;
				default:
					break;
			}

			// Handle names
			// Find the resource name in our ID list, if it was named
			tResourceNameFromID::iterator itrName = m_ResourceNameFromID.find(ResourceID);
			if (itrName != m_ResourceNameFromID.end())
			{
				// Find the resource ID in our name list if it was named
				tResourceIDFromName::iterator itrID = m_ResourceIDFromName.find(itrName->second);
				if (itrID != m_ResourceIDFromName.end())
				{
					// Remove from the lookup
					m_ResourceIDFromName.erase(itrID);
				}

				// Remove from the lookup
				m_ResourceNameFromID.erase(itrName);
			}

			// Erase the resource itself (delete the memory)
			SAFE_DELETE(itrResources->second);
			m_ResourceFromID.erase(itrResources);

			return true;
		}
		return false;
	}

	const tResourceIDSet& GetMaterialResources() const { return m_MaterialResources; }
	const tResourceIDSet& GetTextureResources() const { return m_TextureResources; }

private:
	tResourceFromID m_ResourceFromID;
	
	tResourceIDFromName m_ResourceIDFromName;
	tResourceNameFromID m_ResourceNameFromID;
	
	tResourceIDSet m_MaterialResources;
	tResourceIDSet m_TextureResources;

	NVResourceID m_CurrentID;
	LPDIRECT3DDEVICE8 m_pD3DDev;

};

#define NVRESOURCEMANAGER (NVResourceManager::GetSingleton())


}; //nv_objects

#endif __NVRESOURCEMANAGER_H

