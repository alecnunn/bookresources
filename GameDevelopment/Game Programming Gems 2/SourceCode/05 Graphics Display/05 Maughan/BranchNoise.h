/* Copyright (C) Chris Maughan, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Chris Maughan, 2001"
 */
#ifndef BranchNoise_INCLUDED
#define BranchNoise_INCLUDED

#include "Random.h"

struct BranchNoise {

  BranchNoise();
  ~BranchNoise();

  float branch_noise;

  BranchNoise* getLeft();  // deleted when parent is deleted
  BranchNoise* getRight();

 private:

  BranchNoise* left;
  BranchNoise* right;

  static Random random;

};
  

#endif
