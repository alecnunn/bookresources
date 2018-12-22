/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
   - cmaughan@nvidia.com
        
******************************************************************************/
#ifndef __NVTEXTURERESOURCE_H
#define __NVTEXTURERESOURCE_H

#include "nvresourcemanager.h"
#include "nvtexture.h"

namespace nv_objects
{

class NVTextureResource : public NVBaseResource
{
public:
	NVTextureResource(LPDIRECT3DBASETEXTURE8 pTexture = NULL)
		: NVBaseResource(NVRT_TEXTURE),
			m_pTexture(pTexture)
	{};

	~NVTextureResource()
	{
		SAFE_RELEASE(m_pTexture);
	};

	virtual bool IsDisposed() const { return (m_pTexture == NULL); }

	static NVTextureResource* GetTextureResourcePtr(NVResourceRef Texture)
	{
		NVTextureResource* pTexture = static_cast<NVTextureResource*>(Texture.GetPointer());
		NVASSERT(pTexture && pTexture->GetResourceType() == NVRT_TEXTURE, "Not a texture resource");
		return pTexture;
	}

	// Static helpers for finding texture resources.
	// Note that the call to findresource will make sure the texture is created before it returns.
	// Any calls to NVRESOURCEMANAGER.FindResource will ensure creation before return.
	static LPDIRECT3DBASETEXTURE8 GetTexture(const char* pszResource)
	{
		NVResourceRef Resource = NVRESOURCEMANAGER.FindResource(pszResource);
		NVTextureResource* pTexture = static_cast<NVTextureResource*>(Resource.GetPointer());
		if (pTexture)
		{
			NVASSERT(pTexture->GetResourceType() == NVRT_TEXTURE, "Not a texture resource");
			return pTexture->m_pTexture;
		}
		return NULL;
	}

	static LPDIRECT3DBASETEXTURE8 GetTexture(NVResourceRef Texture)
	{
		NVTextureResource* pTexture = static_cast<NVTextureResource*>(Texture.GetPointer());
		if (pTexture)
		{
			NVASSERT(pTexture->GetResourceType() == NVRT_TEXTURE, "Not a texture resource");
			return pTexture->m_pTexture;
		}
		return NULL;
	}


protected:
	LPDIRECT3DBASETEXTURE8 m_pTexture;
};
typedef std::map<const std::string, NVTextureResource> tNVTextureResourceMap;
	
// An NVTextureFileResource is constructed with the specs of how to create it 
// from a file name.  A later invoke call will ensure it is allocated.
// It can be disposed and reinvoked at any time by the resource manager.
class NVTextureFileResource : public NVTextureResource
{
public:
	typedef enum
	{
		TextureType_2D = 0,
		TextureType_CubeMap = 1
	} NVTextureFileResourceType;

	NVTextureFileResource(LPDIRECT3DDEVICE8 pD3DDev, 
				const std::string& strFileName, 
				NVTextureFileResourceType Type = TextureType_2D,
				DWORD dwWidth = D3DX_DEFAULT,
				DWORD dwHeight = D3DX_DEFAULT, 
				DWORD dwMipLevels = 0,
				DWORD dwUsage = 0,
				D3DFORMAT Format = D3DFMT_UNKNOWN,
				D3DPOOL Pool = D3DPOOL_MANAGED,
				DWORD Filter = D3DX_FILTER_LINEAR, 
				DWORD MipFilter = D3DX_FILTER_LINEAR)
		: m_pD3DDev(pD3DDev),
			m_strFileName(strFileName),
			m_Type(Type),
			m_dwWidth(dwWidth),
			m_dwHeight(dwHeight),
			m_dwMipLevels(dwMipLevels),
			m_dwUsage(dwUsage),
			m_Format(Format),
			m_Pool(Pool),
			m_Filter(Filter),
			m_MipFilter(MipFilter)
	{
	}
	
	virtual ~NVTextureFileResource()
	{
	}


	virtual bool Invoke()
	{
		HRESULT hr;
		if (m_pTexture)
			return true;

		switch(m_Type)
		{
			case TextureType_2D:
				hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
					m_strFileName.c_str(),
					m_dwWidth,
					m_dwHeight,
					m_dwMipLevels,
					m_dwUsage,
					m_Format,
					m_Pool,
					m_Filter,
					m_MipFilter,
					0,
					NULL,
					NULL,
					(LPDIRECT3DTEXTURE8*)&m_pTexture);		
				break;
			case TextureType_CubeMap:
				hr = D3DXCreateCubeTextureFromFileEx(m_pD3DDev, 
					m_strFileName.c_str(),
					m_dwWidth,
					m_dwMipLevels,
					m_dwUsage,
					m_Format,
					m_Pool,
					m_Filter,
					m_MipFilter,
					0,
					NULL,
					NULL,
					(LPDIRECT3DCUBETEXTURE8*)&m_pTexture);		
				break;
			default:
				break;
		}

		return (hr == S_OK);
	}

	virtual bool Dispose()
	{
		if (!m_pTexture)
			return true;

		if (IsLocked())
			return false;

		SAFE_RELEASE(m_pTexture);
		return true;
	}

	virtual bool IsDeviceObject() const
	{
		if (m_Pool == D3DPOOL_DEFAULT)
			return true;

		return false; 
	}

private:
	std::string m_strFileName;
	NVTextureFileResourceType m_Type;
	DWORD m_dwWidth;
	DWORD m_dwHeight;
	DWORD m_dwMipLevels;
	DWORD m_dwUsage;
	D3DFORMAT m_Format;
    D3DPOOL m_Pool;
	DWORD m_Filter;
	DWORD m_MipFilter;
	LPDIRECT3DDEVICE8 m_pD3DDev;

};

class NVTextureNormalMapFromResource : public NVTextureResource
{
public:
	NVTextureNormalMapFromResource(LPDIRECT3DDEVICE8 pD3DDev, NVResourceRef BaseResource, D3DFORMAT Format = (D3DFORMAT)MAKEFOURCC('N', 'V', 'H', 'S'), D3DPOOL Pool = D3DPOOL_DEFAULT)
		: m_pD3DDev(pD3DDev),
		m_BaseResource(BaseResource),
		m_Format(Format),
		m_Pool(Pool)
	{}

	virtual bool Invoke()
	{
		m_pTexture = NVTexture2::CreateNormalMap(m_pD3DDev, (LPDIRECT3DTEXTURE8)NVTextureResource::GetTexture(m_BaseResource), m_Format, m_Pool);
		if (m_pTexture)
			return true;

		return false;
	}

	virtual bool Dispose()
	{
		if (!m_pTexture)
			return true;

		if (IsLocked())
			return false;

		SAFE_RELEASE(m_pTexture);

		return true;
	}

	virtual bool IsDeviceObject() const
	{
		if (m_Pool == D3DPOOL_DEFAULT)
			return true;

		return false; 
	}

	
private:
	LPDIRECT3DDEVICE8 m_pD3DDev;
	NVResourceRef m_BaseResource;
	D3DFORMAT m_Format;
	D3DPOOL m_Pool;
};


// A static texture resource.  Can be created later, or on spec.  Only disposable
// if it is created with parameters.  If you pass a texture pointer then it is treated as fixed.
class NVTextureStaticResource : public NVTextureResource
{
public:
	typedef enum
	{
		TextureType_2D = 0,
		TextureType_CubeMap = 1
	} NVTextureStaticResourceType;

	NVTextureStaticResource(LPDIRECT3DDEVICE8 pD3DDev, 
				NVTextureStaticResourceType Type = TextureType_2D,
				DWORD dwWidth = D3DX_DEFAULT,
				DWORD dwHeight = D3DX_DEFAULT, 
				DWORD dwMipLevels = 0,
				DWORD dwUsage = 0,
				D3DFORMAT Format = D3DFMT_UNKNOWN,
				D3DPOOL Pool = D3DPOOL_MANAGED)
		: m_pD3DDev(pD3DDev),
			m_Type(Type),
			m_dwWidth(dwWidth),
			m_dwHeight(dwHeight),
			m_dwMipLevels(dwMipLevels),
			m_dwUsage(dwUsage),
			m_Format(Format),
			m_Pool(Pool)
	{
		if (Type == TextureType_CubeMap)
		{
			NVASSERT(dwWidth == dwHeight, "Width/Height must be same for cubemap!");
		}
	};
	
	virtual ~NVTextureStaticResource()
	{
		SAFE_RELEASE(m_pTexture);
	}
	
	virtual bool Invoke() 
	{
		HRESULT hr;
		if (m_pTexture)
			return true;

		switch(m_Type)
		{
			case TextureType_2D:
				hr = D3DXCreateTexture(m_pD3DDev, 
					m_dwWidth,
					m_dwHeight,
					m_dwMipLevels,
					m_dwUsage,
					m_Format,
					m_Pool,
					(LPDIRECT3DTEXTURE8*)&m_pTexture);		
				break;

			case TextureType_CubeMap:
				hr = D3DXCreateCubeTexture(m_pD3DDev, 
					m_dwWidth,
					m_dwMipLevels,
					m_dwUsage,
					m_Format,
					m_Pool,
					(LPDIRECT3DCUBETEXTURE8*)&m_pTexture);
				break;
			default:
				break;
		}

		return (hr == S_OK);
	}

	virtual bool Dispose()
	{
		if (!m_pTexture)
			return true;

		if (IsLocked())
			return false;

		SAFE_RELEASE(m_pTexture);
		return true;
	}

	virtual bool IsDeviceObject() const
	{
		if (m_Pool == D3DPOOL_DEFAULT)
			return true;

		return false; 
	}

private:
	std::string m_strFileName;
	NVTextureStaticResourceType m_Type;
	DWORD m_dwWidth;
	DWORD m_dwHeight;
	DWORD m_dwMipLevels;
	DWORD m_dwUsage;
	D3DFORMAT m_Format;
    D3DPOOL m_Pool;
	DWORD m_Filter;
	DWORD m_MipFilter;
	LPDIRECT3DDEVICE8 m_pD3DDev;
};

}; // namespace nv_objects

#endif
