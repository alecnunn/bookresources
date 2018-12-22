// IawCamera.h App Wizard Version 2.0 Beta 1
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

#if !defined(IawCamera_h)
#define IawCamera_h

/**
 * A camera object class.
 */
class IawCamera : public IawObject
{
public:
  /** Constructor */
  IawCamera(DWORD flags = DEFAULTFVF);

  /** Destructor */
  virtual ~IawCamera();

  /**
   * Initialize a Camera object.
   * Default parameters are for object having no velocity, and centered.
   */
  HRESULT Init(IawD3dWrapper* pWrapper);
/*
               IawVector pos = IawVector(0.0f),
               IawVector scale = IawVector(1.0f),
               IawVector rotationaxis = IawVector(0.0f,1.0f,0.0f),
               float rotationangle = 0.0f,
               IawVector vel = IawVector(0.0f),
               IawVector scalevel = IawVector(0.0f),
               float rotationalvel = 0.0f,
               DWORD dwFlags = 0);
*/

  //HRESULT SetViewMatrix(IawMatrix mxView);

private:
  HRESULT SetupVertices(float* pVertices = NULL);
  HRESULT SetupIndices();

  //some new private member vars
  IawMatrix mViewMat;
  IawMatrix mProjectionMat;
  
  IawVector mViewDirection;
};

#endif // IawCamera_h

