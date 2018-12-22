// RandTexture.cpp App Wizard Version 2.0 Beta 1
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

#define RNGLOOKUPTEXTURESIZE 256 //size used for the 
#define NOISEOCTAVETEXTURESIZE 256 //size used by the 
#define NOISECOMPOSITETEXTURESIZE 256 //size used by the 

// default speed to animate octaves at (in seconds). These should be powers of
//  two, but slightly offset so that we doing get update times 'coinciding'
#define UPDATERATE0 8.0f
#define UPDATERATE1 3.79f
#define UPDATERATE2 1.87f
#define UPDATERATE3 0.99f

#define OCTAVESPEED0 0.020f
#define OCTAVESPEED1 0.010f
#define OCTAVESPEED2 0.005f
#define OCTAVESPEED3 0.002f


// Constructor
RandTexture::RandTexture()
{
  //the pointers to the D3DWrapper and the ShaderManager will be handed down by the application.
  mpWrapper       = NULL;
  mpShaderManager = NULL;

  //Init a bunch of texture class pointers to NULL
  mpRNGLookupTexture = NULL;

  for (int i=0;i<NUMOCTAVES;i++)
  {
    mpNoiseOctaveTextureArray[i]             = NULL;
    mpPreviousNoiseOctaveTextureArray[i]     = NULL;
    mpInterpolatedNoiseOctaveTextureArray[i] = NULL;
    mpRNGQuadArray[i]                        = NULL;
    mpInterpolatingQuadArray[i]              = NULL;
    mpCompositeQuadArray[i]                  = NULL;
  }

  mpCompositeNoiseTexture = NULL;

    //This is the 'update rate of the various octaves
  mfNoiseOctaveUpdateRate[0] = UPDATERATE0;
  mfNoiseOctaveUpdateRate[1] = UPDATERATE1;
  mfNoiseOctaveUpdateRate[2] = UPDATERATE2;
  mfNoiseOctaveUpdateRate[3] = UPDATERATE3;

  mfNoiseOctaveSpeed[0] = OCTAVESPEED0;
  mfNoiseOctaveSpeed[1] = OCTAVESPEED1;
  mfNoiseOctaveSpeed[2] = OCTAVESPEED2;
  mfNoiseOctaveSpeed[3] = OCTAVESPEED3;

  mfNoiseOctavePos[0] = 0.0f;
  mfNoiseOctavePos[1] = 0.0f;
  mfNoiseOctavePos[2] = 0.0f;
  mfNoiseOctavePos[3] = 0.0f;

  mbAccelerateUpdates = false;

  //by init'ing these to a large value, we force an update
  mfNoiseOctaveAge[0] = 10.0f; 
  mfNoiseOctaveAge[1] = 10.0f;
  mfNoiseOctaveAge[2] = 10.0f;
  mfNoiseOctaveAge[3] = 10.0f;

  mfCurrentTime = 0.0f;
}

// Destructor
RandTexture::~RandTexture()
{
  mpWrapper       = NULL;
  mpShaderManager = NULL;
  for (int i = 0; i < NUMOCTAVES; i++)
  {
    delete mpRNGQuadArray[i];
    delete mpInterpolatingQuadArray[i];
    delete mpCompositeQuadArray[i];
    delete mpNoiseOctaveTextureArray[i];
    delete mpPreviousNoiseOctaveTextureArray[i];
    delete mpInterpolatedNoiseOctaveTextureArray[i];
  }
  SAFE_DELETE(mpRNGLookupTexture);
  SAFE_DELETE(mpCompositeNoiseTexture);
}

HRESULT RandTexture::InitWorld(IawD3dWrapper* pWrapper)
{
  mpWrapper = pWrapper;

  int i = 0;

  mpRNGLookupTexture = new IawTexture(mpWrapper);
  mpRNGLookupTexture->CreateTexture(RNGLOOKUPTEXTURESIZE,
                                    RNGLOOKUPTEXTURESIZE,
                                    32,
                                    0,
                                    IawTexture::IAW_TEXTR_32_BITS_PER_PIXEL,
                                    NULL);


  
  int shader, imp, comp, element;
  i = 0;

  //   The is just the shader 'skeleton'. The actual stateblocks contained
  //   in the 'element' structure get initialized later (in SetupShaders() )

  // This is the shader used to apply our RNG Lookup Texture onto another 
  // texture/render target.

  mpShaderManager->CreateShader(&shader, "RNG Lookup Shader");   
    mpShaderManager->CreateShaderImplementation(shader, &imp, "RNGL Ver 1");
      mpShaderManager->CreateShaderComponent(shader, imp, &comp, "RNGL V1, Comp 1");
        mpShaderManager->CreateElement(&element, "Apply RNG Lookup Texture");
        mpShaderManager->SetComponentElement(shader, imp, comp, element);

  // This sets up shaders to apply the different noise octaves textures. 
  mpShaderManager->CreateShader(&shader, "Apply Noise Texture 0 Shader");   
    mpShaderManager->CreateShaderImplementation(shader, &imp, "ANT0 Ver 1");
      mpShaderManager->CreateShaderComponent(shader, imp, &comp, "ANT0 V1, Comp 1");
        mpShaderManager->CreateElement(&element, "Apply Noise Texture 0");
        mpShaderManager->SetComponentElement(shader, imp, comp, element);

  mpShaderManager->CreateShader(&shader, "Apply Noise Texture 1 Shader");   
    mpShaderManager->CreateShaderImplementation(shader, &imp, "ANT1 Ver 1");
      mpShaderManager->CreateShaderComponent(shader, imp, &comp, "ANT1 V1, Comp 1");
        mpShaderManager->CreateElement(&element, "Apply Noise Texture 1");
        mpShaderManager->SetComponentElement(shader, imp, comp, element);

  mpShaderManager->CreateShader(&shader, "Apply Noise Texture 2 Shader");   
    mpShaderManager->CreateShaderImplementation(shader, &imp, "ANT2 Ver 1");
      mpShaderManager->CreateShaderComponent(shader, imp, &comp, "ANT2 V1, Comp 1");
        mpShaderManager->CreateElement(&element, "Apply Noise Texture 2");
        mpShaderManager->SetComponentElement(shader, imp, comp, element);

  mpShaderManager->CreateShader(&shader, "Apply Noise Texture 3 Shader");   
    mpShaderManager->CreateShaderImplementation(shader, &imp, "ANT3 Ver 1");
      mpShaderManager->CreateShaderComponent(shader, imp, &comp, "ANT3 V1, Comp 1");
        mpShaderManager->CreateElement(&element, "Apply Noise Texture 3");
        mpShaderManager->SetComponentElement(shader, imp, comp, element);

  InitNoiseOctaves();

  InitQuads();

  return S_OK;
}

HRESULT RandTexture::InitNoiseOctaves(/*int iNumOctaves*/)
{
  HRESULT hr;
  for (int i=0;i<NUMOCTAVES;i++)
  {
    if (mpNoiseOctaveTextureArray[i])
    {
      delete mpNoiseOctaveTextureArray[i];
      mpNoiseOctaveTextureArray[i] = NULL;
    }

    if (mpPreviousNoiseOctaveTextureArray[i])
    {
      delete mpPreviousNoiseOctaveTextureArray[i];
      mpPreviousNoiseOctaveTextureArray[i] = NULL;
    }

    if (mpInterpolatedNoiseOctaveTextureArray[i])
    {
      delete mpInterpolatedNoiseOctaveTextureArray[i];
      mpInterpolatedNoiseOctaveTextureArray[i] = NULL;
    }
  }

  if (mpCompositeNoiseTexture)
  {
    delete mpCompositeNoiseTexture;
    mpCompositeNoiseTexture = NULL;
  }

  //note that we index into the arrays in reverse order here, as we want the highest
  // frequencies to be the last elements in the array.  
  for (i=0;i<NUMOCTAVES;i++)
  {
    mpNoiseOctaveTextureArray[NUMOCTAVES-(i+1)] = new IawRenderTarget(mpWrapper);
    mpPreviousNoiseOctaveTextureArray[NUMOCTAVES-(i+1)] = new IawRenderTarget(mpWrapper);
    mpInterpolatedNoiseOctaveTextureArray[NUMOCTAVES-(i+1)] = new IawRenderTarget(mpWrapper);

    int iOctaveSize = 256 / (int)pow(2,i);  //256,128,64,32, 16, 8, 

    if (FAILED(hr = 
          mpNoiseOctaveTextureArray[NUMOCTAVES-(i+1)]->CreateTexture(
                          iOctaveSize, iOctaveSize, 32, 0, 
                          IawTexture::IAW_TEXTR_RENDER_TARGET | 
                          IawTexture::IAW_TEXTR_32_BITS_PER_PIXEL |
                          IawTexture::IAW_TEXTR_32_BITZ, 
                          NULL)))
    {      
      OutputDebugString("Couldn't create Octave Texture\n");
    }
    else
    {
      mpNoiseOctaveTextureArray[NUMOCTAVES-(i+1)]->Restore();
      mpNoiseOctaveTextureArray[NUMOCTAVES-(i+1)]->BeginScene(); //clear the color and Z buffers
      mpNoiseOctaveTextureArray[NUMOCTAVES-(i+1)]->EndScene();
    }


    if (FAILED(hr = 
          mpPreviousNoiseOctaveTextureArray[NUMOCTAVES-(i+1)]->CreateTexture(
                          iOctaveSize, iOctaveSize, 32, 0, 
                          IawTexture::IAW_TEXTR_RENDER_TARGET | 
                          IawTexture::IAW_TEXTR_32_BITS_PER_PIXEL |
                          IawTexture::IAW_TEXTR_32_BITZ, 
                          NULL)))
    {
      OutputDebugString("Couldn't create Octave Texture\n");
    }
    else
    {
      mpPreviousNoiseOctaveTextureArray[NUMOCTAVES-(i+1)]->Restore();
      mpPreviousNoiseOctaveTextureArray[NUMOCTAVES-(i+1)]->BeginScene(); //clear the color and Z buffers
      mpPreviousNoiseOctaveTextureArray[NUMOCTAVES-(i+1)]->EndScene();
    }


    if (FAILED(hr = 
          mpInterpolatedNoiseOctaveTextureArray[NUMOCTAVES-(i+1)]->CreateTexture(
                          iOctaveSize, iOctaveSize, 32, 0, 
                          IawTexture::IAW_TEXTR_RENDER_TARGET | 
                          IawTexture::IAW_TEXTR_32_BITS_PER_PIXEL |
                          IawTexture::IAW_TEXTR_32_BITZ, 
                          NULL)))
    {      
      OutputDebugString("Couldn't create Octave Texture\n");
    }
    else
    {
      mpInterpolatedNoiseOctaveTextureArray[NUMOCTAVES-(i+1)]->Restore();
      mpInterpolatedNoiseOctaveTextureArray[NUMOCTAVES-(i+1)]->BeginScene(); //clear the color and Z buffers
      mpInterpolatedNoiseOctaveTextureArray[NUMOCTAVES-(i+1)]->EndScene();
    }
  }

  mpCompositeNoiseTexture = new IawRenderTarget(mpWrapper);

  if (FAILED(hr = 
        mpCompositeNoiseTexture->CreateTexture(NOISECOMPOSITETEXTURESIZE, NOISECOMPOSITETEXTURESIZE, 32, 0,
                          IawTexture::IAW_TEXTR_RENDER_TARGET | 
                          IawTexture::IAW_TEXTR_32_BITS_PER_PIXEL |
                          IawTexture::IAW_TEXTR_32_BITZ, 
                          NULL)))
  {      
    OutputDebugString("Couldn't create composite Texture\n");
  }
  else
  {
    mpCompositeNoiseTexture->Restore();
    mpCompositeNoiseTexture->BeginScene(); //clear the color and Z buffers
    mpCompositeNoiseTexture->EndScene();
  }


  return S_OK;
}

// This function does two things. (1) determines if enough time has elapse 
//  that the noise texture needs updating and does so if needed, and (2) at 
//  every frame, creates the interpolated noise texture from the current and
//  previous updates using an alpha blend.
HRESULT RandTexture::UpdateNoiseOctave(int iOctave)
{
  if (mfNoiseOctaveAge[iOctave] > mfNoiseOctaveUpdateRate[iOctave])
  {
    //the current update becomes the old one
    LPDIRECT3DTEXTURE8 pTemp;

    mfNoiseOctaveAge[iOctave] -= mfNoiseOctaveUpdateRate[iOctave];

    pTemp = mpPreviousNoiseOctaveTextureArray[iOctave]->mpTexture;

    mpPreviousNoiseOctaveTextureArray[iOctave]->mpTexture = mpNoiseOctaveTextureArray[iOctave]->mpTexture;

    mpNoiseOctaveTextureArray[iOctave]->mpTexture = pTemp;

    //now we can go update the texture, generating a random offset to start 
    // the lookup into our RNG lookup texture

    float offset = (float)(int(mfCurrentTime)%511)/31.0f;

    mpRNGQuadArray[iOctave]->SetUVOffset(offset, offset);
    mpRNGQuadArray[iOctave]->UpdateVB();

    //mpRNGQuadArray[iOctave]->TargetChanged(mpWrapper,mpShaderManager,true);  // <- note this is very inefficient, need to fix

    mpNoiseOctaveTextureArray[iOctave]->BeginScene();
      IawMatrix mTemp(true);
      mpRNGQuadArray[iOctave]->Render(mTemp, true);
    mpNoiseOctaveTextureArray[iOctave]->EndScene();
  }

  //now create the interpolated version. Because we're dynamically modifying 
  // the value we're going to use for FACTOR, we didn't create a shader here, 
  // but rather just set the renderstates inline.
  float fTween = (mfNoiseOctaveAge[iOctave] / mfNoiseOctaveUpdateRate[iOctave]);
  unsigned char cValue = unsigned char(0xff * fTween);
  DWORD dwValue = (DWORD)cValue<<24 | (DWORD)cValue<<16 | (DWORD)cValue<<8 | cValue;

  mpInterpolatedNoiseOctaveTextureArray[iOctave]->BeginScene();
    //set the material source to be something else
    mpWrapper->mpDevice->SetRenderState(D3DRS_TEXTUREFACTOR, (0xffffffff-dwValue));

    mpWrapper->mpDevice->SetTexture(0,mpPreviousNoiseOctaveTextureArray[iOctave]->mpTexture);
    mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
    mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_TFACTOR);
    mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_MODULATE);
    mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
    //mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_DISABLE); //disable other stages
    mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_ALPHAOP, D3DTOP_DISABLE); //disable other stages
    mpWrapper->mpDevice->SetTexture(1,NULL);

    mpWrapper->mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    IawMatrix mTemp(true);
    mpInterpolatingQuadArray[iOctave]->Render(mTemp, true);

    mpWrapper->mpDevice->SetRenderState(D3DRS_TEXTUREFACTOR, dwValue);
  
    mpWrapper->mpDevice->SetTexture(0,mpNoiseOctaveTextureArray[iOctave]->mpTexture);

    mpWrapper->mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    mpWrapper->mpDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    mpWrapper->mpDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

    mpInterpolatingQuadArray[iOctave]->Render(mTemp, true);
  mpInterpolatedNoiseOctaveTextureArray[iOctave]->EndScene();

  //age the octave of noise
  mfNoiseOctaveAge[iOctave] += mpWrapper->GetFrameTime();

  return S_OK;
}

HRESULT RandTexture::UpdateCompositeNoise()
{
  //we want to do the following:
  //
  // Composite = 1/2 Octave0 + 1/4 Octave1 + 1/8 Octave2...
  //
  // So we'll do it as follows:

  // first: 
  //magic number needed to compensate for the 'washed out look that
  //that results from the bilerping of samples, etc.	
  float fCompensator = 1.5f; 

  //Second, we multiply this by our value of 0.5 (in this case a char of 128) which will use to sum the octaves
  unsigned char temp = (unsigned char)(128.0f * fCompensator);

  //then turn that into a DWORD
  DWORD dwValue = ((DWORD)temp << 24) | ((DWORD)temp << 16) | ((DWORD)temp << 8) | (DWORD)temp;

  //update the VB's of our quads for the windspeeds  
  for (int i =0;i<NUMOCTAVES;i++)
  {
    mfNoiseOctavePos[i] += mfNoiseOctaveSpeed[i] * mpWrapper->GetFrameTime();

    if (mfNoiseOctavePos[i] > 2.0f)
      mfNoiseOctavePos[i] -= 1.0f;

    mpCompositeQuadArray[i]->SetUVOffset(mfNoiseOctavePos[i], mfNoiseOctavePos[i]);
    mpCompositeQuadArray[i]->UpdateVB();
  }

  mpCompositeNoiseTexture->BeginScene();
    mpWrapper->mpDevice->SetRenderState(D3DRS_TEXTUREFACTOR, dwValue);

    mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
    mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_TFACTOR);
    mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_MODULATE);

    mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
    mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_DISABLE); //disable other stages
    mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_ALPHAOP, D3DTOP_DISABLE); //disable other stages
    mpWrapper->mpDevice->SetTexture(1,NULL);

    mpWrapper->mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    mpWrapper->mpDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    mpWrapper->mpDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);

    for (i =0;i<NUMOCTAVES;i++)
    {
      mpWrapper->mpDevice->SetTexture(0,mpInterpolatedNoiseOctaveTextureArray[NUMOCTAVES-(i+1)]->mpTexture);
      IawMatrix mTemp(true);
      mpCompositeQuadArray[i]->Render(mTemp, false);
    }
  mpCompositeNoiseTexture->EndScene();

  return S_OK;
}

HRESULT RandTexture::InitQuads()
{
  for(int i=0;i<NUMOCTAVES;i++)
  {
    if (mpRNGQuadArray[i])
      delete mpRNGQuadArray[i];
    if (mpInterpolatingQuadArray[i])
      delete mpInterpolatingQuadArray[i];
    if (mpCompositeQuadArray[i])
      delete mpCompositeQuadArray[i];
  }

  for(i=0;i<NUMOCTAVES;i++)
  {
    int iOctaveSize = 256 / (int)pow(2,(NUMOCTAVES-(i+1)));  //256,128,64,32, 16, 8,  BUT first octave should be 32, so

    mpRNGQuadArray[i] = new IawPlane( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1); //TLVert
    mpRNGQuadArray[i]->Init(mpWrapper);
    mpRNGQuadArray[i]->SetScale(IawVector((float)iOctaveSize)); 

    //this is where we determine how much gets sampled. for example, in 
    //octave 0, which is 32x32 samples, our UV's go from 0 to 32/512, so a 1:1 mapping
    float scale = /*0.5f*/(float)iOctaveSize/RNGLOOKUPTEXTURESIZE;
    mpRNGQuadArray[i]->SetUVScale(scale, scale);

    mpRNGQuadArray[i]->SetShaderID(mpShaderManager->GetShaderId("RNG Lookup Shader"));

    // Now the other quads used for interpolation. The UV's will default to 0-1, so we don't need to change them.

    mpInterpolatingQuadArray[i] = new IawPlane( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1); //TLVert
    mpInterpolatingQuadArray[i]->Init(mpWrapper);
    mpInterpolatingQuadArray[i]->SetScale(IawVector((float)iOctaveSize));

    mpCompositeQuadArray[i] = new IawPlane( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1); //TLVert
    mpCompositeQuadArray[i]->Init(mpWrapper);
    mpCompositeQuadArray[i]->SetScale(IawVector(NOISECOMPOSITETEXTURESIZE));

  }

  return S_OK;
}

/*
HRESULT RandTexture::DestroyWorld()
{
  // TargetChanging cleans up most things, so call it first
  TargetChanging();

  // Delete Textures
  SAFE_DELETE(mpRNGLookupTexture);

  for (int i = 0; i<NUMOCTAVES;i++)
  {
    SAFE_DELETE(mpNoiseOctaveTextureArray[NUMOCTAVES]);
    SAFE_DELETE(mpPreviousNoiseOctaveTextureArray[NUMOCTAVES]);
    SAFE_DELETE(mpInterpolatedNoiseOctaveTextureArray[NUMOCTAVES]);
  }
  SAFE_DELETE(mpCompositeNoiseTexture);

  
  return S_OK;
}
*/
HRESULT RandTexture::UpdateWorld()
{

  UpdateNoiseOctave(0);
  UpdateNoiseOctave(1);
  UpdateNoiseOctave(2);
  UpdateNoiseOctave(3);

  UpdateCompositeNoise();

  mfCurrentTime += mpWrapper->GetFrameTime();
  return S_OK;
}

HRESULT RandTexture::RenderWorld()
{
  // void

  return S_OK;
}

HRESULT RandTexture::TargetChanging()
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

  for(int i=0;i<NUMOCTAVES;i++)
  {
    mpRNGQuadArray[i]->TargetChanging(mpWrapper, true);
    mpInterpolatingQuadArray[i]->TargetChanging(mpWrapper, true);
    mpCompositeQuadArray[i]->TargetChanging(mpWrapper, true);
  }


  if (mpRNGLookupTexture)
  {
    SAFE_RELEASE(mpRNGLookupTexture->mpTexture);

    for (int i = 0; i<NUMOCTAVES;i++)
    {
      SAFE_RELEASE(mpNoiseOctaveTextureArray[NUMOCTAVES]->mpTexture);
      SAFE_RELEASE(mpNoiseOctaveTextureArray[NUMOCTAVES]->mpZBuffer);
      SAFE_RELEASE(mpPreviousNoiseOctaveTextureArray[NUMOCTAVES]->mpTexture);
      SAFE_RELEASE(mpPreviousNoiseOctaveTextureArray[NUMOCTAVES]->mpZBuffer);
      SAFE_RELEASE(mpInterpolatedNoiseOctaveTextureArray[NUMOCTAVES]->mpTexture);
      SAFE_RELEASE(mpInterpolatedNoiseOctaveTextureArray[NUMOCTAVES]->mpZBuffer);
    }
    SAFE_RELEASE(mpCompositeNoiseTexture->mpTexture);
    SAFE_RELEASE(mpCompositeNoiseTexture->mpZBuffer);
  }

  return hr;
}

HRESULT RandTexture::TargetChanged()
{
  HRESULT hr = 0;
  
  byte *pTempArray = new byte[RNGLOOKUPTEXTURESIZE*RNGLOOKUPTEXTURESIZE*3];


  if (mpRNGLookupTexture)
  {
    //init with some random noise
    for (int y = 0; y<RNGLOOKUPTEXTURESIZE;y++)
	{
      for (int x = 0; x<RNGLOOKUPTEXTURESIZE;x++)
	  {
		  long n = x + y * 59;
		  n = (n<<13) ^ n;
  
		  float val = (FP_t)(1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);    
		  unsigned char bytecol = (byte)(0xff * val);
		  
	
		  int index = (y*RNGLOOKUPTEXTURESIZE+x) *3; 
		  pTempArray[index] = bytecol;  //blue
		  pTempArray[index+1] = bytecol; //green
		  pTempArray[index+2] = bytecol; //red
	  }
	}
	SetBitmapBits(mpRNGLookupTexture->mBitmap,RNGLOOKUPTEXTURESIZE*RNGLOOKUPTEXTURESIZE*3, pTempArray);
//	SetBitmapBits(mpRNGLookupTexture->mAlphaBitmap,RNGLOOKUPTEXTURESIZE*RNGLOOKUPTEXTURESIZE*3, pTempArray);

	mpRNGLookupTexture->Restore();


  }

  


  for(int i=0;i<NUMOCTAVES;i++)
  {
	mpRNGQuadArray[i]->TargetChanged(mpWrapper,mpShaderManager,true);
	mpInterpolatingQuadArray[i]->TargetChanged(mpWrapper,mpShaderManager,true);
	mpCompositeQuadArray[i]->TargetChanged(mpWrapper,mpShaderManager,true);
  }

  InitNoiseOctaves();

  SetupShaders();

  UpdateNoiseOctave(0);
  UpdateNoiseOctave(1);
  UpdateNoiseOctave(2);
  UpdateNoiseOctave(3);

  UpdateCompositeNoise();

  return hr;
}

// The shader hierarchy has been set up in InitWorld() already. This is where
// the device dependant components get defined/setup
HRESULT RandTexture::SetupShaders()
{
  HRESULT hr;

  DWORD dwStateBlockTemp;

  hr = mpWrapper->mpDevice->BeginStateBlock();
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, 0);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    hr = mpWrapper->mpDevice->SetTexture(0,mpRNGLookupTexture->mpTexture);

    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_DISABLE);
    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    hr = mpWrapper->mpDevice->SetTexture(1,NULL);

    hr = mpWrapper->mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  hr = mpWrapper->mpDevice->EndStateBlock(&dwStateBlockTemp);
  hr = mpShaderManager->SetElementStateBlock("Apply RNG Lookup Texture", dwStateBlockTemp);

  hr = mpWrapper->mpDevice->BeginStateBlock();
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, 0);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTexture(0,mpInterpolatedNoiseOctaveTextureArray[0]->mpTexture);

    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_DISABLE);
    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    hr = mpWrapper->mpDevice->SetTexture(1,NULL);

    hr = mpWrapper->mpDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    hr = mpWrapper->mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  hr = mpWrapper->mpDevice->EndStateBlock(&dwStateBlockTemp);
  hr = mpShaderManager->SetElementStateBlock("Apply Noise Texture 0", dwStateBlockTemp);

  hr = mpWrapper->mpDevice->BeginStateBlock();
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, 0);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTexture(0,mpInterpolatedNoiseOctaveTextureArray[1]->mpTexture);

    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_DISABLE);
    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    hr = mpWrapper->mpDevice->SetTexture(1,NULL);

    hr = mpWrapper->mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    hr = mpWrapper->mpDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
  hr = mpWrapper->mpDevice->EndStateBlock(&dwStateBlockTemp);
  hr = mpShaderManager->SetElementStateBlock("Apply Noise Texture 1", dwStateBlockTemp);

  hr = mpWrapper->mpDevice->BeginStateBlock();
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, 0);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTexture(0,mpInterpolatedNoiseOctaveTextureArray[2]->mpTexture);

    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_DISABLE);
    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    hr = mpWrapper->mpDevice->SetTexture(1,NULL);

    hr = mpWrapper->mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    hr = mpWrapper->mpDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
  hr = mpWrapper->mpDevice->EndStateBlock(&dwStateBlockTemp);
  hr = mpShaderManager->SetElementStateBlock("Apply Noise Texture 2", dwStateBlockTemp);

  hr = mpWrapper->mpDevice->BeginStateBlock();
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, 0);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTexture(0,mpInterpolatedNoiseOctaveTextureArray[3]->mpTexture);

    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_DISABLE);
    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    hr = mpWrapper->mpDevice->SetTexture(1,NULL);
    hr = mpWrapper->mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    hr = mpWrapper->mpDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
  hr = mpWrapper->mpDevice->EndStateBlock(&dwStateBlockTemp);
  hr = mpShaderManager->SetElementStateBlock("Apply Noise Texture 3", dwStateBlockTemp);

  return S_OK;
}

void RandTexture::ToggleUpdateAcceleration()
{
  float fTemp = 1.0f;
  if (mbAccelerateUpdates)
  {
    mbAccelerateUpdates = false;
  }
  else
  {
  	mbAccelerateUpdates = true;
  	fTemp = 0.25;
  }

  mfNoiseOctaveUpdateRate[0] = UPDATERATE0 * fTemp;
  mfNoiseOctaveUpdateRate[1] = UPDATERATE1 * fTemp;
  mfNoiseOctaveUpdateRate[2] = UPDATERATE2 * fTemp;
  mfNoiseOctaveUpdateRate[3] = UPDATERATE3 * fTemp;

  float fTemp2 = 1.0f/fTemp;

  mfNoiseOctaveSpeed[0] = OCTAVESPEED0 * fTemp2;
  mfNoiseOctaveSpeed[1] = OCTAVESPEED1 * fTemp2;
  mfNoiseOctaveSpeed[2] = OCTAVESPEED2 * fTemp2;
  mfNoiseOctaveSpeed[3] = OCTAVESPEED3 * fTemp2;

}
