/* Copyright (C) Guy W. Lecky-Thompson, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Guy W. Lecky-Thompson, 2000"
 */
// Pseudo-Random Number Generator
// (C) 2000 Guy W. Lecky-Thompson

#include <stdlib.h>
// Standard ANSI C header file 
#include <limits.h>
// Standard ANSI C header for type limits

#include "prandgen.h"

// The constructor implementation
pseudorandom::pseudorandom()
{
	this->lSeed = 0;
}

// The destructor implementation
pseudorandom::~pseudorandom()
{
	this->lSeed = 0;
}

long pseudorandom::Rand(long lMax)
{
  long lNewSeed = this->lSeed;
  long lReturn;

  lNewSeed = (this->lGen1 * lNewSeed) + this->lGen2;
  // Use modulo operator to ensure < ulMax

  this->lSeed = lNewSeed;
  lReturn = this->lSeed % lMax;
  if (lReturn < 1) lReturn = lReturn * -1; // Keep it positive
  return lReturn;
}

void pseudorandom::SRand(long lSeed)
{
	this->lSeed = lSeed;
	// Pick two large integers such that
	// one is double the other
	this->lGen2 = 3719;
	this->lGen1 = (lGen2 / 2);
}
