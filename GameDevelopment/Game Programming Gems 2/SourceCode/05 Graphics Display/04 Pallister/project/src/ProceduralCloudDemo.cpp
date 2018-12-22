// ProceduralCloudDemo.cpp - Created with App Wizard Version 2.0 Beta 1
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

#include "stdafx.h"

// Define options for the selected scene



//Need to update this
  #define FOV                  (PI * 0.33f)
  #define DEFAULT_ASPECT_RATIO 1.333f

// you can substitute this remmed out code below to get an 'satellite view' of the scene.
// Just place one slash at the start of the line below.
/*
  #define NEARCLIP             8.0f
  #define FARCLIP              300.0f
  #define CAMERAPOS            IawVector(0, 60, -220)
  #define CAMERATARGET         IawVector(0, 30, 0) 
/*/
  #define NEARCLIP             1.0f
  #define FARCLIP				   200.0f
  #define CAMERAPOS            IawVector(0, 0, -10)
  #define CAMERATARGET         IawVector(0, 0, 0) 
// */


  #define SKYCOLOR RGB_MAKE(0x44, 0x44, 0xff)
  #define FOGCOLOR RGB_MAKE(0x99, 0x99, 0xee)

  #define FOGSTART 40.0f
  #define FOGEND 90.0f

  #define INITIALCLOUDLEVEL 0.2f  


// Constructor
ProceduralCloudDemo::ProceduralCloudDemo()
{
  mpWrapper = NULL;

  mWorldMat.SetIdentity();

  mViewMat.SetView(CAMERAPOS, CAMERATARGET, IawVector(0, 1, 0));
  mProjectionMat.SetProjection(FOV, DEFAULT_ASPECT_RATIO, NEARCLIP, FARCLIP);

  mMousePrevX = 0;
  mMousePrevY = 0;

  mpShaderManager		= NULL;

  mpRootObj				= NULL;
  mpSkyObj		 		= NULL;
  mpTerrainObj			= NULL; 
  mpWaterObj			= NULL; 
  mpDisplayPlane		= NULL;
  mpCloudTexturePlane	= NULL;
  mpFPSTextMgr			= NULL;

  mpFontTexture		    = NULL;
  mpTerrainBaseTexture  = NULL;  
  mpCloudTexture        = NULL;  

  //These two classes contain a bunch of textures, and produce a 'turbulence' 
  // texture. StaticRandTexture is static (used for terrain noise) and 
  // RandTexture is animated, used for the clouds.

  mpRandTextureGenerator = new RandTexture();
  mpStaticRandTextureGenerator = new StaticRandTexture();




  mTimeSinceLastFpsUpdate = 10; //This will force an update.

  mfCloudAmount = INITIALCLOUDLEVEL;
	  


  //setup the two our light structures. One is the sunlight, the other is a light we use
  // to over-brighten one of our blends
  
  ZeroMemory(&mSunLight, sizeof(D3DLIGHT8));

  mSunLight.Specular.a = 1.0f;
  mSunLight.Specular.b = 0.0f;
  mSunLight.Specular.g = 0.0f;
  mSunLight.Specular.r = 0.0f;

  mSunLight.Diffuse.a = 1.0f;
  mSunLight.Diffuse.b = 0.8f;
  mSunLight.Diffuse.g = 0.8f;
  mSunLight.Diffuse.r = 0.8f;

  mSunLight.Ambient.a = 1.0f;
  mSunLight.Ambient.b = 0.2f;
  mSunLight.Ambient.g = 0.2f;
  mSunLight.Ambient.r = 0.2f;

  mSunLight.Type = D3DLIGHT_DIRECTIONAL;

  mSunLight.Direction.x = 1.0f;
  mSunLight.Direction.y = -0.75f;
  mSunLight.Direction.z = 0.5f;

  mSunLight.Position.x = -2.0f;
  mSunLight.Position.y = 0.0f;
  mSunLight.Position.z = 0.0f;

  mSunLight.Range = 1000;
  
  
  ZeroMemory(&mCloudMultiplierLight, sizeof(D3DLIGHT8));

  // This light is used to oversaturate the cloud texture to compensate for 
  //  what we lose with the subtraction we do to clamp the noise to get 
  //  isolated clouds
  
  float factor = mfCloudAmount; 

  // This is to compensate for the fact that we fall short of our 
  //  asymptote since we only use 4 terms
  factor *= 1.0f / (0.5f+0.25f+0.125f+0.0625f);   
  
  mCloudMultiplierLight.Diffuse.a = 0.25f/factor; //0.5 because we'll do a modulate2X
  mCloudMultiplierLight.Diffuse.b = 0.25f/factor;
  mCloudMultiplierLight.Diffuse.g = 0.25f/factor;
  mCloudMultiplierLight.Diffuse.r = 0.25f/factor;

  mCloudMultiplierLight.Type = D3DLIGHT_DIRECTIONAL;

  mCloudMultiplierLight.Direction.x = 0.0f;
  mCloudMultiplierLight.Direction.y = 1.0f;
  mCloudMultiplierLight.Direction.z = 0.0f;

  mCloudMultiplierLight.Position.x = -2.0f;
  mCloudMultiplierLight.Position.y = 0.0f;
  mCloudMultiplierLight.Position.z = 0.0f;

  mCloudMultiplierLight.Range = 1000;

}

// Destructor
ProceduralCloudDemo::~ProceduralCloudDemo()
{

  SAFE_DELETE(mpTerrainBaseTexture);
  SAFE_DELETE(mpCloudTexture);

  SAFE_DELETE(mpRandTextureGenerator);
  SAFE_DELETE(mpStaticRandTextureGenerator);

}

// Init world creates all of the object and texture "containers". 
//  Each of these has device-specific data (e.g An object contains a VB)
//  that is only created later in TargetChanged()
//
//  Init has 3 steps: Create Geometry containers, texture containers, 
//  and finally shader containers

HRESULT ProceduralCloudDemo::InitWorld(IawD3dWrapper* pWrapper)
{
  int i; // used in a couple places
  mpWrapper = pWrapper;

  mpWrapper->SetClearColor(FOGCOLOR);  

  //////////////////////////////////////
  // Create Geometry Containers
  //////////////////////////////////////

  // Create a root node for the scene
  mpRootObj = new IawObject();
  mpRootObj->Init(mpWrapper);
  mpRootObj->SetObjectName("Root Obj");
  
  // A Text manager is just a container for a bunch of strings. The strings are
  //  in made of quads (sitting in a VB). It lets us group together strings
  //  into groups that we can enable or disable at once (as we will do with the 
  //  help text)

  mpFPSTextMgr = new IawTextMgr();
  mpFPSTextMgr->Init(mpWrapper, 0);
  int string_id[4];
  mpFPSTextMgr->AddString("XXX", &string_id[0], 0.05f, 0.9f, 0.035f, 0.06f);
  mpFPSTextMgr->AddString("FPS", &string_id[1], 0.17f, 0.9f, 0.035f, 0.06f);
  mpFPSTextMgr->AddString("Cloud Amount:", &string_id[2], 0.35f, 0.9f, 0.035f, 0.06f);
    
  char string[5];
  sprintf(string, "%01.2f", mfCloudAmount);
  mpFPSTextMgr->AddString(string, &string_id[3], 0.8f, 0.9f, 0.035f, 0.06f);
  

  mpHelpTextMgr = new IawTextMgr();
  mpHelpTextMgr->Init(mpWrapper, 0);
  int string_id2[10];
  mpHelpTextMgr->AddString("H - Toggle Help",				&string_id2[0], 0.05f, 0.05f, 0.03f, 0.06f);
  mpHelpTextMgr->AddString("A - Toggle Time Acceleration",	&string_id2[1], 0.05f, 0.15f, 0.03f, 0.06f);
  mpHelpTextMgr->AddString("S - Decrease Cloud Cover",		&string_id2[2], 0.05f, 0.25f, 0.03f, 0.06f);
  mpHelpTextMgr->AddString("D - Increase Cloud Cover",		&string_id2[3], 0.05f, 0.35f, 0.03f, 0.06f);
  mpHelpTextMgr->AddString("1-4 - Display Noise Octaves",	&string_id2[4], 0.05f, 0.45f, 0.03f, 0.06f);
  mpHelpTextMgr->AddString("5 - Display Turbulence",		&string_id2[5], 0.05f, 0.55f, 0.03f, 0.06f);
  mpHelpTextMgr->AddString("6 - Display Cloud Texture",		&string_id2[6], 0.05f, 0.65f, 0.03f, 0.06f);
  mpHelpTextMgr->AddString("7 - Display RNG Lookup",		&string_id2[7], 0.05f, 0.75f, 0.03f, 0.06f);
  mpHelpTextMgr->AddString("X - Exit",						&string_id2[8], 0.051f,0.85f, 0.03f, 0.06f);
  

  mpHelpTextMgr->SetVisible(false); //off by default

  // Since the text uses transparency, we'll add it to the root last, causing it
  //  to get rendered last. (Note: The appwizard offers a displaylist manager that
  //  lets us sort for this sort of thing, but obfuscates the code somewhat, so
  //  we chose not to use it for this demo)

  
  // This is the curved dome-like geometry we'll map the sky to. Note: The 
  //  numbers fed to the Init & Scale function that determine the objects 
  //  size and curvature are kind of "fudge factors". This object is usually
  //  used in a larger terrain engine with a different scale

  mpSkyObj = new IawSkyPlane();
  mpSkyObj->Init(pWrapper,10,280.0f,295.0f,2.0f,2.0f);
  mpSkyObj->SetPos(IawVector(0.0f,25.0f,0.0f));
  mpSkyObj->SetObjectName("SkyObj");
  mpRootObj->AddChild(mpSkyObj);

  // Add a terrain object. This is a terrain generated procedurally by summing 
  //  octaves of noise and feeding them into a power funtion. See 
  //  IawTerrainObject.h/.cpp and ExponentialNoise.h/.cpp for details
  mpTerrainObj = new IawTerrain(TERRAINFVF, 5); //5 is a seed. Change to get different terrain
  mpTerrainObj->Init(pWrapper, 64);
  mpTerrainObj->SetScale(IawVector(2.0f,0.1f,2.0f));  
  mpTerrainObj->SetPos(IawVector(0.0f,-11.0f, 0.0f));
  mpTerrainObj->SetObjectName("Terrain");
  mpRootObj->AddChild(mpTerrainObj);

  // Our water is a simple plane. The water today is, umm, calm.
  mpWaterObj = new IawPlane();
  mpWaterObj->Init(mpWrapper,10);
  mpWaterObj->SetScale(IawVector(40.0f,1.0f,40.0f));  
  mpWaterObj->SetPos(IawVector(0.0f,-60.0f,0.0f));  
  mpWaterObj->SetRotationAngle(PI*0.5f);  
  mpTerrainObj->AddChild(mpWaterObj);

  
  
  //As mentioned before, we add these to the scenegraph last because of transparency
  mpRootObj->AddChild(mpFPSTextMgr);
  mpRootObj->AddChild(mpHelpTextMgr);

  // This is just a screenspace plane used to display textures for explanation purposes
  mpDisplayPlane = new IawPlane( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | 
		                           D3DFVF_SPECULAR | D3DFVF_TEX1); //TLVert
  mpDisplayPlane->Init(mpWrapper);
  mpDisplayPlane->SetUVScale(2.0f,2.0f);
  mpDisplayPlane->SetScale(IawVector(mpWrapper->GetWidth()/1.5f)); 
  mpDisplayPlane->SetVisible(false); //Not displayed by default
  mpRootObj->AddChild(mpDisplayPlane);


  // This plane is not added to the root node because it's not part of the scene.
  //  It's used when rendering to texture to turn the turbulence texture into
  //  the clouds texture
  mpCloudTexturePlane = new IawPlane( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | 
 		                              D3DFVF_SPECULAR | D3DFVF_TEX1); //TLVert
  mpCloudTexturePlane->Init(mpWrapper);
  //TLVert version of our plane uses the scale to set the X and Y screenspace values
  mpCloudTexturePlane->SetScale(IawVector(256.0f)); 


  //////////////////////////////////////
  // Create Texture Containers
  //////////////////////////////////////

  // This is the render target cloud texture that we create from the turbulence
  mpCloudTexture = new IawRenderTarget(mpWrapper);
  mpCloudTexture->CreateTexture(256,256, 32, 0,
	                            IawTexture::IAW_TEXTR_RENDER_TARGET | 
//							    IawTexture::IAW_TEXTR_MUST_HAVE_ALPHA |
                                IawTexture::IAW_TEXTR_32_BITS_PER_PIXEL |
								IawTexture::IAW_TEXTR_32_BITZ, 
								NULL);

  


  //create the texture that will be used by the terrain shader
  mpTerrainBaseTexture = new IawTexture(mpWrapper);
  mpTerrainBaseTexture->CreateTexture(
			IawBmpLoader(IawFileStream("resources\\images\\TerrainHeightColor.bmp")),
			0, 0);

  // We'll create a font texture (our texture class will lay out the characters
  //  for us) and create a shader with it later. The shader will be passed to
  //  both textmgr objects
  mpFontTexture = new IawTexture(mpWrapper);
  mpFontTexture->CreateTexture(IawTexture::FONT_TEXTURE_SIZE,
                               IawTexture::FONT_TEXTURE_SIZE,
                               16,
                               0,
                               IawTexture::IAW_TEXTR_CREATE_FONT,
                               NULL);


  //////////////////////////////////////
  // Create Shader Container Tree
  //////////////////////////////////////

  // The shader manager manages a linked list of shaders & a pool of 'shader 
  //  elements' (basically a stateblock). Each shader has one or more 
  //  implementations, each implementation is comprised of one or more 
  //  components, each component points to an element. 
  //
  // The reason for separating components and elements is to allow sharing 
  //  of stateblocks (e.g. Two different shaders may both use the 
  //  same "settexture(brick)" stateblock.
  //
  // Because the stateblocks are device resources, they actually get setup 
  //  later. Here we just setup the 'shadertree'
  // 
  // Hopefully, the indentation below makes it a little easier to read.

  mpShaderManager = new IawShaderMgr("Shader Manager");


  int shader, imp, comp, element;
  i = 0;
  mpShaderManager->CreateShader(&shader, "Default Shader");  
    mpShaderManager->CreateShaderImplementation(shader, &imp, "DS Ver 1");
    mpShaderManager->CreateShaderComponent(shader, imp, &comp, "DS V1, Comp 1");
        mpShaderManager->CreateElement(&element, "Default shader renderstate");
        mpShaderManager->SetComponentElement(shader, imp, comp, element);


  mpShaderManager->CreateShader(&shader, "Font Shader");   
    mpShaderManager->CreateShaderImplementation(shader, &imp, "Font Ver 1");
      mpShaderManager->CreateShaderComponent(shader, imp, &comp, "FNT V1, Comp 1");
        mpShaderManager->CreateElement(&element, "Font state block");
        mpShaderManager->SetComponentElement(shader, imp, comp, element);

  // Shader used by the terrain, just modulated a height/color with terrain detail
  mpShaderManager->CreateShader(&shader, "Terrain Shader"); 
    mpShaderManager->CreateShaderImplementation(shader, &imp, "Terrain Ver 1");
      mpShaderManager->CreateShaderComponent(shader, imp, &comp, "Terr V1, Comp 1");
        mpShaderManager->CreateElement(&element, "Terrain Base Texture X Detail Texture");
        mpShaderManager->SetComponentElement(shader, imp, comp, element);

  // Applies the cloud texture to the geometry, adds in the blue sky color, etc
  mpShaderManager->CreateShader(&shader, "Cloud Shader");   
    mpShaderManager->CreateShaderImplementation(shader, &imp, "Cloud Ver 1");
      mpShaderManager->CreateShaderComponent(shader, imp, &comp, "Cloud V1, Comp 1");
        mpShaderManager->CreateElement(&element, "apply cloud texture state block");
        mpShaderManager->SetComponentElement(shader, imp, comp, element);
		

  // Takes the sum-of-octaves turbulence texture and turns it into isolated clouds
  mpShaderManager->CreateShader(&shader, "Cloud-From-Turbulence Shader");   
    mpShaderManager->CreateShaderImplementation(shader, &imp, "CFT Ver 1");
      mpShaderManager->CreateShaderComponent(shader, imp, &comp, "CFT V1, Comp 1");
        mpShaderManager->CreateElement(&element, "CFT state block");
        mpShaderManager->SetComponentElement(shader, imp, comp, element);


  // This just displays a texture for debugging.
  mpShaderManager->CreateShader(&shader, "Display Turbulence Texture Shader");   
    mpShaderManager->CreateShaderImplementation(shader, &imp, "DTT Ver 1");
      mpShaderManager->CreateShaderComponent(shader, imp, &comp, "DTT V1, Comp 1");
        mpShaderManager->CreateElement(&element, "Display Turbulence state block");
        mpShaderManager->SetComponentElement(shader, imp, comp, element);

  // This just displays a texture for debugging.
  mpShaderManager->CreateShader(&shader, "Display CFT Texture Shader");   
    mpShaderManager->CreateShaderImplementation(shader, &imp, "DCFT Ver 1");
      mpShaderManager->CreateShaderComponent(shader, imp, &comp, "DCFT V1, Comp 1");
        mpShaderManager->CreateElement(&element, "Display CFT state block");
        mpShaderManager->SetComponentElement(shader, imp, comp, element);

  // This is used to map the clouds onto the water as a reflection,and to 
  //  make the water translucent
  mpShaderManager->CreateShader(&shader, "Water Shader");   
    mpShaderManager->CreateShaderImplementation(shader, &imp, "WS Ver 1");
      mpShaderManager->CreateShaderComponent(shader, imp, &comp, "WS V1, Comp 1");
        mpShaderManager->CreateElement(&element, "Water state block");
        mpShaderManager->SetComponentElement(shader, imp, comp, element);


  // These two classes actually manage a bunch of textures and produce a 
  //  texture that is sum of octaves of noise (in one case, animated as well)
  //  We waited till now to create them, since they use teh shader manager
  //  as well.
  
  //create the RandTextureGenerator, keep in mind, it will also use the shader manager to create some shaders
  mpRandTextureGenerator->SetShaderMgr(mpShaderManager);
  mpRandTextureGenerator->InitWorld(mpWrapper);

  mpStaticRandTextureGenerator->SetShaderMgr(mpShaderManager);
  mpStaticRandTextureGenerator->InitWorld(mpWrapper);
  

  /////////////////////////////////////////////////////////////////////////
  // Now we assign the shaders created above to the objects created earlier
  /////////////////////////////////////////////////////////////////////////

  mpRootObj->SetShaderID(mpShaderManager->GetShaderId("Default Shader"));

  mpTerrainObj->SetShaderID(mpShaderManager->GetShaderId("Terrain Shader"));  
  
  mpFPSTextMgr->SetShaderID(mpShaderManager->GetShaderId("Font Shader"), true);
  mpHelpTextMgr->SetShaderID(mpShaderManager->GetShaderId("Font Shader"), true);
    
  mpSkyObj->SetShaderID(mpShaderManager->GetShaderId("Cloud Shader"), true);
  mpCloudTexturePlane->SetShaderID(mpShaderManager->GetShaderId("Cloud-From-Turbulence Shader"), true);
  
  mpWaterObj->SetShaderID(mpShaderManager->GetShaderId("Water Shader"), true);

  // Now that we are done setting everything up, this will init all the device
  //  dependant stuff (VB's, IB's, Textures & stateblocks)
  TargetChanged();

  // This will OutputDebugString() the whole shader hierarchy 
  mpShaderManager->PrintShaderHierarchy();

  return S_OK;
}

HRESULT ProceduralCloudDemo::DestroyWorld()
{
  // TargetChanging cleans up some device dependant stuff 
  // on the old device before destroying it, so call it first
  TargetChanging();

  SAFE_DELETE(mpRootObj);
  SAFE_DELETE(mpCloudTexturePlane); //need to do separate, since it wasn't added to root

  // Delete Textures
  SAFE_DELETE(mpFontTexture);
  SAFE_DELETE(mpCloudTexture);  
  SAFE_DELETE(mpTerrainBaseTexture);
  
  return S_OK;
}


HRESULT ProceduralCloudDemo::UpdateWorld()
{
//  mpWrapper->mpDevice->SetTransform(D3DTS_WORLD, &mWorldMat.mMx);
//  mpWrapper->mpDevice->SetTransform(D3DTS_VIEW, &mViewMat.mMx);

  ///////////////////////////////////
  //Updates all moving objects according to time elapsed, etc
  mpRootObj->Update(mpWrapper->GetFrameTime(), true);
  
  ///////////////////////////////////
  //update frame counter
  if (mTimeSinceLastFpsUpdate > 1.0f) //update the frame counter once in a while
  {
    DWORD dwFps = (DWORD)(1.0f / mpWrapper->GetFrameTime());
    char fpsstring[10];
    sprintf(fpsstring, "%03d", dwFps);
    //two ways to do this. either modify the string content,
    // or have different strings for each number and place, and
    // select the ones to draw
    mpFPSTextMgr->ChangeStringContent(0,fpsstring);
    mTimeSinceLastFpsUpdate = 0;
  }
  mTimeSinceLastFpsUpdate += mpWrapper->GetFrameTime();

  ///////////////////////////////////
  //
  // This is where our animated turbulence is updated for the frame. See that 
  //  class for the details
  //
  ///////////////////////////////////  
  mpRandTextureGenerator->UpdateWorld();
  // mpStaticRandTextureGenerator->UpdateWorld(); //not needed because its static

  ///////////////////////////////////
  //
  // Now we create our Cloud texture from the noise. First thing is to calculate 
  //  a Texturefactor value for the current level of noise.
  //
  ///////////////////////////////////  
  unsigned char cTemp = (1.0f-mfCloudAmount) * 0xff;
  DWORD dwTemp = (DWORD)cTemp<<24 | (DWORD)cTemp<<16 | (DWORD)cTemp<<8 | cTemp;
  
  // Set this as the texture factor. The rest of the renderstate setup is done
  // by Cloud shader (see SetupShaders() )
  mpWrapper->mpDevice->SetRenderState(D3DRS_TEXTUREFACTOR, dwTemp);
  
  // And render it onto the cloudtexture's surface
  mpCloudTexture->BeginScene();

  mpCloudTexturePlane->Render(mWorldMat, true);
  
  mpCloudTexture->EndScene();

  return S_OK;
}

HRESULT ProceduralCloudDemo::RenderWorld()
{
  HRESULT hr;
  hr = S_OK;

  // Put this back in if you've changed it to view from 'satellite' view
  //mpWrapper->mpDevice->SetRenderState(D3DRS_CULLMODE , D3DCULL_NONE);

  // This recurses through the tree of objects and draws them all.
  if (mpRootObj)
    hr = mpRootObj->Render(mWorldMat, true);

  return S_OK;
}

// This is called prior to any device change. Let's us tear down old stuff
//  before building up the new stuff
HRESULT ProceduralCloudDemo::TargetChanging()
{
  HRESULT hr = S_OK;

  if (!mpWrapper || !(mpWrapper->mpDevice))
    return S_OK;

  mpWrapper->mpDevice->SetTexture(0, NULL);
  mpWrapper->mpDevice->SetTexture(1, NULL);
  mpWrapper->mpDevice->SetTexture(2, NULL);
  mpWrapper->mpDevice->SetTexture(3, NULL);
  mpWrapper->mpDevice->SetTexture(4, NULL);
  mpWrapper->mpDevice->SetTexture(5, NULL);
  mpWrapper->mpDevice->SetTexture(6, NULL);
  mpWrapper->mpDevice->SetTexture(7, NULL);

  // This will delete all state blocks and invalidate all shaders.
  if(mpShaderManager)
    mpShaderManager->TargetChanging(mpWrapper);

  if (mpRootObj)
	hr = mpRootObj->TargetChanging(mpWrapper, true);

  if (mpCloudTexturePlane)
    hr = mpCloudTexturePlane->TargetChanging(mpWrapper, true);

  if (mpRandTextureGenerator)
    mpRandTextureGenerator->TargetChanging();

  if (mpStaticRandTextureGenerator)
    mpStaticRandTextureGenerator->TargetChanging();

  SAFE_RELEASE(mpTerrainBaseTexture->mpTexture);
  
  SAFE_RELEASE(mpFontTexture->mpTexture);
  SAFE_RELEASE(mpCloudTexture->mpTexture);

  return hr;
}

HRESULT ProceduralCloudDemo::TargetChanged()
{
  HRESULT hr = 0;
  if (mpFontTexture)
    mpFontTexture->Restore();
  if (mpCloudTexture)
    mpCloudTexture->Restore();

  mpWrapper->mpDevice->SetTransform(D3DTS_WORLD, &mWorldMat.mMx);
  mpWrapper->mpDevice->SetTransform(D3DTS_VIEW, &mViewMat.mMx);
  mpWrapper->mpDevice->SetTransform(D3DTS_PROJECTION, &mProjectionMat.mMx);

  mpWrapper->mpDevice->SetLight(0, &mSunLight);
  mpWrapper->mpDevice->LightEnable(0, TRUE);
  

  //mpWrapper->mpDevice->SetLight(1, &mCloudMultiplierLight);
  //mpWrapper->mpDevice->LightEnable(1, FALSE);

  D3DMATERIAL8 mtrl;
  ZeroMemory(&mtrl, sizeof(D3DMATERIAL8));
  mtrl.Diffuse.r = 1.0f; mtrl.Ambient.r = 0.0f;
  mtrl.Diffuse.g = 1.0f; mtrl.Ambient.g = 0.0f;
  mtrl.Diffuse.b = 1.0f; mtrl.Ambient.b = 0.0f;
  mtrl.Diffuse.a = 1.0f; mtrl.Ambient.a = 0.0f;
  mpWrapper->mpDevice->SetMaterial(&mtrl);

  mpWrapper->mpDevice->SetRenderState(D3DRS_AMBIENT, 0x00202020);

  //set default renderstates 

  mpWrapper->mpDevice->SetRenderState(D3DRS_NORMALIZENORMALS , TRUE);
  mpWrapper->mpDevice->SetRenderState(D3DRS_DITHERENABLE, TRUE);

  hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_MINFILTER, D3DTEXF_LINEAR ); 
  hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_MAGFILTER, D3DTEXF_LINEAR ); 
  hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_MINFILTER, D3DTEXF_LINEAR ); 
  hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_MAGFILTER, D3DTEXF_LINEAR ); 

  mpDisplayPlane->SetScale(IawVector(mpWrapper->GetWidth()/1.5f)); 

  mpRootObj->TargetChanged(mpWrapper, mpShaderManager, true);
  mpCloudTexturePlane->TargetChanged(mpWrapper, mpShaderManager, true);


  hr = mpWrapper->mpDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
  hr = mpWrapper->mpDevice->SetRenderState(D3DRS_FOGCOLOR, FOGCOLOR);
  
  float fFogStart = FOGSTART;
  float fFogEnd = FOGEND;

  hr = mpWrapper->mpDevice->SetRenderState(D3DRS_FOGCOLOR, FOGCOLOR);
  hr = mpWrapper->mpDevice->SetRenderState( D3DRS_FOGSTART, *((LPDWORD) (&fFogStart)) );
	
  hr = mpWrapper->mpDevice->SetRenderState( D3DRS_FOGEND, *((LPDWORD) (&fFogEnd)) );
  hr = mpWrapper->mpDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);  


  if (mpTerrainBaseTexture)
    mpTerrainBaseTexture->Restore();


  //tear down the old shaders, sinse the the texture pointers are going to change
  mpShaderManager->TargetChanging(mpWrapper);

  mpRandTextureGenerator->TargetChanged();
  mpStaticRandTextureGenerator->TargetChanged();

  SetupShaders();


  return hr;
}

// The shader hierarchy has been set up in InitWorld() already. This is where
// the device dependant components get defined/setup
HRESULT ProceduralCloudDemo::SetupShaders()
{
  HRESULT hr;  

  ///////////////////////////////////////////////////////
  //
  // Modulate texture with diffuse, blending off, fog off
  //
  ///////////////////////////////////////////////////////  
  DWORD dwDefaultShaderStateBlock = 0x00000000;

  hr = mpWrapper->mpDevice->BeginStateBlock();
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_MODULATE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    hr = mpWrapper->mpDevice->SetTexture(0,NULL);
    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_DISABLE); //disable other stages
    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_ALPHAOP, D3DTOP_DISABLE); //disable other stages
    hr = mpWrapper->mpDevice->SetTexture(1,NULL);

    hr = mpWrapper->mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	mpWrapper->mpDevice->LightEnable(0, TRUE);
	mpWrapper->mpDevice->LightEnable(1, FALSE);

	hr = mpWrapper->mpDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);


  hr = mpWrapper->mpDevice->EndStateBlock(&dwDefaultShaderStateBlock);

  hr = mpShaderManager->SetElementStateBlock("Default shader renderstate", dwDefaultShaderStateBlock);


  ///////////////////////////////////////////////////////
  //
  // Stage 0 - modulate base texture with diffuse
  // Stage 1 - modulate detail texture with current
  //
  ///////////////////////////////////////////////////////  
  DWORD dwTerrainStateBlock = 0x00000000;
  
  hr = mpWrapper->mpDevice->BeginStateBlock();
    hr = mpWrapper->mpDevice->SetTexture(0,mpTerrainBaseTexture->mpTexture);    
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, 0);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_MODULATE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    
	hr = mpWrapper->mpDevice->SetTexture(1,mpStaticRandTextureGenerator->GetTurbulenceTexture()->mpTexture);
	hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, 1);
    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_COLORARG1, D3DTA_TEXTURE );
    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_COLORARG2, D3DTA_CURRENT);	
    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_MODULATE);
	hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    
	hr = mpWrapper->mpDevice->SetTextureStageState(2,D3DTSS_COLOROP, D3DTOP_DISABLE); //disable other stages
    hr = mpWrapper->mpDevice->SetTextureStageState(2,D3DTSS_ALPHAOP, D3DTOP_DISABLE); //disable other stages
    hr = mpWrapper->mpDevice->SetTexture(2,NULL);   

    hr = mpWrapper->mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	hr = mpWrapper->mpDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);

	mpWrapper->mpDevice->LightEnable(0, TRUE);
	//mpWrapper->mpDevice->LightEnable(1, FALSE);


  hr = mpWrapper->mpDevice->EndStateBlock(&dwTerrainStateBlock);
  
  hr = mpShaderManager->SetElementStateBlock("Terrain Base Texture X Detail Texture", dwTerrainStateBlock);


  ///////////////////////////////////////////////////////
  //
  // modulate font texture with diffuse, blending on
  //
  ///////////////////////////////////////////////////////  
  DWORD dwFontShaderStateBlock = 0x00000000;

  hr = mpWrapper->mpDevice->BeginStateBlock();
  hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, 0);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_MODULATE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    hr = mpWrapper->mpDevice->SetTexture(0,mpFontTexture->mpTexture);
    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_DISABLE); //disable other stages
    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_ALPHAOP, D3DTOP_DISABLE); //disable other stages
    hr = mpWrapper->mpDevice->SetTexture(1,NULL);

    hr = mpWrapper->mpDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

    hr = mpWrapper->mpDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    hr = mpWrapper->mpDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    hr = mpWrapper->mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	hr = mpWrapper->mpDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

  hr = mpWrapper->mpDevice->EndStateBlock(&dwFontShaderStateBlock);

  hr = mpShaderManager->SetElementStateBlock("Font state block", dwFontShaderStateBlock);

  ///////////////////////////////////////////////////////
  //
  // Stage 0 - factor - texture
  // Stage 1 - modulate 2X current with diffuse
  // 
  ///////////////////////////////////////////////////////  
  DWORD dwCFTStateBlock = 0x00000000; //cloud from turbulence;

  hr = mpWrapper->mpDevice->BeginStateBlock();

    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_TFACTOR);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_SUBTRACT);
	hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_DISABLE); //disable other stages
    hr = mpWrapper->mpDevice->SetTexture(0,mpRandTextureGenerator->GetTurbulenceTexture()->mpTexture);	

	hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_MODULATE2X); 
	hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_COLORARG2, D3DTA_CURRENT);
	hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_ALPHAOP, D3DTOP_DISABLE); //disable other stages
	hr = mpWrapper->mpDevice->SetTexture(1,NULL);

  	
	hr = mpWrapper->mpDevice->SetTextureStageState(2,D3DTSS_COLOROP, D3DTOP_DISABLE); //disable other stages
    hr = mpWrapper->mpDevice->SetTextureStageState(2,D3DTSS_ALPHAOP, D3DTOP_DISABLE); //disable other stages
    hr = mpWrapper->mpDevice->SetTexture(2,NULL);

    
	hr = mpWrapper->mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    
	hr = mpWrapper->mpDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	hr = mpWrapper->mpDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);	
  
  hr = mpWrapper->mpDevice->EndStateBlock(&dwCFTStateBlock);


  hr = mpShaderManager->SetElementStateBlock("CFT state block", dwCFTStateBlock);

  ///////////////////////////////////////////////////////
  //
  // Stage 0 - factor + texture (factor adds sky color)  
  // 
  ///////////////////////////////////////////////////////  
  
  DWORD dwACTStateBlock  = 0x00000000; 

  hr = mpWrapper->mpDevice->BeginStateBlock();

    hr = mpWrapper->mpDevice->SetRenderState(D3DRS_TEXTUREFACTOR, RGB_MAKE(0x44, 0x44, 0xff));

    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_TFACTOR);
	hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_ADD);
	hr = mpWrapper->mpDevice->SetTexture(0,mpCloudTexture->mpTexture);	

	
	hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_DISABLE); 
	hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_ALPHAOP, D3DTOP_DISABLE); //disable other stages
	hr = mpWrapper->mpDevice->SetTexture(1,NULL);
  	
	hr = mpWrapper->mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	hr = mpWrapper->mpDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	hr = mpWrapper->mpDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);

	mpWrapper->mpDevice->LightEnable(0, FALSE);
	mpWrapper->mpDevice->LightEnable(1, TRUE);
  
  hr = mpWrapper->mpDevice->EndStateBlock(&dwACTStateBlock);
  

  hr = mpShaderManager->SetElementStateBlock("apply cloud texture state block", dwACTStateBlock);
  

  ///////////////////////////////////////////////////////
  //
  // adds cloud to translucent water factor
  // 
  ///////////////////////////////////////////////////////  

  
  DWORD dwWaterShaderStateBlock = 0x00000000;

  hr = mpWrapper->mpDevice->BeginStateBlock();

    hr = mpWrapper->mpDevice->SetRenderState(D3DRS_TEXTUREFACTOR, RGBA_MAKE(0x44, 0x44, 0xff, 0x55));
	
  
  
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TFACTOR);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_ADD);
	hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
	hr = mpWrapper->mpDevice->SetTexture(0,mpCloudTexture->mpTexture);	

	hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_DISABLE);
    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	hr = mpWrapper->mpDevice->SetTexture(1,NULL);

	hr = mpWrapper->mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    hr = mpWrapper->mpDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    hr = mpWrapper->mpDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	hr = mpWrapper->mpDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);

  hr = mpWrapper->mpDevice->EndStateBlock(&dwWaterShaderStateBlock);
  
  hr = mpShaderManager->SetElementStateBlock("Water state block", dwWaterShaderStateBlock);

  ///////////////////////////////////////////////////////
  //
  // Stage 0 - just displays a texture
  // 
  ///////////////////////////////////////////////////////  


  DWORD dwDisplayTurbulenceStateBlock = 0x00000000;

  hr = mpWrapper->mpDevice->BeginStateBlock();

    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    hr = mpWrapper->mpDevice->SetTexture(0,mpRandTextureGenerator->GetTurbulenceTexture()->mpTexture);	
	
	hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_DISABLE); //disable other stages
    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_ALPHAOP, D3DTOP_DISABLE); //disable other stages
    hr = mpWrapper->mpDevice->SetTexture(1,NULL);
    
	hr = mpWrapper->mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  
  hr = mpWrapper->mpDevice->EndStateBlock(&dwDisplayTurbulenceStateBlock);

  hr = mpShaderManager->SetElementStateBlock("Display Turbulence state block", dwDisplayTurbulenceStateBlock);

  ///////////////////////////////////////////////////////
  //
  // Stage 0 - just displays a texture
  // 
  ///////////////////////////////////////////////////////  
  DWORD dwDisplayCFTStateBlock = 0x00000000;

  hr = mpWrapper->mpDevice->BeginStateBlock();

    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    hr = mpWrapper->mpDevice->SetTexture(0,mpCloudTexture->mpTexture);	
	
	hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_DISABLE); //disable other stages
    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_ALPHAOP, D3DTOP_DISABLE); //disable other stages
    hr = mpWrapper->mpDevice->SetTexture(1,NULL);
    
	hr = mpWrapper->mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  
  hr = mpWrapper->mpDevice->EndStateBlock(&dwDisplayCFTStateBlock);

  hr = mpShaderManager->SetElementStateBlock("Display CFT state block", dwDisplayCFTStateBlock);


  return S_OK;
}


void ProceduralCloudDemo::KeyDown(WPARAM wParam)
{ 	

  switch (wParam)
  {
  case 'H':
    if (mpHelpTextMgr)
	{
      if (mpHelpTextMgr->GetVisible())
	    mpHelpTextMgr->SetVisible(false);
	  else
	    mpHelpTextMgr->SetVisible(true);
	}
	break;
  case 'A':
    if (mpRandTextureGenerator)
		mpRandTextureGenerator->ToggleUpdateAcceleration();
	break;
	
  case 'S':
    if (mfCloudAmount>=0.02f)
	  mfCloudAmount-=0.02f;
	break;

  case 'D':
	if (mfCloudAmount<=0.98f)
	  mfCloudAmount+=0.02f;
	break;

  case '1':
    //if we're already pointed to this shader then make the plane disappear
    if (mpDisplayPlane && mpShaderManager)
	{
	  if ((mpDisplayPlane->GetShaderID() == mpShaderManager->GetShaderId("Apply Noise Texture 0 Shader")) &&	  
	      (mpDisplayPlane->GetVisible() == true) )
	  {
	    mpDisplayPlane->SetVisible(false);
	  }
	  else
	  {
	    mpDisplayPlane->SetShaderID(mpShaderManager->GetShaderId("Apply Noise Texture 0 Shader"), true);
	    mpDisplayPlane->SetVisible(true);
	  }
	}
	break;
  case '2':
	if (mpDisplayPlane && mpShaderManager)
	{
      //if we're already pointed to this shader then make the plane disappear
	  if ((mpDisplayPlane->GetShaderID() == mpShaderManager->GetShaderId("Apply Noise Texture 1 Shader")) &&	  
	      (mpDisplayPlane->GetVisible() == true) )
	  {
	    mpDisplayPlane->SetVisible(false);
	  }
	  else
	  {
	    mpDisplayPlane->SetShaderID(mpShaderManager->GetShaderId("Apply Noise Texture 1 Shader"), true);
	    mpDisplayPlane->SetVisible(true);
	  }
	}
	break;
  case '3':
    if (mpDisplayPlane && mpShaderManager)
	{
      //if we're already pointed to this shader then make the plane disappear
	  if ((mpDisplayPlane->GetShaderID() == mpShaderManager->GetShaderId("Apply Noise Texture 2 Shader")) &&	  
	      (mpDisplayPlane->GetVisible() == true) )
	  {
	    mpDisplayPlane->SetVisible(false);
	  }
	  else
	  {
	    mpDisplayPlane->SetShaderID(mpShaderManager->GetShaderId("Apply Noise Texture 2 Shader"), true);
	    mpDisplayPlane->SetVisible(true);
	  }
	}
	break;
  case '4':
	if (mpDisplayPlane && mpShaderManager)
	{
      //if we're already pointed to this shader then make the plane disappear
	  if ((mpDisplayPlane->GetShaderID() == mpShaderManager->GetShaderId("Apply Noise Texture 3 Shader")) &&	  
	      (mpDisplayPlane->GetVisible() == true) )
	  {
	    mpDisplayPlane->SetVisible(false);
	  }
	  else
	  {
  	    mpDisplayPlane->SetShaderID(mpShaderManager->GetShaderId("Apply Noise Texture 3 Shader"), true);
	    mpDisplayPlane->SetVisible(true);
	  }
	}
	break;

  case '5':
    if (mpDisplayPlane && mpShaderManager)
	{

      //if we're already pointed to this shader then make the plane disappear
      if ((mpDisplayPlane->GetShaderID() == mpShaderManager->GetShaderId("Display Turbulence Texture Shader")) &&	  
          (mpDisplayPlane->GetVisible() == true) )
	  {
        mpDisplayPlane->SetVisible(false);
	  }
      else
	  {
        mpDisplayPlane->SetShaderID(mpShaderManager->GetShaderId("Display Turbulence Texture Shader"), true);
        mpDisplayPlane->SetVisible(true);
	  }
	}
  break;

  case '6':
    if (mpDisplayPlane && mpShaderManager)
	{
      //if we're already pointed to this shader then make the plane disappear
      if ((mpDisplayPlane->GetShaderID() == mpShaderManager->GetShaderId("Display CFT Texture Shader")) &&	  
          (mpDisplayPlane->GetVisible() == true) )
	  {	  
        mpDisplayPlane->SetVisible(false);
        mpDisplayPlane->SetDiffuse(0xffffffff);
        mpDisplayPlane->UpdateVB();
 	  }
      else
	  {
        //unsigned char cTemp = (1.0f-mfCloudAmount) * 0xff;
        //DWORD dwTemp = (DWORD)cTemp<<24 | (DWORD)cTemp<<16 | (DWORD)cTemp<<8 | cTemp;

        float factor = mfCloudAmount; //aa
        factor *= 1.0f / (0.5f+0.25f+0.125f+0.0625f);

        unsigned char cTemp = 0xff;
	    if (factor >0.25f)
		    cTemp *= 0.25f/factor;
        DWORD dwTemp = (DWORD)cTemp<<24 | (DWORD)cTemp<<16 | (DWORD)cTemp<<8 | cTemp;

        mpDisplayPlane->SetShaderID(mpShaderManager->GetShaderId("Display CFT Texture Shader"), true);
        mpDisplayPlane->SetVisible(true);
        mpDisplayPlane->SetDiffuse(dwTemp);
        mpDisplayPlane->UpdateVB();
      }
	}
	break;

	case '7':
	if (mpDisplayPlane && mpShaderManager)
	{
      //if we're already pointed to this shader then make the plane disappear
	  if ((mpDisplayPlane->GetShaderID() == mpShaderManager->GetShaderId("RNG Lookup Shader")) &&	  
	      (mpDisplayPlane->GetVisible() == true) )
	  {
 	    mpDisplayPlane->SetVisible(false);
	  }
	  else
	  {
	    mpDisplayPlane->SetShaderID(mpShaderManager->GetShaderId("RNG Lookup Shader"), true);
	    mpDisplayPlane->SetVisible(true);
	  }
	}
	break;	
  }

  // OK, this *really* isn't the best place for this, but since I did a hack 
  //  above to display the texture, I need to undo it here.
  
  float factor = mfCloudAmount; 

  factor *= 1.0f / (0.5f+0.25f+0.125f+0.0625f);

  mCloudMultiplierLight.Diffuse.a = 0.25f/factor;
  mCloudMultiplierLight.Diffuse.b = 0.25f/factor;
  mCloudMultiplierLight.Diffuse.g = 0.25f/factor;
  mCloudMultiplierLight.Diffuse.r = 0.25f/factor;

  if ((mpWrapper) && (mpWrapper->mpDevice) && mpCloudTexturePlane)
  {
	mpWrapper->mpDevice->SetLight(1, &mCloudMultiplierLight);
    //mpWrapper->mpDevice->LightEnable(1, FALSE);

	//try changing diffuse values in CloudTexturePlane;
	if (mpCloudTexturePlane)
	{
	  unsigned char temp = 0xff;

	  if (factor>0.25f)
	    temp *= (0.25f/factor);
	  mpCloudTexturePlane->SetDiffuse((DWORD)temp<<24 | (DWORD)temp<<16 | (DWORD)temp <<8 | (DWORD)temp);
	  mpCloudTexturePlane->UpdateVB();
	}	  

	if (mpFPSTextMgr)
	{
      char string[5];
      sprintf(string, "%01.2f", mfCloudAmount);
	  
      mpFPSTextMgr->ChangeStringContent(3,string);
	}
  }  

  return;
}

void ProceduralCloudDemo::KeyUp(WPARAM wParam)
{ 
  return;
}

void ProceduralCloudDemo::MouseLeftUp(WPARAM wParam, LPARAM lParam)
{ 
  return;
}

void ProceduralCloudDemo::MouseLeftDown(WPARAM wParam, LPARAM lParam)
{   
  return;
}

void ProceduralCloudDemo::MouseRightUp(WPARAM wParam, LPARAM lParam)
{ 
  return;
}

void ProceduralCloudDemo::MouseRightDown(WPARAM wParam, LPARAM lParam)
{   
  return;
}

void ProceduralCloudDemo::MouseMove(WPARAM wParam, LPARAM lParam)
{
  if (wParam & MK_LBUTTON)  // Left button down plus mouse move == rotate
  {
    //we need a variable to modify our rotation speed with frame rate 
    float scaler = 1.0f;
    if (mpWrapper) //we could get into this loop after the wrapper has been invalidates
    {
      scaler = 2.0f / 640;//max(mpWrapper->m_dwRenderWidth, mpWrapper->m_dwRenderHeight);  
    }

    //we need to compare the last position to the current position
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);

    if (x - mMousePrevX)
    {
      mViewMat.Rotate(mViewMat.mMx._12, mViewMat.mMx._22, mViewMat.mMx._32, PI*scaler*(float)(x - mMousePrevX));
    }

    if (y - mMousePrevY)
    {
      mViewMat.Rotate(mViewMat.mMx._11, mViewMat.mMx._21, mViewMat.mMx._31, PI*scaler*(float)(y - mMousePrevY));
    }

    mpWrapper->mpDevice->SetTransform(D3DTS_WORLD, &mWorldMat.mMx);
    mpWrapper->mpDevice->SetTransform(D3DTS_VIEW, &mViewMat.mMx);

    mMousePrevX = x;
    mMousePrevY = y;
  }
  else
  {
    //need to update these, or the next time someone clicks, the delta is massive
    mMousePrevX = LOWORD(lParam);
    mMousePrevY = HIWORD(lParam);
  }
}

