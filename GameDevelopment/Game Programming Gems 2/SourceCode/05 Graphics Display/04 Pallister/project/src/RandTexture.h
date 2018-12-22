// RandTexture.h App Wizard Version 2.0 Beta 1
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

#if !defined(RandTexture_h)
#define RandTexture_h

#if !defined(NUMOCTAVES)
#define NUMOCTAVES 4
#endif

/**
 * This class manages constantly changing "random" textures
 */
class RandTexture
{
public:
  RandTexture();
  virtual ~RandTexture();

  virtual HRESULT InitWorld(IawD3dWrapper* pWrapper);
//  virtual HRESULT DestroyWorld();
  virtual HRESULT UpdateWorld();
  virtual HRESULT RenderWorld();
  virtual HRESULT TargetChanging();
  virtual HRESULT TargetChanged();
  virtual HRESULT SetupShaders();

  virtual HRESULT InitNoiseOctaves(/*int iNumOctaves*/);
  virtual HRESULT UpdateNoiseOctave(int iOctave);
  virtual HRESULT UpdateCompositeNoise(); //adds up the 4 octaves

  virtual HRESULT InitQuads();

  void ToggleUpdateAcceleration();

  void SetShaderMgr(IawShaderMgr* pShaderManager)
  {
    mpShaderManager = pShaderManager;
  }

  IawTexture *GetTurbulenceTexture()
  {
    return mpCompositeNoiseTexture;
  }

private:
  IawD3dWrapper* mpWrapper;
  IawShaderMgr*  mpShaderManager;

  IawPlane* mpRNGQuadArray[NUMOCTAVES];
  IawPlane* mpInterpolatingQuadArray[NUMOCTAVES];
  IawPlane* mpCompositeQuadArray[NUMOCTAVES];

  IawTexture* mpRNGLookupTexture;

  IawRenderTarget* mpNoiseOctaveTextureArray[NUMOCTAVES];
  IawRenderTarget* mpPreviousNoiseOctaveTextureArray[NUMOCTAVES];
  IawRenderTarget* mpInterpolatedNoiseOctaveTextureArray[NUMOCTAVES];
  IawRenderTarget* mpCompositeNoiseTexture;

  float mfNoiseOctaveUpdateRate[NUMOCTAVES];
  float mfNoiseOctavePos[NUMOCTAVES];
  float mfNoiseOctaveSpeed[NUMOCTAVES];
  float mfNoiseOctaveAge[NUMOCTAVES];
  float mfCurrentTime;

  bool  mbAccelerateUpdates;
};

#endif // !defined(RandTexture_h)

