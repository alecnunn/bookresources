// ProceduralCloudDemo.h App Wizard Version 2.0 Beta 1
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

#if !defined(ProceduralCloudDemo_h)
#define ProceduralCloudDemo_h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
 * This is the main application class.
 */
class ProceduralCloudDemo
{
public:
  ProceduralCloudDemo();
  virtual ~ProceduralCloudDemo();

  virtual HRESULT InitWorld(IawD3dWrapper* pWrapper);	// called at startup
  virtual HRESULT DestroyWorld();						// called at teardown	
  virtual HRESULT UpdateWorld();						// called per frame
  virtual HRESULT RenderWorld();						// called per frame
  virtual HRESULT TargetChanging();						// called at device change
  virtual HRESULT TargetChanged();						// called at device change
  virtual HRESULT SetupShaders();						// called at device change


  // called as they occur
  void KeyDown(WPARAM wParam);
  void KeyUp(WPARAM wParam);
  void MouseLeftUp(WPARAM wParam, LPARAM lParam);
  void MouseLeftDown(WPARAM wParam, LPARAM lParam);
  void MouseRightUp(WPARAM wParam, LPARAM lParam);
  void MouseRightDown(WPARAM wParam, LPARAM lParam);
  void MouseMove(WPARAM wParam, LPARAM lParam);

  
private:
  /**
   * This is a wrapper for the graphics device
   */
  IawD3dWrapper* mpWrapper;

  IawMatrix mWorldMat;       /**< This is the world matrix */
  IawMatrix mProjectionMat;  /**< This is the projection matrix */
  IawMatrix mViewMat;        /**< This is the view matrix */

  /**
   * This is the container that will hold all our shaders.
   */
  IawShaderMgr* mpShaderManager;

  /*
   * Objects in the world 
   */
  IawObject* mpRootObj;			// scene root node
  IawSkyPlane* mpSkyObj;		// geometry we'll map the sky to
  IawTerrain*  mpTerrainObj;		// simple procedural terrain patch
  IawPlane*    mpWaterObj;    // plane we'll shade differently to show water
  
  IawPlane* mpCloudTexturePlane;// plane used when doing the RTT to create
								// the cloud texture
  IawPlane* mpDisplayPlane;		// used for displaying textures for demo/debug    
  
  IawTextMgr* mpFPSTextMgr;     // used for the frame rate
  IawTextMgr* mpHelpTextMgr;    // used for display help info

  
  D3DLIGHT8 mSunLight;				//Used in our scene
  D3DLIGHT8 mCloudMultiplierLight;  //Used as part of the cloud texture creation


  /**
   * Textures
   */
  IawTexture* mpFontTexture;			// gets handed to the two text managers
  IawTexture*  mpTerrainBaseTexture;	// we load a bitmap for this

  RandTexture* mpRandTextureGenerator;				// Classes that creates & 
													// updates an animated 
													// sum-of-four-octaves 
													// turbulence texture. 

  StaticRandTexture* mpStaticRandTextureGenerator; 	// Same as above, but not 
													// animated 
													

  IawRenderTarget* mpCloudTexture;      // used to create the cloud texture from
										// Noise texture generator above
  
  
  // Some vars we'll need.
  float mTimeSinceLastFpsUpdate; //only update the counter every second
  float mfCloudAmount;			 //amount of clouds, how many/how isolated
  
  int mMousePrevX;
  int mMousePrevY;
};

#endif // !defined(ProceduralCloudDemo_h)

