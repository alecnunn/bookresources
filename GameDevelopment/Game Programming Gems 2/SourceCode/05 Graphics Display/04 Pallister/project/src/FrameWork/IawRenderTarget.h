// IawRenderTarget.h App Wizard Version 2.0 Beta 1
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
// Authors: Kim Pallister,Dean Macri - Intel Technology Diffusion Team
// ----------------------------------------------------------------------

#if !defined(RenderTarget_h)
#define RenderTarget_h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
 * A texture class which allows for the texture to be the render target.
 */
class IawRenderTarget : public IawTexture
{
public:
  /**
   * Constructor.
   * @param pWrapper A Direct3D wrapper
   */
  IawRenderTarget(IawD3dWrapper* pWrapper);

  /** Destructor */
  virtual ~IawRenderTarget();

  /** Begin a scene for rendering */
  HRESULT BeginScene();

  /** End a scene */
  HRESULT EndScene();

  /** A Direct3D Surface Z-buffer */
  LPDIRECT3DSURFACE8 mpZBuffer;

protected:
  virtual HRESULT SelectParameters(D3DFORMAT& rFormat,
                                   D3DPOOL& rPool,
                                   UINT& rLevels,
                                   UINT& rUsage);
  virtual HRESULT CreateAttachedSurfaces();

  D3DVIEWPORT8 mVpOld;
  LPDIRECT3DSURFACE8 mpSavedRenderTarget;
  LPDIRECT3DSURFACE8 mpSavedZBuffer;
};

#endif // !defined(RenderTarget_h)

