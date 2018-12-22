/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#include "nvtex.h"
#include "dds.h"

using namespace std;

namespace nv_objects
{


NVBaseTexture::NVBaseTexture()
: m_dwWidth(0),
	m_dwHeight(0),
	m_d3dFormat(D3DFMT_UNKNOWN)
{
}

NVTexture::NVTexture()
: m_pTexture(NULL)
{

}

NVCubeTexture::NVCubeTexture()
: m_pCubeTexture(NULL)
{

}

NVHILOTexture::NVHILOTexture()
: m_pHILOTexture(NULL)
{

}

NVTexture::~NVTexture()
{
    SAFE_RELEASE(m_pTexture);
}

NVCubeTexture::~NVCubeTexture()
{
    SAFE_RELEASE(m_pCubeTexture);
}

NVHILOTexture::~NVHILOTexture()
{
	SAFE_RELEASE(m_pHILOTexture);
}

HRESULT NVTexture::CreateTexture(LPDIRECT3DDEVICE8 pD3DDev, DWORD dwWidth, DWORD dwHeight, WORD dwMipmaps, D3DFORMAT d3dFormat)
{
	HRESULT hr;

	hr = D3DXCreateTexture(pD3DDev, dwWidth, dwHeight, dwMipmaps, 0, d3dFormat, D3DPOOL_MANAGED, &m_pTexture);	
	if (FAILED(hr))
		return hr;

	m_dwWidth = dwWidth;
	m_dwHeight = dwHeight;
	m_d3dFormat = d3dFormat;

	return S_OK;
}

HRESULT NVHILOTexture::CreateTexture(LPDIRECT3DDEVICE8 pD3DDev, DWORD dwWidth, DWORD dwHeight, WORD dwMipmaps)
{
	HRESULT hr;

	hr = pD3DDev->CreateTexture(dwWidth, dwHeight, dwMipmaps, 0, (D3DFORMAT)MAKEFOURCC('N', 'V', 'H', 'S'), D3DPOOL_DEFAULT, &m_pHILOTexture);	
	if (FAILED(hr))
		return hr;

	m_dwWidth = dwWidth;
	m_dwHeight = dwHeight;
	m_d3dFormat = (D3DFORMAT)MAKEFOURCC('N', 'V', 'H', 'S');

	return S_OK;
}

HRESULT NVCubeTexture::CreateCubeTexture(LPDIRECT3DDEVICE8 pD3DDev, DWORD dwWidth, WORD dwMipmaps, D3DFORMAT d3dFormat)
{
	HRESULT hr;

	hr = D3DXCreateCubeTexture(pD3DDev, dwWidth, dwMipmaps, 0, d3dFormat, D3DPOOL_MANAGED, &m_pCubeTexture);
	if (FAILED(hr))
		return hr;

	m_dwWidth = dwWidth;
	m_dwHeight = dwWidth;
	m_d3dFormat = d3dFormat;

	return S_OK;
}

HRESULT NVCubeTexture::CreateNormalizationCubeMap(LPDIRECT3DDEVICE8 pD3DDev, DWORD dwWidth, WORD dwMipmaps)
{
	HRESULT hr;

	hr = D3DXCreateCubeTexture(pD3DDev, dwWidth, dwMipmaps, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &m_pCubeTexture);
	if(FAILED(hr))
	{
		return hr;
	}

	m_dwWidth = dwWidth;
	m_dwHeight = dwWidth;
	m_d3dFormat = D3DFMT_X8R8G8B8;

	for (int i = 0; i < 6; i++)
	{
		D3DLOCKED_RECT Locked;
		D3DXVECTOR3 Normal;
		float w,h;
		D3DSURFACE_DESC ddsdDesc;
		
		m_pCubeTexture->GetLevelDesc(0, &ddsdDesc);

		m_pCubeTexture->LockRect((D3DCUBEMAP_FACES)i, 0, &Locked, NULL, 0);

		for (int y = 0; y < ddsdDesc.Height; y++)
		{
			h = (float)y / ((float)(ddsdDesc.Height - 1));
			h *= 2.0f;
			h -= 1.0f;

			for (int x = 0; x < ddsdDesc.Width; x++)
			{
				w = (float)x / ((float)(ddsdDesc.Width - 1));
				w *= 2.0f;
				w -= 1.0f;

				DWORD* pBits = (DWORD*)((BYTE*)Locked.pBits + (y * Locked.Pitch));
				pBits += x;

				switch((D3DCUBEMAP_FACES)i)
				{
					case D3DCUBEMAP_FACE_POSITIVE_X:
						Normal = D3DXVECTOR3(1.0f, -h, -w);
						break;
					case D3DCUBEMAP_FACE_NEGATIVE_X:
						Normal = D3DXVECTOR3(-1.0f, -h, w);
						break;
					case D3DCUBEMAP_FACE_POSITIVE_Y:
						Normal = D3DXVECTOR3(w, 1.0f, h);
						break;
					case D3DCUBEMAP_FACE_NEGATIVE_Y:
						Normal = D3DXVECTOR3(w, -1.0f, -h);
						break;
					case D3DCUBEMAP_FACE_POSITIVE_Z:
						Normal = D3DXVECTOR3(w, -h, 1.0f);
						break;
					case D3DCUBEMAP_FACE_NEGATIVE_Z:
						Normal = D3DXVECTOR3(-w, -h, -1.0f);
						break;
					default:
						assert(0);
						break;
				}

				D3DXVec3Normalize(&Normal, &Normal);

				// Scale to be a color from 0 to 255 (127 is 0)
				Normal += D3DXVECTOR3(1.0f, 1.0f, 1.0f);
				Normal *= 127.0f;

				// Store the color
				*pBits = (DWORD)(((DWORD)Normal.x << 16) | ((DWORD)Normal.y << 8) | ((DWORD)Normal.z << 0));

			}
		}
		m_pCubeTexture->UnlockRect((D3DCUBEMAP_FACES)i, 0);
	}

	if ((dwMipmaps == 0) || (dwMipmaps > 1))
		D3DXFilterCubeTexture(m_pCubeTexture, NULL, 0, D3DX_FILTER_LINEAR);

	return S_OK;
}

HRESULT NVTexture::CreateFromFile(LPDIRECT3DDEVICE8 pDev, const std::string& strFileName, D3DFORMAT d3dFormat, DWORD dwMipLevels)
{
	HRESULT hr;
	string strExtension;
    string strPathname;

	std::string::size_type Pos = strFileName.find_last_of("\\", strFileName.size());
	if (Pos != strFileName.npos)
	{
		// Check the root directory of the running process
		strPathname = strFileName;
	}
	else
	{
		int	const	kReturnSize = GetCurrentDirectory(0, &strPathname[0]);
		strPathname.resize(kReturnSize);

		GetCurrentDirectory(kReturnSize, &strPathname[0]);
		// Remove the string end
		strPathname = strPathname.substr(0, strPathname.size() - 1);

		strPathname += "\\" + strFileName;
	}
    
	if(strPathname.empty())
	{
		assert(0);
		return E_INVALIDARG;
	}

    // Get the filename extension
	string::size_type ExtensionPos = strPathname.find_last_of(".");
	if (ExtensionPos == string::npos)
		return E_NOTIMPL;

    SAFE_RELEASE(m_pTexture);

	strExtension = strPathname.substr(ExtensionPos, strPathname.size());
	if ((strExtension == ".bmp") ||
		(strExtension == ".tga") ||
		(strExtension == ".jpg") ||
		(strExtension == ".dds"))
	{
	    hr = D3DXCreateTextureFromFileEx( pDev,
			strPathname.c_str(), 
			D3DX_DEFAULT,
			D3DX_DEFAULT,
			dwMipLevels,
			0,
			d3dFormat,
			D3DPOOL_MANAGED,
			D3DX_FILTER_LINEAR,
			D3DX_FILTER_LINEAR,
			0,
			NULL,
			NULL,
			&m_pTexture);
		if (FAILED(hr))
			return hr;

	}
	else
	{
	    // Can add code here to check for other file formats before failing
		assert(0);
		return E_NOTIMPL;
	}

	// Set the caller specified format
	if( d3dFormat != D3DFMT_UNKNOWN )
		m_d3dFormat = d3dFormat;

	return S_OK;
}

HRESULT NVCubeTexture::CreateFromFile(LPDIRECT3DDEVICE8 pDev, const std::string& strFileName, D3DFORMAT d3dFormat)
{
	HRESULT hr;
	string strExtension;
    string strPathname;

	std::string::size_type Pos = strFileName.find_last_of("\\", strFileName.size());
	if (Pos != strFileName.npos)
	{
		// Check the root directory of the running process
		strPathname = strFileName.substr(0, Pos);
	}
	else
	{
		strPathname = ".\\" + strFileName;
	}

	if(strPathname.empty())
	{
		assert(0);
		return E_INVALIDARG;
	}
	
    // Get the filename extension
	string::size_type ExtensionPos = strPathname.find_last_of(".");
	if (ExtensionPos == string::npos)
		return E_NOTIMPL;

	SAFE_RELEASE(m_pCubeTexture);

	// Cubemaps can only be loaded from files if they are in .dds format
	strExtension = strPathname.substr(ExtensionPos, strPathname.size());
	if (strExtension == ".dds")
	{

	    hr = D3DXCreateCubeTextureFromFileExA( pDev,
			strPathname.c_str(), 
			D3DX_DEFAULT,
			0,
			0,
			d3dFormat,
			D3DPOOL_MANAGED,
			D3DX_FILTER_LINEAR,
			D3DX_FILTER_LINEAR,
			0,
			NULL,
			NULL,
			&m_pCubeTexture);

		if (FAILED(hr))
			return hr;
	}
	else
	{
	    // Can add code here to check for other file formats before failing
		assert(0);
		return E_NOTIMPL;
	}

	// Set the caller specified format
	if( d3dFormat != D3DFMT_UNKNOWN )
		m_d3dFormat = d3dFormat;

	return S_OK;
}


}; // nv_objects





