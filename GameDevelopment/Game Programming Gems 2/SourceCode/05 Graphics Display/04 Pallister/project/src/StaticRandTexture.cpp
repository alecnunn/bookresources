// StaticRandTexture.cpp App Wizard Version 2.0 Beta 1
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

#define RNGLOOKUPTEXTURESIZE 256.0f //size used by the 
#define NOISEOCTAVETEXTURESIZE 256.0f //size used by the 
#define NOISECOMPOSITETEXTURESIZE 256.0f //size used by the 


// Constructor
StaticRandTexture::StaticRandTexture()
{
  mpWrapper       = NULL;
  mpShaderManager = NULL;

  mpRNGLookupTexture = NULL;

  for (int i=0;i<NUMOCTAVES;i++)
  {
    mpNoiseOctaveTextureArray[i] = NULL;
    mpRNGQuadArray[i]            = NULL;
  }
  mpCompositeQuad         = NULL;
  mpCompositeNoiseTexture = NULL;
}

// Destructor
StaticRandTexture::~StaticRandTexture()
{
  mpWrapper       = NULL;
  mpShaderManager = NULL;
  for (int i = 0; i < NUMOCTAVES; i++)
  {
    delete mpRNGQuadArray[i];
    delete mpNoiseOctaveTextureArray[i];
  }
  SAFE_DELETE(mpCompositeQuad);
  SAFE_DELETE(mpRNGLookupTexture);
  SAFE_DELETE(mpCompositeNoiseTexture);
}


HRESULT StaticRandTexture::InitWorld(IawD3dWrapper* pWrapper)
{
  mpWrapper = pWrapper;

  int i = 0;

  //miRNGLookupTextureSize = (int)RNGLOOKUPTEXTURESIZE;

  mpRNGLookupTexture = new IawTexture(mpWrapper);
  mpRNGLookupTexture->CreateTexture((int)RNGLOOKUPTEXTURESIZE,
                                    (int)RNGLOOKUPTEXTURESIZE,
                                    32,
                                    0,
                                    IawTexture::IAW_TEXTR_32_BITS_PER_PIXEL,
                                    NULL);

  int shader, imp, comp, element;
  i = 0;

  mpShaderManager->CreateShader(&shader, "Static RNG Lookup Shader");   
    mpShaderManager->CreateShaderImplementation(shader, &imp, "RNGL Ver 1");
      mpShaderManager->CreateShaderComponent(shader, imp, &comp, "RNGL V1, Comp 1");
        mpShaderManager->CreateElement(&element, "Static Apply RNG Lookup Texture");
        mpShaderManager->SetComponentElement(shader, imp, comp, element);

  InitNoiseOctaves();

  InitQuads();

  return S_OK;
}


HRESULT StaticRandTexture::InitNoiseOctaves(/*int iNumOctaves*/)
{
  HRESULT hr;

  for (int i=0;i<NUMOCTAVES;i++)
  {
    if (mpNoiseOctaveTextureArray[i])
    {
      delete mpNoiseOctaveTextureArray[i];
      mpNoiseOctaveTextureArray[i] = NULL;
    }
  }

  SAFE_DELETE(mpCompositeNoiseTexture);

  //note that we index into the arrays in reverse order here, as we want the highest
  // frequencies to be the last elements in the array.  
  for (i=0;i<NUMOCTAVES;i++)
  {
    mpNoiseOctaveTextureArray[NUMOCTAVES-(i+1)] = new IawRenderTarget(mpWrapper);

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
  }

  mpCompositeNoiseTexture = new IawRenderTarget(mpWrapper);

  if (FAILED(hr = 
    mpCompositeNoiseTexture->CreateTexture((int)NOISECOMPOSITETEXTURESIZE, 
                  (int)NOISECOMPOSITETEXTURESIZE, 32, 0,
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
HRESULT StaticRandTexture::UpdateNoiseOctave(int iOctave)
{
  mpNoiseOctaveTextureArray[iOctave]->BeginScene();
  IawMatrix mTemp(true);
  mpRNGQuadArray[iOctave]->Render(mTemp, true);
  mpNoiseOctaveTextureArray[iOctave]->EndScene();
  return S_OK;
}

HRESULT StaticRandTexture::UpdateCompositeNoise()
{
  //we want to do the following:
  //
  // Composite = 1/2 Octave0 + 1/4 Octave1 + 1/8 Octave2...
  //
  // So we'll do it as follows:

  // first: 
  //magic number needed to compensate for the 'washed out' look that
  //that results from the bilerping of samples, etc.	
  float fCompensator = 1.5f; 

  //again, we normalize because we are only using 4 terms in this series
  //fCompensator *= (1.0f/(0.5f +0.25f+0.125f+0.0625f));

  //Second, we multiply this by our value of 0.5 (in this case a char of 128) which will use to sum the octaves
  unsigned char temp = (unsigned char)(128.0f * fCompensator);

  //then turn that into a DWORD
  DWORD dwValue = ((DWORD)temp << 24) | ((DWORD)temp << 16) | ((DWORD)temp << 8) | (DWORD)temp;

  mpCompositeNoiseTexture->BeginScene();
    mpWrapper->mpDevice->SetRenderState(D3DRS_TEXTUREFACTOR, dwValue);
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

    mpWrapper->mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    mpWrapper->mpDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    mpWrapper->mpDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
    //mpWrapper->mpDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    for (int i =0;i<NUMOCTAVES;i++)
    {
      mpWrapper->mpDevice->SetTexture(0,mpNoiseOctaveTextureArray[NUMOCTAVES-(i+1)]->mpTexture);
      IawMatrix mTemp(true);
      mpCompositeQuad->Render(mTemp, false);
    }
  mpCompositeNoiseTexture->EndScene();

  return S_OK;
}


HRESULT StaticRandTexture::InitQuads()
{
  for(int i=0;i<NUMOCTAVES;i++)
  {
    if (mpRNGQuadArray[i])
      delete mpRNGQuadArray[i];
  }
  if (mpCompositeQuad)
    delete mpCompositeQuad;


  for(i=0;i<NUMOCTAVES;i++)
  {
    int iOctaveSize = 256 / (int)pow(2,(NUMOCTAVES-(i+1)));  //256,128,64,32, 16, 8,  BUT first octave should be 32, so

    mpRNGQuadArray[i] = new IawPlane( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | 
      D3DFVF_SPECULAR | D3DFVF_TEX1); //TLVert
    mpRNGQuadArray[i]->Init(mpWrapper);
    mpRNGQuadArray[i]->SetScale(IawVector((float)iOctaveSize)); 

    //this is where we determine how much gets sampled. for example, in 
    //octave 0, which is 32x32 samples, our UV's go from 0 to 32/512, so a 1:1 mapping
    float scale = /*0.5f*/(float)iOctaveSize/RNGLOOKUPTEXTURESIZE;
    mpRNGQuadArray[i]->SetUVScale(scale, scale);

    mpRNGQuadArray[i]->SetShaderID(mpShaderManager->GetShaderId("Static RNG Lookup Shader"));

    // Now the other quads used for interpolation. The UV's will default to 0-1, so we don't need to change them.
  }

  mpCompositeQuad = new IawPlane( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1); //TLVert
  mpCompositeQuad->Init(mpWrapper);
  mpCompositeQuad->SetScale(IawVector(NOISECOMPOSITETEXTURESIZE));

  return S_OK;
}


HRESULT StaticRandTexture::TargetChanging()
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
  }
  mpCompositeQuad->TargetChanging(mpWrapper, true);


  if (mpRNGLookupTexture)
  {
    SAFE_RELEASE(mpRNGLookupTexture->mpTexture);
  }

  return hr;
}

HRESULT StaticRandTexture::TargetChanged()
{
  HRESULT hr = 0;

  byte *pTempArray = new byte[(int)RNGLOOKUPTEXTURESIZE*(int)RNGLOOKUPTEXTURESIZE*3];

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


        int index = (y*(int)RNGLOOKUPTEXTURESIZE+x) *3; 
        pTempArray[index] = bytecol;  //blue
        pTempArray[index+1] = bytecol; //green
        pTempArray[index+2] = bytecol; //red
      }
    }
    SetBitmapBits(mpRNGLookupTexture->mBitmap,(int)RNGLOOKUPTEXTURESIZE*(int)RNGLOOKUPTEXTURESIZE*3, pTempArray);
    //	SetBitmapBits(mpRNGLookupTexture->mAlphaBitmap,RNGLOOKUPTEXTURESIZE*RNGLOOKUPTEXTURESIZE*3, pTempArray);

    mpRNGLookupTexture->Restore();

  }

  for(int i=0;i<NUMOCTAVES;i++)
  {
    mpRNGQuadArray[i]->TargetChanged(mpWrapper,mpShaderManager,true);
  }
  mpCompositeQuad->TargetChanged(mpWrapper,mpShaderManager,true);

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
HRESULT StaticRandTexture::SetupShaders()
{
  HRESULT hr;
  
  
  DWORD dwRNGsb;
  hr = mpWrapper->mpDevice->BeginStateBlock();
  hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, 0);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    //hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    hr = mpWrapper->mpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    hr = mpWrapper->mpDevice->SetTexture(0,mpRNGLookupTexture->mpTexture);

    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_DISABLE); //disable other stages
    hr = mpWrapper->mpDevice->SetTextureStageState(1,D3DTSS_ALPHAOP, D3DTOP_DISABLE); //disable other stages
    hr = mpWrapper->mpDevice->SetTexture(1,NULL);

    //hr = mpWrapper->mpDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE); //KP-REMOVE
    hr = mpWrapper->mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  hr = mpWrapper->mpDevice->EndStateBlock(&dwRNGsb);
  hr = mpShaderManager->SetElementStateBlock("Static Apply RNG Lookup Texture", dwRNGsb);

  return S_OK;
}
