/* 
  FloatHack.h -- interface of Chris Lomont's fast and alternative floating point routines
  version 0.5, October, 2005
  www.lomont.org

  Copyright (C) 2002-2006 Chris Lomont

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the author be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  excluding commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  If you want to use this for commercial applications please contact me.

  Chris Lomont, you can contact me from www.lomont.org
*/

#ifndef _FLOATHACK_H
#define _FLOATHACK_H

#include <iostream>

namespace FloatHack { // namespace to hide functions


// call this in your code - tests some basic functionality
// needed for these routines to function properly.
// return true iff architecture passes basic tests for hack compatibility
bool TestArchitecture(void);


/**** functions to find exceptional float values */
// return true if the float is a Not-a-Number (NaN)
bool IsNaN(float fval);
// return true if the float is denormalized, i.e., exp=0 and mant!=0
bool IsDenormalized(float fval);
// return true if the float is a positive OR negative infinity
bool IsInfinity(float fval);
// return true if fval is not infinity or NaN
bool IsValid(float fval);


/**** queries about float values ****/
#if 1 
// macro versions - fast, but downside is they pollute the namespace
// todo - create template versions to remove namespace pollution

#define FI(f) (*((int *) &(f)))          // convert float to int
#define FU(f) (*((unsigned int *) &(f))) // convert float to unsigned int

#define LessThanZero(f)	           (FU(f) > 0x80000000U)
#define LessThanOrEqualsZero(f)	   (FI(f) <= 0)
#define IsZero(f)                  ((FI(f)<<1)==0)
#define GreaterThanOrEqualsZero(f) (FU(f) <= 0x80000000U)
#define GreaterThanZero(f)         (FI(f) > 0)

#else
bool LessThanZero(float fval);
bool LessThanOrEqualsZero(float fval);
bool IsZero(float fval);
bool GreaterThanOrEqualsZero(float fval);
bool GreaterThanZero(float fval);
#endif



/**** The math part of the library ****/

// return the power of 2 from the exponent. Returns 0 for denormalized floats.
int IntLog2(float fval);

// return the power of 2 from the exponent. Handles denormalized floats.
// slower than IntLog2
int IntLog2Exact(float fval);

// return the absolute value of the float
float FastAbs(float fval);
// negate the value
float Negate(float fval);

// compute sqrt quickly. Requires x >= 0.
float FastSqrt1(float fval);
// compute sqrt quickly. Requires x >= 0.
float FastSqrt2(float fval);
// compute 1/sqrt(x) quickly. Requires x>0
float InvSqrt(float fval);

/**** clamping functions ****/

// clamp a float to [0,1]
float Clamp01(float fval);
// clamp a float to [A,B] (requires A<B)
float ClampAB(float fval, float A, float B);
// clamp a float to [0,infinity)
float ClampNonnegative(float fval);


/**** conversion functions ****/
// convert a float to an int and vice versa. Valid for |fval| <= 2^22 - 1, 
// that is, -4194303 <= fval <= 4914303
int FloatToInt(float fval);
float IntToFloat(int ival);

// convert a float to a long and vice versa. Valid for |fval| <= 2^31-1
long  FloatToLong(float fval);

/**** comparison functions ****/

// return true iff aval < bval
bool LessThan(float aval, float bval);
// return true iff aval > bval
bool GreaterThan(float aval, float bval);

// return true iff af and bf are within maxDiff of each other
// in bits.
// maxDiff=1000 is a good value to find close values
bool LomontCompare(float af, float bf, int maxDiff);

/**** misc functions ****/

// fast float swap with XOR
void FastSwap(float & a, float & b);

/**** two functions to split a float into components, and reassemble them *****/

// create a float, given a sign 0 for positive or 1 for negative,
// an unsigned exponent from 0 to 255 (biased by 127), and a mantissa in [0,(1<<23)-1]
float MakeFloat(unsigned int sign, unsigned int exp, unsigned int mant);
// split a float into sign, exponent, and mantissa
// sign is 0 for positive or 1 for negative,
// unsigned exponent is from 0 to 255 (biased by 127), and mantissa is in [0,(1<<23)-1]
void SplitFloat(float fval, unsigned int & sign, unsigned int & exp, unsigned int & mantissa);


/**** two functions to output float values to help understand them *****/

// print out the floating point number line 
void MakeNumberLine(std::ostream & os);
// write out a float value with details
void DumpFloat(float fval, std::ostream & os);


}; // end namespace

#endif // _FLOATHACK_H
// end - FloatHack.h