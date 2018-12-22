/*
---------------------------------------------------------------------------------------------------
Meshula Physics Demo
Created for Games Programming Gems IV
Copyright (c) 2003 Nick Porcino, http://meshula.net

The MIT License: http://www.opensource.org/licenses/mit-license.php

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or 
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

---------------------------------------------------------------------------------------------------
*/

#include "PMath.h"

using namespace PMath;

void PMath::QuatToBasis(Mat44& pResult, const Quaternion a)
{
	Real x = a[0];
	Real y = a[1];
	Real z = a[2];
	Real w = a[3];

	Real x2 = x + x; 
	Real y2 = y + y; 
	Real z2 = z + z;
	
	Real xx = x * x2;
	Real xy = x * y2;
	Real xz = x * z2;
	Real yy = y * y2;
	Real yz = y * z2;
	Real zz = z * z2;
	Real wx = w * x2;
	Real wy = w * y2;
	Real wz = w * z2;

	pResult[ 0] = k1 - (yy + zz);
	pResult[ 1] = xy - wz;
	pResult[ 2] = xz + wy;

	pResult[ 4] = xy + wz;
	pResult[ 5] = k1 - (xx + zz);
	pResult[ 6] = yz - wx;

	pResult[ 8] = xz - wy;
	pResult[ 9] = yz + wx;
	pResult[10] = k1 - (xx +yy);
}

/*
	This function is based on equation 11 in

	Rotating Objects Using Quaternions
	Nick Bobick
	Game Developer Magazine, Feb. 1998, pp. 34-42
 */

void PMath::InputAngularVelocity(Quaternion& result, Real dt, const Vec3f a)
{
	Quaternion velquat;
	velquat[0] = a[0];	// x
	velquat[1] = a[1];	// y
	velquat[2] = a[2];  // z
	velquat[3] = k0;    // w

	Multiply(velquat, velquat, a);
	Real scale = dt * kHalf;
	result[0] += velquat[0] * scale;
	result[1] += velquat[1] * scale;
	result[2] += velquat[2] * scale;
	result[3] += velquat[3] * scale;

	Normalize(result, result);
}

void PMath::Normalize(Vec3f& result, const Vec3f source)
{
	Real square = Sqr(source[0]) + Sqr(source[1]) + Sqr(source[2]);
	if (square > k0)	// if vector is zero, don't do anything
	{
		Real mag = RecipSqrt(square);
		result[0] = source[0] * mag; 
		result[1] = source[1] * mag; 
		result[2] = source[2] * mag; 
	}
}

void PMath::Normalize(Quaternion& result, const Quaternion source)
{
	Real square = Sqr(source[0]) + Sqr(source[1]) + Sqr(source[2]) + Sqr(source[3]);
	if (square > k0)	// if Quaternion is zero, don't do anything
	{
		Real mag = RecipSqrt(square);
		result[0] = source[0] * mag; 
		result[1] = source[1] * mag; 
		result[2] = source[2] * mag; 
		result[3] = source[3] * mag; 
	}
}
