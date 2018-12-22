// IawExponentialNoise.cpp App Wizard Version 2.0 Beta 1
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
//
// PURPOSE:
//    
// IawExponentialNoise.cpp: implementation of the CIawExponentialNoise class. 
//   Implements N octaves of noise, which is fed into a power function as:
//
//    Result = Amplitude(x,y) * Pow(SumNoise(x,y), Exponent(x,y))
//
// Separate noise functions are used to drive the amplitude and exponent over distance.
//
// ----------------------------------------------------------------------
// Authors: Kim Pallister,Dean Macri - Intel Technology Diffusion Team
// ----------------------------------------------------------------------

#include "..\IawFrameWork.h"

// Constructor...
IawExponentialNoise::IawExponentialNoise(long lSeed, int numOctaves)
{
  //these three random number generators are used to vary the terrain height and shape over distance
  // the first is for the initial height value. The second is for the exponent used in a power function
  // and the third is amplitude.
  // for any given point Final Height = Amplitude(x,y)*(InitialHeight(x,y)^Exponent(x,y))
  //
  mpNoiseGenerator    = new IawNoiseGenerator(numOctaves, lSeed);
  mpExponentGenerator = new IawNoiseGenerator(4, lSeed + 1);
  mpHeightGenerator   = new IawNoiseGenerator(1, lSeed + 2);

  mExpScale  = 0.2f;
  mExpMin    = 0.33f;//MIN_EXPONENT;	//FOR THIS DEMO I HAVE HARD-CODED
  mExpMax    = 0.33f;//MAX_EXPONENT;	// THE EXPONENTS TO GET A CERTAIN
  mHeightMin = 0.4f;//MIN_HEIGHT;       // LOOK FROM MY TERRAIN.
  mHeightMax = 1.5f;//MAX_HEIGHT;
}

// Destructor...
IawExponentialNoise::~IawExponentialNoise()
{
  SAFE_DELETE(mpNoiseGenerator);
  SAFE_DELETE(mpExponentGenerator);
  SAFE_DELETE(mpHeightGenerator);
}


FP_t IawExponentialNoise::Value(FP_t x, FP_t y, FP_t z)
{
  FP_t height,
       exp,
       sgn,
       height_scale;

  height = mpHeightGenerator->Value(x, y, z);
  sgn = height >= (FP_t)0 ? (FP_t)1 : (FP_t)(-1);
  height_scale = (FP_t)(sgn * pow(fabs(height), 0.5f) * 0.5f + 0.5f) * (mHeightMax - mHeightMin) + mHeightMin;
  exp = mpExponentGenerator->Value(x * mExpScale, y, z * mExpScale);
  sgn = exp >= (FP_t)0 ? (FP_t)1 : (FP_t)(-1);
  exp = (FP_t)(sgn * pow(fabs(exp), 0.5f) * 0.5f + 0.5f) * (mExpMax - mExpMin) + mExpMin;
  height = mpNoiseGenerator->Value(x, y, z);
  sgn = height >= (FP_t)0 ? (FP_t)1 : (FP_t)(-1);

  return (sgn * (FP_t)pow(fabs(height),exp)) * height_scale;
}

FP_t IawExponentialNoise::ExpValue(FP_t x, FP_t y, FP_t z)
{
  FP_t exp,
       sgn;

  exp = mpExponentGenerator->Value(x * mExpScale, y, z * mExpScale);
  sgn = exp >= (FP_t)0 ? (FP_t)1 : (FP_t)(-1);
  exp = (FP_t)(sgn * pow(fabs(exp), 0.5f) * 0.5f + 0.5f) * (mExpMax - mExpMin) + mExpMin;
  return exp;
}

FP_t IawExponentialNoise::AmpValue(FP_t x, FP_t y, FP_t z)
{
  FP_t amp,
       sgn;

  amp = mpHeightGenerator->Value(x, y, z);
  sgn = amp >= (FP_t)0 ? (FP_t)1 : (FP_t)(-1);
  amp = (FP_t)(sgn * pow(fabs(amp), 0.5f) * 0.5f + 0.5f) * (mHeightMax - mHeightMin) + mHeightMin;
  return amp;
}

void IawExponentialNoise::BuildMap(FP_t originX,
                                   FP_t originY,
                                   FP_t originZ,
                                   FP_t dimX,
                                   FP_t dimY,
                                   FP_t dimZ,
                                   int mapWidth,
                                   int mapHeight,
                                   FP_t* pMap)
{
  int i, j;

  FP_t x,
       y,
       z,
       height,
       sgn,
       height_scale;

  FP_t f_i, f_j;

  FP_t f_width  = (FP_t)mapWidth;
  FP_t f_height = (FP_t)mapHeight;
  FP_t exp;  

  for (i=0; i<mapHeight; i++)
  {
    f_i = (float)i / (f_height - 1);
    for (j=0; j<mapWidth; j++)
    {
      f_j = (float)j / (f_width - 1);

      x = originX - dimX / (FP_t)2.0 + dimX * f_j;
      y = originY;
      z = originZ - dimZ / (FP_t)2.0 + dimZ * f_i;
      height = mpHeightGenerator->Value(x, y, z);
      sgn = height >= (FP_t)0 ? (FP_t)1 : (FP_t)(-1);
      height_scale = (FP_t)(sgn * pow(fabs(height), 0.5f) * 0.5f + 0.5f) * (mHeightMax - mHeightMin) + mHeightMin;
      exp = mpExponentGenerator->Value(x * mExpScale, y, z * mExpScale);
      sgn = exp >= (FP_t)0 ? (FP_t)1 :(FP_t)(-1);
      exp = (sgn * (FP_t)pow(fabs(exp), 0.5f) * 0.5f + 0.5f) * (mExpMax - mExpMin) + mExpMin;
      height = mpNoiseGenerator->Value(x, y, z);
      sgn = height >= (FP_t)0 ? (FP_t)1 : (FP_t)(-1);
      pMap[i*mapWidth + j] = sgn * (FP_t)pow(fabs(height),exp) * height_scale;
    }
  }
}

void IawExponentialNoise::SetSequence(long seq)
{
  mpNoiseGenerator->SetSequence(seq);
  mpExponentGenerator->SetSequence(seq+1);
}

