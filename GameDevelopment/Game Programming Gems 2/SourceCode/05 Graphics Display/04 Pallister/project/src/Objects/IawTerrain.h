// IawTerrain.h App Wizard Version 2.0 Beta 1
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

#if !defined(IawTerrain_h)
#define IawTerrain_h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/** Terrain FVF */
#define TERRAINFVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2)

/** Draw the normals of a terrain patch */
///#define DRAW_NORMALS

#ifdef DRAW_NORMALS
/**
 * This is equivalent of D3DLvert, but we don't really need tex coords, so
 * could reduce later.
 */
typedef struct tagNormalVert
{
  D3DVALUE x;
  D3DVALUE y;
  D3DVALUE z;
  DWORD color;
  DWORD specular;
  D3DVALUE tu1;
  D3DVALUE tv1;
} NormalVert;
#endif // DRAW_NORMALS

/**
 * A single terrain object.
 */
class IawTerrain : public IawObject
{
public:
  /**
   * Constructor.
   * @param flags FVF for the terrain object.
   * @param seed A value for the psuedo-random number generator.
   */
  IawTerrain(DWORD flags =DEFAULTFVF, long seed = 0);

  /** Destructor */
  virtual ~IawTerrain();

  /** Initialize a terrain object */
  HRESULT Init(IawD3dWrapper* pWrapper,
               int divisions = 1);
/*               IawVector pos = IawVector(0.0f),
               IawVector scale = IawVector(1.0f),
               IawVector rotationAxis = IawVector(0.0f,1.0f,0.0f),
               float rotationAngle = 0.0f,
               IawVector velcoity = IawVector(0.0f),
               IawVector scaleVelocity = IawVector(0.0f),
               float rotationalVel = 0.0f,
               DWORD flags = 0);
*/
#ifdef DRAW_NORMALS

  /**
   * Render the terrain object.
   * This method is only used when the Display List Manger is <b>not</b>
   * included, and DRAW_NORMALS is defined.
   */
  HRESULT Render(IawMatrix& rWorldMat, bool renderChildren = true);

  /**
   * Update the terrain object because the render target changed.
   * This is only used when DRAW_NORMALS is defined.
   */
  HRESULT TargetChanged(IawD3dWrapper* pWrapper, bool updateChildren = true);
#endif // DRAW_NORMALS

  //@{
  /** Access method */
  void SetUpdate(bool update) { mUpdate = update; }
  bool GetUpdate() { return mUpdate; }

  void SetWater(bool water) { mHasWater = water; }
  bool GetWater() { return mHasWater; }

  void SetMaxHeight(float maxHeight) { mMaxHeight = maxHeight; }
  float GetMaxHeight() { return mMaxHeight; }
  //@}

  IawVector mObserverPos; /**< Observer position */
  IawVector mTerrainPos;  /**< Terrain position */
private:
  HRESULT SetupVertices(float* pVertices =NULL);
  HRESULT SetupIndices();

  HRESULT GenerateTerrain();

  float mMaxHeight;

  IawExponentialNoise* mNoiseGenerator;
  float* mpHeightField;

  float mTerrainScale;
  IawVector mScale2;
  int mNumDivisions, mGridDim;

  bool mHasWater;
  bool mUpdate;

#ifdef DRAW_NORMALS
  LPDIRECT3DVERTEXBUFFER8 mpNormalVB;
  NormalVert* mpNormalVerts;
#endif
};

#endif //IawTerrain_h
