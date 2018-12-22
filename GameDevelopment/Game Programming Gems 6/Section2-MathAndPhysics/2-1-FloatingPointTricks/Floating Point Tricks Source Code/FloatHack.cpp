/* 
  FloatHack.cpp -- implementation of Chris Lomont's fast and alternative floating point routines
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

// code to implement the floating point hacks
#include <cassert>
#include <iostream>
#include <cstdlib>
#include "FloatHack.h"

#ifdef _DEBUG
#define SAFETY_CHECKS // define this to enable safety asserts, useful for debugging
#endif
// #define C99 // define this if you want to use C99 stuff (although you need to fix other C++ code below, like iostreams)
#define _SIGNED_SHIFT // define this if bitshift of signed ints preserves sign - implementation defined

// these are used throughout to convert types
#ifndef C99
#define _fval_to_ival(fval,ival) (ival) = *(int*)&(fval)
#define _ival_to_fval(ival,fval) (fval) = *(float*)&(ival)
#else
#define _fval_to_ival(fval,ival) memcpy(&ival,&fval,sizeof(float)) // these are safer, but perhaps slower
#define _ival_to_fval(ival,fval) memcpy(&fval,&ival,sizeof(int))   // C99 does not make the *(int*)& types well-defined
#endif

// given an integer representation of a float, compute a mask
// of the sign bit repeated for every bit
#ifdef _SIGNED_SHIFT
#define SIGNMASK(i) ((i)>>31) 
#else
#define SIGNMASK(i) (~((((unsigned int)(i))>>31)-1))
#endif


// todo - place asserts throughout
// todo - use struct to convert option
// todo - check signed int shift, create mask macro
// todo - use conversion functions

namespace FloatHack {
using namespace std;

// a type useful for dealing with interspersed int and float actions on the same data
typedef union {float f; int i; } IntOrFloat;

// return true iff architecture passes basic tests for hack compatibility
bool TestArchitecture(void)
	{
	bool retval = true;                     // assume passes
	retval &= (sizeof(float)==sizeof(int)); // this required, else pick types with same size
	retval &= (4==sizeof(float));           // this required, we derived ideas based on this

#ifdef _SIGNED_SHIFT
	// check shifts of signed ints preserve sign
	int i = -1;	
	i >>= 5;
	retval &= (i<0); // check if we support signed shifts, else do not define _SIGNED_SHIFT
#endif

	return retval;
	} // TestArchitecture

// return true if the float is a Not-a-Number (NaN)
bool IsNaN(float fval)
	{
	unsigned int sign, exp, mantissa;
	SplitFloat(fval,sign,exp,mantissa);
	return (255 == exp) && (0 != mantissa);
	} // IsNaN

// return true if the float is denormalized, i.e., exp=0 and mant!=0
bool IsDenormalized(float fval)
	{
	unsigned int sign, exp, mantissa;
	SplitFloat(fval,sign,exp,mantissa);
	return (0 == exp) && (0 != mantissa);
	} // IsDenormalized

// return true if the float is a positive OR negative infinity
bool IsInfinity(float fval)
	{
	unsigned int sign, exp, mantissa;
	SplitFloat(fval,sign,exp,mantissa);
	return (255 == exp) && (0 == mantissa);
	} // IsInfinity

// return true if fval is not infinity or NaN
bool IsValid(float fval)
	{
	return (!IsNaN(fval)) && (!IsInfinity(fval));
	} // IsValid


#ifndef LessThanZero // not present yet

bool LessThanZero(float fval)
	{
	unsigned int ival;
	_fval_to_ival(fval,ival);
	return ival > 0x80000000UL;
	}

bool LessThanOrEqualsZero(float fval)
	{
	int ival;
	_fval_to_ival(fval,ival);
	return ival <= 0;
	}

bool IsZero(float fval)
	{
	unsigned int ival;
	_fval_to_ival(fval,ival);
	return 0 == (ival&0x7FFFFFFF); // ignore sign
	} // IsZero

bool GreaterThanOrEqualsZero(float fval)
	{
	unsigned int ival;
	_fval_to_ival(fval,ival);
	return ival <= 0x80000000UL;
	}

bool GreaterThanZero(float fval)
	{
	int ival;
	_fval_to_ival(fval,ival);
	return ival > 0;
	}

#endif // #ifndef LessThanZero

int IntLog2(float fval)
	{
	assert(fval > 0);
	unsigned int ival;
	_fval_to_ival(fval,ival);
	return (ival>>23) - 127; // strip out exponent, remove sign, unbias
	} // IntLog2

int IntLog2Exact(float fval)
	{
	assert(fval > 0);
	int ival,exp;
	_fval_to_ival(fval,ival);
	exp = ival >> 23;  // get exponent
	ival &= (1<<23)-1; // get mantissa alone
	if ((exp == 0) && (ival > 0))
		{
		// handle denormalized numbers
		while ((ival & 0x400000) == 0) 
			{ // find highest bit
			ival <<= 1;
			exp--;
			}
		}
	return exp - 127;
	} // IntLog2Exact

float FastAbs(float fval)
	{
	int ival;
	_fval_to_ival(fval,ival);
	ival &= 0x7FFFFFFF;       // strip sign bit
	_ival_to_fval(ival,fval);
	return fval;
	} // FastAbs

// negate the value
float Negate(float fval)
	{
	int ival;
	_fval_to_ival(fval,ival);
	ival ^= 0x80000000;       // flip sign bit
	_ival_to_fval(ival,fval);
	return fval;
	} // Negate


// compute sqrt(x)
// assumes x >= 0
float FastSqrt1(float fval)
	{
	assert(fval >= 0);
	int ival;
	_fval_to_ival(fval,ival);
	if (0 == ((ival >> 23)&255))
		return 0; // close
	return fval*InvSqrt(fval);
	} // FastSqrt1

// compute sqrt(x)
// assumes x >= 0
float FastSqrt2(float fval)
	{
	assert(fval >= 0);
	float retval;
	int ival;
	_fval_to_ival(fval,ival);
	ival &=0x7FFFFFFF;  // handle the -0 case, else may get garbage!
	ival -= 0x3f800000; // subtract 127 from biased exponent
#ifdef _SIGNED_SHIFT
	ival >>= 1;         // requires signed shift to preserve sign
#else
	assert(0); // todo - unsupported?
#endif
	ival += 0x3f800000; // rebias the new exponent
	_ival_to_fval(ival,retval);
	return retval;
	} // FastSqrt2

// compute 1/sqrt(x)
// assumes x > 0
float InvSqrt(float x)
	{
    // to see why works, read paper on www.lomont.org
	float xhalf = 0.5f*x;
	int i;
	_fval_to_ival(x,i);      // get bits for floating value
	i = 0x5f375a86 - (i>>1); // gives initial guess y0 with magic number
	_ival_to_fval(i,x);      // convert bits back to float
	x = x*(1.5f-xhalf*x*x);  // Newton step, repeating increases accuracy
	return x;
	} // InvSqrt

float Clamp01(float fval)
	{
	int s;
	IntOrFloat val;
	val.f = fval;
	s = SIGNMASK(val.i); // all 1's if sign bit 1
	val.i &= ~s; // 0 if was negative

	// clamp to 1
	val.f = 1.0f - val.f;
	s = SIGNMASK(val.i); // all 1's if sign bit 1
	val.i &= ~s; // 0 if was negative
	val.f = 1.0f - val.f;

	assert((0 <= val.f) && (val.f <= 1.0f));
	return val.f;	
	} // Clamp01

float ClampAB(float fval, float A, float B)
	{
	assert(A < B); // todo - make asserts have message also
	fval -= A;
	fval /= (B-A);
	fval = Clamp01(fval);
	fval *= (B-A);
	fval += A;
	assert((A <= fval) && (fval <= B));
	return fval;
	} // ClampAB

float ClampNonnegative(float fval)
	{
	int s;
	IntOrFloat val;
	val.f = fval;
	s = SIGNMASK(val.i); // all 1's if sign bit 1
	val.i &= ~s; // 0 if was negative

	assert(0 <= val.f);
	return val.f;	
	} // Clamp01

int FloatToInt(float fval)
	{
	// float version goes as follows - the double upcast gives better range - time for faster?
		// convert
	fval += (1<<23) + (1<<22);
	int ival;
	_fval_to_ival(fval,ival);
	ival &= (1<<23)-1; // mask out
	ival -= (1<<22);              	
	return ival;
	} // FloatToInt

float IntToFloat(int ival)
	{
	IntOrFloat val,bias;
	val.i = ival;
	bias.i = ((23+127)<<23)+(1<<22);
	val.i += bias.i;
	val.f -= bias.f;
	return val.f;
	} // IntToFloat

long FloatToLong(float fval)
	{
	// upcast to get enough bits, value is 0x59C00000 = 2^51+2^52
    double dtemp = fval + (((65536.0 * 65536.0 * 16.0) + 32768.0) * 65536.0);
    return (*(long *) &dtemp) - 0x80000000;
	} // FloatToLong


// return true iff aval < bval
bool LessThan(float aval, float bval)
	{
	int ai;
	int bi;
	_fval_to_ival(aval,ai);
	_fval_to_ival(bval,bi);
	int test = ai&bi&0x80000000; // check sign bits
	if (test)
		return bi < ai; // both negative, reverse compare
	return ai < bi;
	} // LessThan

// return true iff aval > bval
bool GreaterThan(float aval, float bval)
	{
	int ai;
	int bi;
	_fval_to_ival(aval,ai);
	_fval_to_ival(bval,bi);
	int test = ai&bi&0x80000000; // check sign bits
	if (test)
		return ai < bi; // both negative, reverse compare
	return bi < ai;
	} // GreaterThan


bool LomontCompare(float af, float bf, int maxDiff)
	{ // fast routine, portable across compilers
	  // constant time execution
	  // to see why works, read paper on www.lomont.org
	int ai;
	int bi;
	_fval_to_ival(af,ai);
	_fval_to_ival(bf,bi);
	int test = SIGNMASK(ai^bi);
	assert((0 == test) || (0xFFFFFFFF == test));
	int diff = (((0x80000000 - ai)&(test)) | (ai & (~test))) - bi;
	int v1 = maxDiff + diff;
	int v2 = maxDiff - diff;
	return (v1|v2) >= 0;
	} // LomontCompare

// fast float swap with XOR
void FastSwap(float & a, float & b)
	{ // shows how to do - to be useful you must inline the code
	int ai;
	int bi;
	_fval_to_ival(a,ai);
	_fval_to_ival(b,bi);
	
	// classic no temp swap trick
	ai ^= bi;
	bi ^= ai;
	ai ^= bi;

	_ival_to_fval(ai,a);
	_ival_to_fval(bi,b);
	} // FastSwap


// create a float, given a sign 0 for positive or 1 for negative,
// an unsigned exponent from 0 to 255 (biased by 127), and a mantissa in [0,(1<<23)-1]
float MakeFloat(unsigned int sign, unsigned int exp, unsigned int mant)
	{
	unsigned int val = (sign<<31) | ((exp&255)<<23) | (mant & ((1<<23)-1));
	float f = *(float*)&val;
	return f;
	} // MakeFloat

// split a float into sign, exponent, and mantissa
// sign is 0 for positive or 1 for negative,
// signed exponent is from -128 to 127, and mantissa is in [0,(1<<23)-1]
void SplitFloat(float fval, unsigned int & sign, unsigned int & exp, unsigned int & mantissa)
	{
	unsigned int ival; // want unsigned to avoid shift ambiguity
	_fval_to_ival(fval,ival);
	sign = (ival>>31) != 0 ? 1 : 0;
	exp  = (ival >> 23) & 255;
	mantissa = ival & ((1<<23)-1);
	} // SplitFloat

void MakeNumberLine(ostream & os)
	{
	int val;
	float * a = reinterpret_cast<float*>(&val);

	os << "Negative NaNs\n";
	*a = MakeFloat(1,255,(1<<23)-1);
	for (int pos = 0; pos < 3; ++pos, --val)
		DumpFloat(*a,os);
	os << "...\n";
	*a = MakeFloat(1,254,(1<<23)-1);
	val += 4;
	for (int pos = 0; pos < 3; ++pos, --val)
		DumpFloat(*a,os);

	os << "\nNegative infinity\n";
	*a = MakeFloat(1,255,0);
	DumpFloat(*a,os);

	os << "\nNegative normalized\n";
	*a = MakeFloat(1,254,(1<<23)-1);
	for (int pos = 0; pos < 3; ++pos, --val)
		DumpFloat(*a,os);
	os << "...\n";  
	*a = MakeFloat(1,1,2);
	for (int pos = 0; pos < 3; ++pos, --val)
		DumpFloat(*a,os);


	os << "\nNegative denormalized\n";
	*a = MakeFloat(1,0,(1<<23)-1);
	for (int pos = 0; pos < 3; ++pos, --val)
		DumpFloat(*a,os);
	os << "...\n";  
	*a = MakeFloat(1,0,0);
	val += 3;
	for (int pos = 0; pos < 3; ++pos, --val)
		DumpFloat(*a,os);

	os << "\nNegative zero\n";
	*a = MakeFloat(1,0,0);
	DumpFloat(*a,os);


	os << "\nPositive zero\n";
	*a = MakeFloat(0,0,0);
	DumpFloat(*a,os);

	os << "\nPositive denormalized\n";
	*a = MakeFloat(0,0,1);
	for (int pos = 0; pos < 3; ++pos, ++val)
		DumpFloat(*a,os);
	os << "...\n";  
	*a = MakeFloat(0,0,(1<<23)-3);
	for (int pos = 0; pos < 3; ++pos, ++val)
		DumpFloat(*a,os);

	os << "\nPositive normalized\n";
	*a = MakeFloat(0,1,0);
	for (int pos = 0; pos < 3; ++pos, ++val)
		DumpFloat(*a,os);
	os << "...\n";  
	*a = MakeFloat(0,254,(1<<23)-3);
	for (int pos = 0; pos < 3; ++pos, ++val)
		DumpFloat(*a,os);

	os << "\nPositive infinity\n";
	*a = MakeFloat(0,255,0);
	DumpFloat(*a,os);

	os << "\nPositive NaNs\n";
	*a = MakeFloat(0,255,1);
	for (int pos = 0; pos < 3; ++pos, ++val)
		DumpFloat(*a,os);
	os << "...\n";  
	*a = MakeFloat(0,255,(1<<23)-3);
	for (int pos = 0; pos < 3; ++pos, ++val)
		DumpFloat(*a,os);

	os << "\nEXTRA***********************\n";
	*a = MakeFloat(0,255,0);
	for (int pos = 0; pos < (1<<23); ++pos, ++val)
		{
		char text[1000];
		sprintf(text,"%f",*a);
		if ('Q' != text[3])
			DumpFloat(*a,os);
		}

	*a = MakeFloat(1,255,0);
	for (int pos = 0; pos < (1<<23); ++pos, ++val)
		{
		char text[1000];
		sprintf(text,"%f",*a);
		if ('Q' != text[4])
			DumpFloat(*a,os);
		}
	} // MakeNumberLine


void DumpFloat(float fval, ostream & os)
	{ // todo dump sign,exp,mant also?
	os << fval << "\t ";
	hex(os);
	os << *reinterpret_cast<int*>(&fval);
	dec(os);
	os << "\t " << *reinterpret_cast<int*>(&fval) << endl;
	} // DumpFloat


	} // end namespace
// end - FloatHack.cpp