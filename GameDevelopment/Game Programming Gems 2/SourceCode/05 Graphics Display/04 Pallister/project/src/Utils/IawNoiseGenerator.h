// IawNoiseGenerator.h App Wizard Version 2.0 Beta 1
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

#if !defined(IawNoiseGenerator_h)
#define IawNoiseGenerator_h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
 * This class is a psuedo-random number gereator.
 * A seeded value will generate the same results.  Useful for terrain height
 * map generation.
 */
class IawNoiseGenerator
{
public:
  IawNoiseGenerator(int numOctaves = 8, long seq = 174829);
  virtual ~IawNoiseGenerator();

  FP_t Value(FP_t x, FP_t y, FP_t z);

  virtual void BuildMap(FP_t originX,
                        FP_t originY,
                        FP_t originZ,
                        FP_t dimX,
                        FP_t dimY,
                        FP_t dimZ,
                        int mapWidth,
                        int mapHeight,
                        FP_t* pMap);

  void SetSequence(long seq);

protected:
  long mSeq;
  long mPrimes[16][3];
  int  mNumOctaves;
  
private:
  inline FP_t Noise(int octave, long x, long y);
  inline FP_t Interpolate(FP_t a, FP_t b, FP_t frac);
  inline FP_t InterpolatedNoise(int octave, FP_t x, FP_t y);
};

#endif // IawNoiseGenerator_h

