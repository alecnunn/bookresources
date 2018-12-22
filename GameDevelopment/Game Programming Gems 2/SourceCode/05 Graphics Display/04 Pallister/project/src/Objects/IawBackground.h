// IawBackground.h App Wizard Version 2.0 Beta 1
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

#if !defined(IawBackground_h)
#define IawBackground_h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
 * A background object.
 * This is a background for your scene.  It takes into account windowsize upon
 * initialization.
 */
class IawBackground : public IawObject
{
public:
  /** Constructor */
  IawBackground(DWORD flags = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1);

  /** Destructor */
  virtual ~IawBackground();

  /** Initialize the background plane. */
  HRESULT Init(IawD3dWrapper* pWrapper = NULL);
private:
  // Set up background plane vertices, takiong into account window size
  HRESULT SetupVertices(float* pVertices = NULL);
  HRESULT SetupIndices();
};

#endif // IawBackground_h

