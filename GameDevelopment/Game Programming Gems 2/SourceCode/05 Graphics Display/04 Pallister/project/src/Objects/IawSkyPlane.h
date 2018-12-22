// IawSkyPlane.h App Wizard Version 2.0 Beta 1
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

#if !defined(IawSkyPlane_h)
#define IawSkyPlane_h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
 * A sky plane object.
 * A sky plane is a plane whose vertices are pulled down over a sphere to match
 * the curvature. Useful for things like a dome.
 */
class IawSkyPlane : public IawObject
{
public:
  /** Constructor */
  IawSkyPlane(DWORD flags = DEFAULTFVF);

  /** Destructor */
  virtual ~IawSkyPlane();

  /**
   * Initialize a sky plane.
   */
  HRESULT Init(IawD3dWrapper* pWrapper,
               int divisions = 10,
               float planetRadius = 100.0f,
               float atmosphereRadius = 120.0f,
               float hTileFactor = 1.0f,
               float vTileFactor = 1.0f);
/*               IawVector pos = IawVector(0.0f),
               IawVector scale = IawVector(1.0f),
               IawVector rotationAxis = IawVector(0.0f,1.0f,0.0f),
               float rotationAngle = 0.0f,
               IawVector velocity = IawVector(0.0f),
               IawVector scaleVelocity = IawVector(0.0f),
               float rotationalVel = 0.0f,
               DWORD flags = 0);
*/
private:
  float mPlanetRadius;
  float mAtmosphereRadius;
  float mHTileFactor;
  float mVTileFactor;

  HRESULT SetupVertices(float *pVertices =NULL);
  HRESULT SetupIndices();
};

#endif // IawSkyPlane_h
