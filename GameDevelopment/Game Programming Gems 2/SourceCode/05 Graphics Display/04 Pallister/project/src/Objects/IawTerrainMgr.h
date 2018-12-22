// IawTerrainMgr.h App Wizard Version 2.0 Beta 1
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
#if !defined(IawTerrainMgr_h)
#define IawTerrainMgr_h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define TERRAIN_NUM_DIVISIONS 10

/**
 * A terrain manager.
 * This class handles the details of managing multiple terrain objects.
 */
class IawTerrainMgr
{
public:
  IawTerrainMgr(DWORD flags = DEFAULTFVF);
  ~IawTerrainMgr();

  HRESULT Init(IawD3dWrapper* pWrapper, IawVector pos , IawVector scale, float horizondistance);
  HRESULT Update(float frameTime, IawVector viewPos, IawVector viewDir);
  HRESULT Render(IawMatrix worldMat);
  HRESULT TargetChanged(IawD3dWrapper* pWrapper, IawShaderMgr* pShaderMgr);
/*
  float GetTerrainHeight(float x, float y, float z);
  float GetTerrainExp(float x, float y, float z);
  float GetTerrainAmp(float x, float y, float z);
*/

/*
  HRESULT SetGlobalLod(int LOD);
  bool mDynamicLod;
*/

  inline SetTerrainShaderID(int shaderId) { mTerrainShaderId = shaderId;}
  inline SetWaterShaderID(int shaderId) { mWaterShaderId = shaderId;}

  bool mUseCulling; //just for testing

  IawVector mPos;
  IawVector mAbsPos;

  IawVector mViewPosition;
  IawVector mViewDirection;

  //made these public so I could access them for test purposes
  // make private later
/*
  IawTexture *mpTerrainTexture;
  IawTexture *mpDetailTexture;
  IawTexture *mpMaskTexture;//, *mpMaskTextureSysmem;
  IawTexture *mpTerrainAndMaskTexture;//, *mpTerrainAndMaskTextureSysmem;

  IawTexture *mpCloudTexture;

  bool mShowTextures;
*/

  int mCurrentUpdateNumH;
  int mCurrentUpdateNumV;
  float mShiftX;
  float mShiftZ;

/*
  DWORD mFogColor;
*/
  static const int TERRAIN_MAX_HEIGHT;
  static const int TERRAIN_NUM_TESSELATIONS;

  //@{
  /** Procedural world parameter. */
  static const int EARTH_RADIUS;
  static const int CLOUD_HEIGHT;
  static const int HORIZON;
  //@}

private:
  HRESULT InitTerrainArray(); //this should get a random number at some time

/*
  HRESULT TestLODDistance(int row, int col);
*/

/*
  HRESULT RestoreTerrainNoiseTexture(CDRGWrapper *pWrapper);
  HRESULT RestoreTerrainBaseTexture(CDRGWrapper *pWrapper);
  HRESULT RestoreMaskTexture(CDRGWrapper *pWrapper);
  HRESULT RestoreTerrainAndMaskTexture(CDRGWrapper *pWrapper);
*/

  IawVector mScale;
  IawMatrix mLocalToWorldMx;
  
  //IawVector mViewPosition;
  //IawVector mViewDirection;
  float mHorizon;
  float mMaxHeight;
/*
  float   mWaterAltitude;
  float   mSnowAltitude;
*/

  IawD3dWrapper* mpWrapper;
  IawShaderMgr* mpShaderMgr;
  //int mShaderId;

  IawTerrain* mpTerrainArray/*[NUM_TERRAIN_LODS]*/[TERRAIN_NUM_DIVISIONS][TERRAIN_NUM_DIVISIONS];
  IawObject*  mpRootObjectArray/*[NUM_TERRAIN_LODS]*/[TERRAIN_NUM_DIVISIONS][TERRAIN_NUM_DIVISIONS];

  DWORD mFVF;

  //made these public so I could access them for test purposes
  // make private later
  //IawTexture* mpTerrainTexture;
  //IawTexture* mpDetailTexture;

  int mTerrainShaderId;
  int mWaterShaderId;
};

#endif // IawTerrainMgr_h
