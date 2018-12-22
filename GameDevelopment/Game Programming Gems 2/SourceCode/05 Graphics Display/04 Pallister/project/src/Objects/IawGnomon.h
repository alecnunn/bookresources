// IawGnomon.h App Wizard Version 2.0 Beta 1
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

#if !defined(IawGnomon_h)
#define IawGnomon_h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
 * This class is used to contain an XYZ axis, stored and drawn as a series of
 * line segments.
 */
class IawGnomon : public IawObject
{
public:
  /**
   * Constructor.
   * @param flags FVF for this object.
   */
  IawGnomon(DWORD flags =/*D3DFVF_LVERTEX*/(D3DFVF_XYZ | D3DFVF_DIFFUSE | \
    D3DFVF_SPECULAR | D3DFVF_TEX1));

  /** Destructor */
  virtual ~IawGnomon();

  /**
   * Initialize a Gnomon object.
   */
  HRESULT Init(IawD3dWrapper* pWrapper);
/*               IawVector pos = IawVector(0.0f),
               IawVector scale = IawVector(1.0f),
               IawVector rotationAxis  = IawVector(0.0f, 1.0f, 0.0f),
               float rotationAngle = 0.0f,
               IawVector velocity = IawVector(0.0f),
               IawVector scaleVelocity = IawVector(0.0f),
               float rotationalVel = 0.0f,
               DWORD flags = 0);
*/
  /**
   * Render an object.
   * This is used when  the Display List Manager is <b>not</b> included.
   */
  HRESULT Render(IawMatrix& rWorldMat, bool renderChildren = true);

  /**
   * Get the triangle count.
   */
  int GetTriangleCount() { return 0; }

private:
  HRESULT SetupVertices(float* pVertices = NULL);
  HRESULT SetupIndices();
};


#endif // IawGnomon_h
