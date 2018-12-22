/* Copyright (C) Chris Maughan, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Chris Maughan, 2001"
 */
#include "nvinc.h"
#include "Random.h"
#include "BranchNoise.h"

#pragma warning( disable : 4244 )

Random BranchNoise::random;

BranchNoise::BranchNoise() : left(0), right(0) {

  branch_noise = random;

}

BranchNoise::~BranchNoise() {

  delete left;
  delete right;
}


BranchNoise* BranchNoise::getLeft() {

  if (!left) left = new BranchNoise();

  return left;
}

BranchNoise* BranchNoise::getRight() {

  if (!right) right = new BranchNoise();

  return right;
}


