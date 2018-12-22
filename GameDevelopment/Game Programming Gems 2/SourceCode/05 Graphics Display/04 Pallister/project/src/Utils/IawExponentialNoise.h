// IawExponentialNoise.h App Wizard Version 2.0 Beta 1
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

#if !defined(IawExponentialNoise_h)
#define IawExponentialNoise_h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
 * This class generates noise using an exponential formula.
 * This is especially useful for making procedural textures smooth, or
 * generating height maps for procedural terrain.
 */
class IawExponentialNoise
{
public:
  /**
   * Constructor
   * @param lSeed The seed value for psuedo-random number generation.
   * @param numOctaves The number of octaves to use.
   */
  IawExponentialNoise(long lSeed, int numOctaves = 8);

  /** Destructor */
  virtual ~IawExponentialNoise();

  virtual FP_t Value(FP_t x, FP_t y, FP_t z);
  virtual FP_t ExpValue(FP_t x, FP_t y, FP_t z);
  virtual FP_t AmpValue(FP_t x, FP_t y, FP_t z);

  virtual void BuildMap(FP_t originX,
                        FP_t originY,
                        FP_t originZ,
                        FP_t dimX,
                        FP_t dimY,
                        FP_t dimZ,
                        int mapWidth,
                        int mapHeight,
                        FP_t* pMap);

  /** Set the seed for psuedo-random number generation */
  void SetSequence(long seq);

private:
  IawNoiseGenerator* mpNoiseGenerator;
  IawNoiseGenerator* mpExponentGenerator;
  IawNoiseGenerator* mpHeightGenerator;

  FP_t mExpScale,
       mExpMin,
       mExpMax;

  FP_t mHeightMin,
       mHeightMax;
};

#endif // IawExponentialNoise_h

