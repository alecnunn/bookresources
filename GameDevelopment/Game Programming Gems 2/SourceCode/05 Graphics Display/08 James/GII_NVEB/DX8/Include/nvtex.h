/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#ifndef __NVTEX_H_
#define __NVTEX_H_

#include "nvinc.h"
#include <nvdevice.h>

namespace nv_objects
{

class NVBaseTexture
{
public:
	NVBaseTexture();

	DWORD GetWidth() const { return m_dwWidth; }
    DWORD GetHeight() const { return m_dwHeight; }
	D3DFORMAT GetFormat() const { return m_d3dFormat; }

protected:
    DWORD					m_dwWidth;      // Width of texture image file
    DWORD					m_dwHeight;     // Height of texture image file
	D3DFORMAT				m_d3dFormat;    // Format of texture
};

class NVTexture : public NVBaseTexture
{
public:
	NVTexture();
	~NVTexture();

	virtual HRESULT CreateFromFile(LPDIRECT3DDEVICE8 pDev, const std::string& strName, D3DFORMAT d3dFormat=D3DFMT_UNKNOWN, DWORD dwMipLevels = 0);
	virtual HRESULT CreateTexture(LPDIRECT3DDEVICE8 pD3DDev, DWORD dwWidth, DWORD dwHeight, WORD dwMipmaps, D3DFORMAT d3dFormat = D3DFMT_UNKNOWN);

	LPDIRECT3DTEXTURE8 GetTexture() const
	{
		return m_pTexture;
	}

protected:
	LPDIRECT3DTEXTURE8		m_pTexture;     // The d3d texture object
};

class NVCubeTexture : public NVBaseTexture
{
public:
	NVCubeTexture();
	~NVCubeTexture();

	virtual HRESULT CreateFromFile(LPDIRECT3DDEVICE8 pDev, const std::string& strName, D3DFORMAT d3dFormat=D3DFMT_UNKNOWN);
	virtual HRESULT CreateCubeTexture(LPDIRECT3DDEVICE8 pD3DDev, DWORD dwWidth, WORD dwMipmaps, D3DFORMAT d3dFormat = D3DFMT_UNKNOWN);
	virtual HRESULT CreateNormalizationCubeMap(LPDIRECT3DDEVICE8 pD3DDev, DWORD dwWidth = 256, WORD dwMipmaps = 1);

	LPDIRECT3DCUBETEXTURE8 GetTexture() const
	{
		return m_pCubeTexture;
	}

protected:
	LPDIRECT3DCUBETEXTURE8	m_pCubeTexture; // The d3d texture object for a cubemap

};

class NVHILOTexture : public NVBaseTexture
{
public:
	NVHILOTexture();
	~NVHILOTexture();

	virtual HRESULT CreateTexture(LPDIRECT3DDEVICE8 pD3DDev, DWORD dwWidth, DWORD dwHeight, WORD dwMipmaps);
	
	LPDIRECT3DTEXTURE8 GetTexture() const
	{
		return m_pHILOTexture;
	}

protected:
	LPDIRECT3DTEXTURE8	m_pHILOTexture; // The d3d texture object for a cubemap

};



}; // nv_objects

#endif // D3DTEX_H

