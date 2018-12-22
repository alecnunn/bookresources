// IawRenderTarget.cpp App Wizard Version 2.0 Beta 1
// ----------------------------------------------------------------------
// 
// Copyright © 2001 Intel Corporation
// All Rights Reserved
// 
// Permission is granted to use, copy, distribute and prepare derivative works of this 
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  This software is provided "AS IS." 
//
// Intel specifically disclaims all warranties, express or implied, and all liability,
// including consequential and other indirect damages, for the use of this software, 
// including liability for infringement of any proprietary rights, and including the 
// warranties of merchantability and fitness for a particular purpose.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.

// ----------------------------------------------------------------------
//
// PURPOSE:
//    
// IawRenderTarget.cpp: implementation of the CIawRenderTarget class
//
// ----------------------------------------------------------------------
// Authors: Kim Pallister,Dean Macri - Intel Technology Diffusion Team
// ----------------------------------------------------------------------

#include "..\IawFrameWork.h"

// Constructor
IawRenderTarget::IawRenderTarget(IawD3dWrapper* pWrapper) : IawTexture(pWrapper)
{
  mpZBuffer = NULL;
}

// Destructor
IawRenderTarget::~IawRenderTarget()
{
  SAFE_RELEASE(mpZBuffer);
}

// Select texture parameters
HRESULT IawRenderTarget::SelectParameters(D3DFORMAT& rFormat,
                                          D3DPOOL& rPool,
                                          UINT& rLevels,
                                          UINT& rUsage)
{
  int formats = 0;
  D3DFORMAT* d3d_formats = new D3DFORMAT[IawD3dWrapper::MAX_TEXTURE_FORMAT_TRIES];
  int idx;
  
  rLevels = 1;
  rUsage = D3DUSAGE_RENDERTARGET;
  rPool = D3DPOOL_DEFAULT;

  int iUsageCode = 0;
  
  //if ((mHasAlpha) && (mCreateFlags & IAW_TEXTR_32_BITS_PER_PIXEL) && (mCreateFlags & IAW_TEXTR_RENDER_TARGET))
  if (mHasAlpha)
	iUsageCode += 1;
  if (mCreateFlags & IAW_TEXTR_32_BITS_PER_PIXEL)
	iUsageCode += 2;
  if (mCreateFlags & IAW_TEXTR_RENDER_TARGET)
    iUsageCode += 4;

  //TODO: Currently, bump map formats (e.g. U8V8, etc) are not supported. Since this will conflict with some
  // of the above flags, their shouldn't be a full set of 16 permutations if/when that flag is added.

  //Note most common formats are being tested for here. Some of those ommitted include RGB 332, XRGB 4444, A8, etc)

  switch (iUsageCode) {
//  case 0: // Alpha: NO --- 32bpp: NO --- RenderTarget: NO
//    ;
//	break;
  case 1: // Alpha: YES --- 32bpp: NO --- RenderTarget: NO
    d3d_formats[formats++] = D3DFMT_A4R4G4B4;
    d3d_formats[formats++] = D3DFMT_A1R5G5B5;
    d3d_formats[formats++] = D3DFMT_A8R8G8B8;
    d3d_formats[formats++] = D3DFMT_A8R3G3B2;
    d3d_formats[formats++] = D3DFMT_R5G6B5;
    d3d_formats[formats++] = D3DFMT_X1R5G5B5;
	d3d_formats[formats++] = D3DFMT_X8R8G8B8;
	d3d_formats[formats++] = D3DFMT_X4R4G4B4;
    d3d_formats[formats++] = D3DFMT_R8G8B8;	
  	break;
  case 2: // Alpha: NO --- 32bpp: YES --- RenderTarget: NO
    d3d_formats[formats++] = D3DFMT_X8R8G8B8;
	d3d_formats[formats++] = D3DFMT_A8R8G8B8;
    d3d_formats[formats++] = D3DFMT_R8G8B8;
    d3d_formats[formats++] = D3DFMT_R5G6B5;
    d3d_formats[formats++] = D3DFMT_X1R5G5B5;
	d3d_formats[formats++] = D3DFMT_X4R4G4B4;
    d3d_formats[formats++] = D3DFMT_A1R5G5B5;
    d3d_formats[formats++] = D3DFMT_A4R4G4B4;
    d3d_formats[formats++] = D3DFMT_A8R3G3B2;
    break;
  case 3: // Alpha: YES --- 32bpp: YES --- RenderTarget: NO (The former gets 
	      // priority, better to get alpha and less bits than no alpha)
    d3d_formats[formats++] = D3DFMT_A8R8G8B8;
	d3d_formats[formats++] = D3DFMT_A4R4G4B4;
	d3d_formats[formats++] = D3DFMT_A1R5G5B5;
    d3d_formats[formats++] = D3DFMT_A8R3G3B2;
	d3d_formats[formats++] = D3DFMT_X8R8G8B8;
	d3d_formats[formats++] = D3DFMT_R8G8B8;
    d3d_formats[formats++] = D3DFMT_R5G6B5;
    d3d_formats[formats++] = D3DFMT_X1R5G5B5;
	d3d_formats[formats++] = D3DFMT_X4R4G4B4;
    
    break;
  case 4: // Alpha: NO --- 32bpp: NO --- RenderTarget: YES
	d3d_formats[formats++] = D3DFMT_R5G6B5;
	d3d_formats[formats++] = D3DFMT_X1R5G5B5;
	d3d_formats[formats++] = D3DFMT_X8R8G8B8;
    d3d_formats[formats++] = D3DFMT_A8R8G8B8;
	d3d_formats[formats++] = D3DFMT_A4R4G4B4;
	d3d_formats[formats++] = D3DFMT_A1R5G5B5;
    d3d_formats[formats++] = D3DFMT_A8R3G3B2;
	d3d_formats[formats++] = D3DFMT_X4R4G4B4;
	d3d_formats[formats++] = D3DFMT_R8G8B8;
    break;
  case 5: // Alpha: YES --- 32bpp: NO --- RenderTarget: YES
	d3d_formats[formats++] = D3DFMT_A8R8G8B8;
	d3d_formats[formats++] = D3DFMT_A4R4G4B4;
	d3d_formats[formats++] = D3DFMT_A1R5G5B5;
    d3d_formats[formats++] = D3DFMT_A8R3G3B2;
	d3d_formats[formats++] = D3DFMT_R5G6B5;
	d3d_formats[formats++] = D3DFMT_X1R5G5B5;
	d3d_formats[formats++] = D3DFMT_X8R8G8B8;
	d3d_formats[formats++] = D3DFMT_X4R4G4B4;
    d3d_formats[formats++] = D3DFMT_R8G8B8;
    break;
  case 6: // Alpha: NO --- 32bpp: YES --- RenderTarget: YES
	d3d_formats[formats++] = D3DFMT_X8R8G8B8;
    d3d_formats[formats++] = D3DFMT_R5G6B5;
	d3d_formats[formats++] = D3DFMT_X1R5G5B5;
	d3d_formats[formats++] = D3DFMT_A8R8G8B8;
	d3d_formats[formats++] = D3DFMT_A4R4G4B4;
	d3d_formats[formats++] = D3DFMT_A1R5G5B5;
    d3d_formats[formats++] = D3DFMT_A8R3G3B2;
	d3d_formats[formats++] = D3DFMT_X4R4G4B4;
	d3d_formats[formats++] = D3DFMT_R8G8B8;
    break;
  case 7: // Alpha: YES --- 32bpp: YES --- RenderTarget: YES
	d3d_formats[formats++] = D3DFMT_A8R8G8B8;
	d3d_formats[formats++] = D3DFMT_A4R4G4B4;
	d3d_formats[formats++] = D3DFMT_A1R5G5B5;
    d3d_formats[formats++] = D3DFMT_A8R3G3B2;
	d3d_formats[formats++] = D3DFMT_X8R8G8B8;
    d3d_formats[formats++] = D3DFMT_R5G6B5;
	d3d_formats[formats++] = D3DFMT_X1R5G5B5;
	d3d_formats[formats++] = D3DFMT_X4R4G4B4;
	d3d_formats[formats++] = D3DFMT_R8G8B8;
    break;
  default: // Alpha: NO --- 32bpp: NO --- RenderTarget: NO
    d3d_formats[formats++] = D3DFMT_R5G6B5;
	d3d_formats[formats++] = D3DFMT_X1R5G5B5;
	d3d_formats[formats++] = D3DFMT_A1R5G5B5;
	d3d_formats[formats++] = D3DFMT_X4R4G4B4;
	d3d_formats[formats++] = D3DFMT_A4R4G4B4;
	d3d_formats[formats++] = D3DFMT_X8R8G8B8;
	d3d_formats[formats++] = D3DFMT_A8R8G8B8;
	d3d_formats[formats++] = D3DFMT_A8R3G3B2;
	d3d_formats[formats++] = D3DFMT_R8G8B8;
    break;
  }


  // Check for a supported format
  idx = 0;
  D3DDEVICE_CREATION_PARAMETERS d3dCP;
  D3DDISPLAYMODE d3dMode;

  mpWrapper->mpDevice->GetCreationParameters(&d3dCP);
  mpWrapper->mpDevice->GetDisplayMode(&d3dMode);
  while((idx < formats) && (FAILED(mpWrapper->mpDirectX8->CheckDeviceFormat(
    d3dCP.AdapterOrdinal,
    d3dCP.DeviceType,
    d3dMode.Format,
    rUsage,
    D3DRTYPE_TEXTURE,
    d3d_formats[idx]))))
  {
    idx++;
  }
  
  if(idx == formats)
  {
    delete [] d3d_formats;
    return D3DERR_NOTAVAILABLE;
  }
  
  rFormat = d3d_formats[idx];
  
  delete [] d3d_formats;
  
  return S_OK;
}


HRESULT IawRenderTarget::CreateAttachedSurfaces()
{
  HRESULT hr;
  D3DSURFACE_DESC surface_desc;
  int iZFormats = 0;
  D3DFORMAT* zformats = new D3DFORMAT[IawD3dWrapper::MAX_TEXTURE_FORMAT_TRIES];
  int idx = 0;
  D3DDEVICE_CREATION_PARAMETERS d3d_cp;
  D3DDISPLAYMODE d3d_mode;
  
  SAFE_RELEASE(mpZBuffer);
  
  mpTexture->GetLevelDesc(0, &surface_desc);
  
  mpZBuffer = NULL;
  mpWrapper->mpDevice->GetCreationParameters(&d3d_cp);
  mpWrapper->mpDevice->GetDisplayMode(&d3d_mode);
  if(mCreateFlags & IAW_TEXTR_32_BITZ)
  {
    zformats[iZFormats++] = D3DFMT_D32;
	zformats[iZFormats++] = D3DFMT_D24X8;
    zformats[iZFormats++] = D3DFMT_D24S8;
    zformats[iZFormats++] = D3DFMT_D24X4S4;    
    zformats[iZFormats++] = D3DFMT_D16;
    zformats[iZFormats++] = D3DFMT_D15S1;
    //zformats[iZFormats++] = D3DFMT_D16_LOCKABLE;
    
    
    while((idx < iZFormats) && (FAILED(mpWrapper->mpDirectX8->CheckDepthStencilMatch(
      d3d_cp.AdapterOrdinal,
      d3d_cp.DeviceType,
      mpWrapper->mD3dFormat,
      surface_desc.Format,
      zformats[idx])
      )))
    {
      idx++;
    }			
    hr = mpWrapper->mpDevice->CreateDepthStencilSurface(surface_desc.Width, surface_desc.Height, 
      zformats[idx], D3DMULTISAMPLE_NONE, &mpZBuffer);
    if(FAILED(hr))
    {
      delete [] zformats;
      return hr;
    }
  }
  else if(mCreateFlags & IAW_TEXTR_16_BITZ)
  {
    zformats[iZFormats++] = D3DFMT_D16 ;
    zformats[iZFormats++] = D3DFMT_D15S1;
    //zformats[iZFormats++] = D3DFMT_D16_LOCKABLE;
    zformats[iZFormats++] = D3DFMT_D32;
	zformats[iZFormats++] = D3DFMT_D24X8;
    zformats[iZFormats++] = D3DFMT_D24S8;    
    zformats[iZFormats++] = D3DFMT_D24X4S4;
    
    while((idx < iZFormats) && (FAILED(mpWrapper->mpDirectX8->CheckDepthStencilMatch(
      d3d_cp.AdapterOrdinal,
      d3d_cp.DeviceType,
      mpWrapper->mD3dFormat,
      surface_desc.Format,
      zformats[idx])
      )))
    {
      idx++;
    }
    hr = mpWrapper->mpDevice->CreateDepthStencilSurface(surface_desc.Width, surface_desc.Height, 
      D3DFMT_D16, D3DMULTISAMPLE_NONE, &mpZBuffer);
    if(FAILED(hr))
    {
      delete [] zformats;
      return hr;
    }
  }
  
  delete [] zformats;
  
  return S_OK;
}



HRESULT IawRenderTarget::BeginScene()
{
  D3DVIEWPORT8 temp_viewport;
  D3DSURFACE_DESC surface_desc;
  LPDIRECT3DSURFACE8 tex_surface = NULL;
  HRESULT hr;

  if (mpTexture == NULL) //then the texture was never init'd correctly
  {
    return E_FAIL;
  }

  hr = mpWrapper->mpDevice->GetRenderTarget(&mpSavedRenderTarget);
  hr = mpWrapper->mpDevice->GetDepthStencilSurface(&mpSavedZBuffer);
  
  mpWrapper->mpDevice->GetViewport(&mVpOld);

  memcpy(&temp_viewport, &mVpOld, sizeof(D3DVIEWPORT8));
  mpTexture->GetLevelDesc(0, &surface_desc);
  
  temp_viewport.Width = surface_desc.Width;
  temp_viewport.Height = surface_desc.Height;

  mpTexture->GetSurfaceLevel(0, &tex_surface);
  hr = mpWrapper->mpDevice->SetRenderTarget(tex_surface, mpZBuffer);
  SAFE_RELEASE(tex_surface);

  //Set the right size VP
  mpWrapper->mpDevice->SetViewport(&temp_viewport);

  //Clear the texture surface.
  mpWrapper->mpDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

  //Start a scene
  mpWrapper->mpDevice->BeginScene();

  return S_OK;
}

HRESULT IawRenderTarget::EndScene()
{
  if (mpTexture == NULL) //texture was never init'd correctly, BS() would have failed
  {
    return E_FAIL;
  }

  mpWrapper->mpDevice->EndScene();

  //set back to previous render target and z-buffer
  mpWrapper->mpDevice->SetRenderTarget(mpSavedRenderTarget, mpSavedZBuffer);
  mpWrapper->mpDevice->SetViewport(&mVpOld);

  SAFE_RELEASE(mpSavedRenderTarget);
  SAFE_RELEASE(mpSavedZBuffer);

  return S_OK;
}


