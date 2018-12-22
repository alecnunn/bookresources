/* 
  TestFloatHack.cpp -- test of Chris Lomont's fast and alternative floating point routines
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


// code to test the floating point hack routines
// Chris Lomont (code@lomont.org)
// tested with Visual Studio.NET 2003


#include <iostream>
#include <cmath>
#include <cassert>
#include "Floathack.h"

using namespace std;
using namespace FloatHack; // for the hacked functions

void TestExceptional(void)
	{  // test exceptional functions
	float a,b;
	cout << "All expression should return 1\n";
	a = -1;
	cout << "-1 is a number: " << IsValid(a) << endl;
	b = sqrt(a);
	cout << "sqrt(-1) is a NaN: " << IsNaN(b) << endl;

	a = MakeFloat(0,0,0); // make 0 this way to prevent optimizer from removing
	b = 1/a;
	cout << "1/0 is infinity: " << IsInfinity(b) << endl;

	// create denormalized value:
	a = MakeFloat(0,1,0); // smallest normalized number
	cout << "Normalized: " << !IsDenormalized(a) << endl;
	a = a/2; // now denormalized
	cout << "Denormalized: " << IsDenormalized(a) << endl;

	cout << "\n\n";
	} // TestExceptional


void TestQueries(void)
	{ // test query functions
	float vals[] = {-1.0f,MakeFloat(1,0,1),0.0f,MakeFloat(0,0,1),1.0f};
	cout << "TestQueries for -1,-eps,0,eps,1 for\n";

	cout << "LessThanZero, ";
	cout << "LessThanOrEqualsZero, ";
	cout << "IsZero, ";
	cout << "GreaterThanOrEqualsZero\n";
	cout << "GreaterThanZero, ";

	for (int pos = 0; pos < 5; ++pos)
		{
		float a = vals[pos];
		cout << "Value " << a << endl;
		cout << LessThanZero(a);
		cout << LessThanOrEqualsZero(a);
		cout << IsZero(a);
		cout << GreaterThanOrEqualsZero(a);
		cout << GreaterThanZero(a);
		cout << endl;
		}
	cout << "\n\n";
	} // TestQueries

float Error(float exactVal, float fastVal)
	{
	if (exactVal == 0) return (fabs(fastVal) < 0.00001 ? 0.0f : 1.0f);
	return fabs((exactVal-fastVal)/exactVal);
	} // Error

void TestMath(void)
	{ // test math functions on various exponent/sign varioations
	unsigned int exps[]  = {1,1,2,126,127,128,253,254};
	
	cout << "TestMath\n";

	for (int outer = 0; outer < 8; ++outer)
		for (int sign = 0; sign <= 1; ++sign)
			for (int p = 0; p < 1000000; ++p)
				{
				int mant = rand() & ((1<<23)-1);
				float fval,fastVal,exactVal;
				fval = MakeFloat(sign,exps[outer],mant);
				fastVal = FastAbs(fval);
				exactVal = fabs(fval);
				if (fastVal != exactVal)
					cout << "FastAbs failed on " << fval << endl;

				if ((fval > 0) && (false == IsDenormalized(fval)))
					{
					fastVal  = InvSqrt(fval);
					exactVal = 1.0f / sqrt(fval);
					if (fabs(fastVal/exactVal-1) > 0.01)
						cout << "InvSqrt failed on " << fval << endl;

					fastVal  = static_cast<float>(IntLog2(fval));
					exactVal = static_cast<float>(floor(log10(fval)/log10(2.0)+0.5));
					if (fastVal != exactVal)
						cout << "IntLog2 failed on " << fval << endl;

					fastVal  = static_cast<float>(IntLog2Exact(fval));
					exactVal = static_cast<float>(floor(log10(fval)/log10(2.0)+0.5));
					if (fastVal != exactVal)
						cout << "IntLog2Exact failed on " << fval << endl;
					}

				if (fval >= 0)
					{
					fastVal = FastSqrt1(fval);
					exactVal = sqrt(fval);
					if (Error(fastVal,exactVal) > 0.01)
						cout << "FastSqrt1 failed on " << fval << endl;

					fastVal = FastSqrt2(fval);
					exactVal = sqrt(fval);
					if (Error(fastVal,exactVal) > 0.10)
						cout << "FastSqrt2 failed on " << fval << endl;
					}
				} // for each float value
	cout << "\n\n";
	} // TestMath

void TestClamp(void)
	{
	cout << "TestClamp\n";

	cout << "-5 clamped to [0,1] is " << Clamp01(-5) << endl;
	cout << "0 clamped to [0,1] is " << Clamp01(0) << endl;
	cout << "0.5 clamped to [0,1] is " << Clamp01(0.5f) << endl;
	cout << "1 clamped to [0,1] is " << Clamp01(1) << endl;
	cout << "1.1 clamped to [0,1] is " << Clamp01(1.1f) << endl;
	cout << "500.125 clamped to [0,1] is " << Clamp01(500.125f) << endl;

	cout << "-5 clamped to [-4,3] is " << ClampAB(-5,-4,3) << endl;
	cout << "-5 clamped to [-5,3] is " << ClampAB(-5,-5,3) << endl;
	cout << "-5 clamped to [-6,3] is " << ClampAB(-5,-6,3) << endl;
	cout << "-5 clamped to [-8,-6] is " << ClampAB(-5,-8,-6) << endl;

	cout << "-5 clamped to [0,infinity) is " << ClampNonnegative(-5) << endl;
	cout << "-1 clamped to [0,infinity) is " << ClampNonnegative(-1) << endl;
	cout << "0 clamped to [0,infinity) is " << ClampNonnegative(0) << endl;
	cout << "0.01 clamped to [0,infinity) is " << ClampNonnegative(0.01f) << endl;
	cout << "10000 clamped to [0,infinity) is " << ClampNonnegative(10000) << endl;
	cout << endl;
	
	} // TestClamp

bool TestConvert(long val)
	{
	float fval;
	fval = static_cast<float>(val);
	val  = FloatToLong(fval);
	if (fabs(fval - val) > 0.005f)
		{
		cout << "FloatToLong failed on " << val << endl;
		return false;
		}
	return true;
	} // TestConvert

void TestConversion(void)
	{
	cout << "TestConversion\n";

	
	cout << "FloatToInt 1234: " << FloatToInt(1234) << endl;
	cout << "FloatToInt -123.45: " << FloatToInt(-123.45f) << endl;
	cout << "FloatToInt 0.05: " << FloatToInt(0.05f) << endl;
	cout << "FloatToInt -0.05: " << FloatToInt(-0.05f) << endl;
	cout << "FloatToInt -0.55: " << FloatToInt(-0.55f) << endl;
	cout << "FloatToInt -0.50: " << FloatToInt(-0.50f) << endl;
	cout << "FloatToInt -0.45: " << FloatToInt(-0.45f) << endl;
	cout << "FloatToInt 0.55: " << FloatToInt(0.55f) << endl;
	cout << "FloatToInt 0.50: " << FloatToInt(0.50f) << endl;
	cout << "FloatToInt 0.45: " << FloatToInt(0.45f) << endl;

	cout << "IntToFloat 100: "  << IntToFloat(100) << endl;
	cout << "IntToFloat -100: " << IntToFloat(-100) << endl;
	cout << "IntToFloat 0: "    << IntToFloat(0) << endl;

	cout << "FloatToLong 1234: " << FloatToLong(1234) << endl;
	cout << "FloatToLong -123.45: " << FloatToLong(-123.45f) << endl;
	cout << "FloatToLong 0.05: " << FloatToLong(0.05f) << endl;
	cout << "FloatToLong -0.05: " << FloatToLong(-0.05f) << endl;

	bool passed = true;
	int top = (1<<22)-1;

	cout << "Testing Int Conversion Ranges...\n";
	for (int val = -top; val <= top; ++val)
		{
		float fval;
		int ival;
		fval = IntToFloat(val);
		ival = FloatToInt(fval);
		if (fabs(fval - val) > 0.005)
			{
			if (passed == true)
				cout << "Error: IntToFloat failed on " << val << endl;
			passed = false;
			}
		if (fabs(fval - ival) > 0.005)
			{
			if (passed == true)
				cout << "Error: FloatToInt failed on " << val << endl;
			passed = false;
			}
		}
	if (true == passed)
		cout << "FloatToInt and IntToFloat are valid over +-" << top << endl;
	else
		cout << "FloatToInt or IntToFloat failed\n";

	cout << "Testing Long Conversion Ranges...\n";
	passed = true;
	for (int val = -top; val <= top; ++val)
		{
		float fval = static_cast<float>(val);
		int ival;
		ival = FloatToLong(fval);
		if (fabs(fval - ival) > 0.005f)
			{
			if (passed == true)
				cout << "Error: FloatToLong failed on " << val << endl;
			passed = false;
			}
		}
	// test some other ranges
	passed &= TestConvert(1<<31);
	passed &= TestConvert(1<<29);
	passed &= TestConvert(1<<28);
	passed &= TestConvert(-(1<<31));
	
	if (true == passed)
		cout << "FloatToLong and LongToFloat are valid over +-" << top << endl;
	else
		cout << "FloatToLong or LongToFloat failed\n";

	cout << "\n\n";
	} // TestConversion

void TestComparison(void)
	{
	// create a lot of values at random, and test the comparison functions

	cout << "TestComparison\n";
	bool passed = true;
	for (int pos = 0; pos < 100000; ++pos)
		{
		unsigned int exp,mant,dist,maxDiff;
		float v1,v2; // we create v1 and v2 close, v1 < v2

		exp  = rand()%254; // avoid overflow values
		mant = (rand()&((1<<21)-1))+1000;
		dist = (rand()&0xFFFF)+5; // ensure nonzero
		maxDiff = rand()&0xFFFF;
		// both positive
		v1 = MakeFloat(0,exp,mant);
		v2 = MakeFloat(0,exp,mant+dist);

		passed &= LessThan(v1,v2);
		passed &= GreaterThan(v2,v1);
		if (dist <= maxDiff)
			passed &= LomontCompare(v1,v2,maxDiff);
		else
			passed &= !LomontCompare(v1,v2,maxDiff);
		// both negative
		v1 = MakeFloat(1,exp,mant+dist);
		v2 = MakeFloat(1,exp,mant);

		passed &= LessThan(v1,v2);
		passed &= GreaterThan(v2,v1);
		if (dist <= maxDiff)
			passed &= LomontCompare(v1,v2,maxDiff);
		else
			passed &= !LomontCompare(v1,v2,maxDiff);
		// mixed sign
		v1 = MakeFloat(1,exp,mant);
		v2 = MakeFloat(0,exp,mant);
		passed &= LessThan(v1,v2);
		passed &= GreaterThan(v2,v1);

		}

	if (true == passed)
		cout << "Compare passed.\n\n";
	else
		cout << "Compare failed.\n\n";
	} // TestComparison


int main(void)
	{
	cout << "Test:" << TestArchitecture() << endl;

	float v = 1.0,v2 = 0.0;

//	control87

	v = MakeFloat(1,0,0);
	v2 = sqrt(v);
	v = FastSqrt2(v2);

	DumpFloat(v2,cout);
	cout << sizeof(double) << ' ' << sizeof(long double) << endl;
	cout << "FastSqrt1(-0) is " << FastSqrt1(v2) << endl;

	int i = 0;
	i -= 0x3f80000;
	i >>= 1;
	i += 0x3f80000;
	v = InvSqrt(0);
	cout << v << " is not 0\n";

//	return 0;

	TestExceptional();
	TestQueries();
	TestMath();
	TestClamp();
	TestConversion();
	TestComparison();

	// test swap
	float a = 1.0f;
	float b = -12345.0f;
	FastSwap(a,b);
	if ((b == 1.0f) && (a == -12345.0f)) // can compare exact here
		cout << "Swap passed\n\n";
	else
		cout << "Swap failed\n\n";
	b = Negate(b);
	if (b == -1.0f)
		cout << "Negate succeeded.\n\n";
	else
		cout << "Negate failed.\n\n";

	return 0;
	} // main

// end - TestFloatHack.cpp
