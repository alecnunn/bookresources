/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
  
  - cmaughan@nvidia.com
      
        
******************************************************************************/

#include "nvresourcemanager.h"

namespace nv_objects
{

// Remove a resource from the manager. Is private, and only called by the resourceref destructor
bool NVResourceManager::RemoveResource(NVBaseResource* pResource)
{
	if (pResource == NULL)
		return true;

	tResourceSet::iterator itrResource = m_Resources.find(pResource);
	
	DISPDBG(3, "NVResourceManager: Removed: " << DEBUG_GETRESOURCEINFO(pResource));
	
	if (itrResource != m_Resources.end())
	{
		// Handle resource type lists
		switch(pResource->GetResourceType())
		{
			case NVRT_MATERIAL:
				m_MaterialResources.erase(pResource);
				break;
			case NVRT_TEXTURE:
				m_TextureResources.erase(pResource);
				break;
			case NVRT_RENDERSTYLE:
				m_RenderStyleResources.erase(pResource);
				break;
			default:
				break;
		}

		// Handle names
		// Find the resource name in our list, if it was named
		tResourceNameFromResource::iterator itrName = m_ResourceNameFromResource.find(pResource);
		if (itrName != m_ResourceNameFromResource.end())
		{
			// Find the resource in our name list if it was named
			tResourceFromName::iterator itrResource = m_ResourceFromName.find(itrName->second);
			if (itrResource != m_ResourceFromName.end())
			{
				// Remove from the lookup
				m_ResourceFromName.erase(itrResource);
			}

			// Remove from the lookup
			m_ResourceNameFromResource.erase(itrName);
		}

		m_Resources.erase(itrResource);

		return true;
	}
	return false;
}

// Dispose all current resources - doesn't remove them, simply causes them to remove their re-allocatable
// resource state
void NVResourceManager::DisposeResources()
{
	DISPDBG(3, "NVResourceManager: DisposeResources");

	tResourceSet::iterator itrResource = m_Resources.begin();
	while (itrResource != m_Resources.end())
	{
		NVBaseResource* pResource = (*itrResource);
		bool bDisposed = pResource->Dispose();
		if (bDisposed)
		{
			DISPDBG(3, "NVResourceManager: Disposed " << DEBUG_GETRESOURCEINFO(pResource));
		}
		else
		{
			DISPDBG(3, "NVResourceManager: Did not dispose " << DEBUG_GETRESOURCEINFO(pResource));
		}

		itrResource++;
	}
}

// Disposes resources tied to devices
void NVResourceManager::DisposeDeviceObjects()
{
	DISPDBG(3, "NVResourceManager: DisposeDeviceObjects");

	tResourceSet::iterator itrResource = m_Resources.begin();
	while (itrResource != m_Resources.end())
	{
		NVBaseResource* pResource = (*itrResource);

		if (pResource->IsDeviceObject())
		{
			bool bDisposed = pResource->Dispose();

			if (bDisposed)
			{
				DISPDBG(3, "NVResourceManager: Disposed " << DEBUG_GETRESOURCEINFO(pResource));
			}
			else
			{
				DISPDBG(0, "NVResourceManager: Could not dispose " << DEBUG_GETRESOURCEINFO(pResource) << " device object");
			}
		}

		itrResource++;
	}
}

// Finds a named resource
NVResourceRef NVResourceManager::FindResource(const char* pszResourceName)
{
	if (!pszResourceName || *pszResourceName == 0)
		return NVResourceRef(NULL);

	tResourceFromName::const_iterator itrResource = m_ResourceFromName.find(pszResourceName);
	if (itrResource != m_ResourceFromName.end())
	{
		return NVResourceRef(itrResource->second);
	}

	return NVResourceRef(NULL);
}

// Add a resource with an optional name
NVResourceRef NVResourceManager::AddResource(NVBaseResource* pResource, const char* pszName)
{
	assert(pResource);
#ifdef _DEBUG
	NVASSERT(FindResource(pszName) == NULL, "Tried to add a named resource that existed!");
#endif

	switch(pResource->GetResourceType())
	{
		case NVRT_MATERIAL:
			m_MaterialResources.insert(pResource);
			break;
		case NVRT_TEXTURE:
			m_TextureResources.insert(pResource);
			break;
		case NVRT_RENDERSTYLE:
			m_RenderStyleResources.insert(pResource);
			break;
		default:
			break;
	}
	
	m_Resources.insert(pResource);
	if (pszName)
	{
		m_ResourceFromName[pszName] = pResource;
		m_ResourceNameFromResource[pResource] = pszName;
	}

	DISPDBG(3, "NVResourceManager: Added: " << DEBUG_GETRESOURCEINFO(pResource));

	return (NVResourceRef(pResource));
}

NVResourceRef NVResourceManager::GetFirstMaterial()
{
	m_itrCurrentMaterial = m_MaterialResources.begin();
	return NVResourceRef(*m_itrCurrentMaterial);
}

NVResourceRef NVResourceManager::GetFirstTexture()
{
	m_itrCurrentTexture = m_TextureResources.begin();
	return NVResourceRef(*m_itrCurrentTexture);
}

NVResourceRef NVResourceManager::GetFirstRenderStyle()
{
	m_itrCurrentRenderStyle = m_RenderStyleResources.begin();
	return NVResourceRef(*m_itrCurrentRenderStyle);
}

NVResourceRef NVResourceManager::GetNextMaterial()
{
	m_itrCurrentMaterial++;
	if (m_itrCurrentMaterial == m_MaterialResources.end())
		return NVResourceRef(NULL);

	return NVResourceRef(*m_itrCurrentMaterial);
}

NVResourceRef NVResourceManager::GetNextTexture()
{
	m_itrCurrentTexture++;
	if (m_itrCurrentTexture == m_TextureResources.end())
		return NVResourceRef(NULL);

	return NVResourceRef(*m_itrCurrentTexture);
}

NVResourceRef NVResourceManager::GetNextRenderStyle()
{
	m_itrCurrentRenderStyle++;
	if (m_itrCurrentRenderStyle == m_RenderStyleResources.end())
		return NVResourceRef(NULL);

	return NVResourceRef(*m_itrCurrentRenderStyle);
}


#ifdef _DEBUG
// Debug helper function
std::string NVResourceManager::Debug_GetResourceInfo(NVBaseResource* pResource)
{
	std::string strResourceType = " (Unknown)";
	std::ostringstream strOut;

	switch(pResource->GetResourceType())
	{
		case NVRT_MATERIAL:
			strResourceType = "(Material)";
			break;
		case NVRT_TEXTURE:
			strResourceType = "(Texture)";
			break;
		case NVRT_RENDERSTYLE:
			strResourceType = "(RenderStyle)";
		default:
			break;
	}
		
	tResourceNameFromResource::const_iterator itrName = m_ResourceNameFromResource.find(pResource);
	if (itrName != m_ResourceNameFromResource.end())
		strOut << pResource << " '" << itrName->second << "' " << strResourceType;
	else
		strOut << pResource << " 'UNNAMED' " << strResourceType;

	return strOut.str();
}
#endif

NVBaseResource::~NVBaseResource()
{
	NVRESOURCEMANAGER.RemoveResource(this);
}


};

