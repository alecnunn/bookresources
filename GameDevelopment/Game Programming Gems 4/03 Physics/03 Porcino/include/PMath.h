/** @file PMath.h

	Necessary types and utilities for the physics engine
*/
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

#ifndef _PMATH_H_
#define _PMATH_H_

#include <math.h>

typedef unsigned int	uint32;
typedef float			Real;

// common constants

#define kEps	Real(0.0001f)
#define kN1	Real(-1.0f)
#define k0		Real(0.0f)
#define kHalf	Real(0.5f)
#define k1		Real(1.0f)
#define k2		Real(2.0f)
#define k3		Real(3.0f)
#define k4		Real(4.0f)
#define k5		Real(5.0f)
#define k6		Real(6.0f)
#define k7		Real(7.0f)
#define k8		Real(8.0f)
#define k9		Real(9.0f)
#define k10	Real(10.0f)
#define k11	Real(11.0f)
#define k12	Real(12.0f)
#define k1000	Real(1000.0f)

/** @namespace PMath
	@brief Physics Math namespace

	Only as much math as is required by the physics engine is included here

	No SIMD implementation is provided, but this API is designed to map naturally onto
	SSE and VU0 intrinsics

	@todo - modify API to return result, not put result in referenced parameter
 */



namespace PMath {
	typedef Real Vec3f[3];		//!< A SIMD implementation would need 4 components
	typedef Real Quaternion[4];		//!< Stored as x, y, z, w
	typedef Real Mat44[16];			//!< stored in column major format (like OpenGL)

	//! Square a real number
	inline	Real Sqr(Real a)													{ return a * a; }

	//! Square root
	inline	Real Sqrt(Real a)													{ return sqrtf(a); }

	//! Absolute value
	inline	Real Abs(Real a)													{ return (a > 0) ? a : -a; }

	//! Reciprocal square root
	inline	Real RecipSqrt(Real a)												{ return (k1 / sqrtf(a)); }

	//! compare one vector to another
	inline	bool Equal(const Vec3f a, const Vec3f b, Real eps)					{ return (Abs(a[0]-b[0]) + Abs(a[1]-b[1]) + Abs(a[2]-b[2])) < eps; } 

	//! Copy one vector into another
	inline	void Set(Vec3f& a, const Vec3f b)									{ a[0] = b[0]; a[1] = b[1]; a[2] = b[2]; }

	//! return inner product of two vectors
	inline	Real Dot(const Vec3f a, const Vec3f b)								{ return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]; }

	//! Zero a vector
	inline	void Zero(Vec3f& a)													{ a[0] = Real(0.0f); a[1] = Real(0.0f); a[2] = Real(0.0f); }

	//! Return true if vector is zero
	inline bool IsZero(const Vec3f a)											{ return Dot(a, a) <= kEps; }

	//! Get length of vector
	inline	Real Length(const Vec3f a)											{ return Sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]); }

	//! Normalize a vector
			void Normalize(Vec3f& result, const Vec3f source);

	//! Add two vectors
	inline	void Add(Vec3f& a, const Vec3f b)									{ a[0] += b[0]; a[1] += b[1]; a[2] += b[2]; }

	//! Add two vectors
	inline	void Add(Vec3f& result, const Vec3f a, const Vec3f b)				{ result[0] = a[0]+b[0]; result[1] = a[1]+b[1]; result[2] = a[2]+b[2]; }

	//! Subtract two vectors
	inline	void Subtract(Vec3f& a, const Vec3f b)								{ a[0] -= b[0]; a[1] -= b[1]; a[2] -= b[2]; }

	//! Subtract two vectors
	inline	void Subtract(Vec3f& result, const Vec3f a, const Vec3f b)			{ result[0] = a[0]-b[0]; result[1] = a[1]-b[1]; result[2] = a[2]-b[2]; }

	inline	Real Distance(const Vec3f a, const Vec3f b)							{ Vec3f temp; Subtract(temp, a, b); return Length(temp); }

	//! Multiply and accumulate two vectors
	inline	void MultiplyAccumulate(Vec3f& a, Real scale, const Vec3f b)		{ a[0] += scale * b[0]; a[1] += scale * b[1]; a[2] += scale * b[2]; }

	/// linearly interpolate between a and b
	inline	void Lerp(Vec3f& result, Real t, const Vec3f a, const Vec3f b)		{ result[0] = t * a[0] + (k1-t) * b[0]; result[1] = t * a[1] + (k1-t) * b[1]; result[2] = t * a[2] + (k1-t) * b[2]; }

	//! Multiply two vectors, element-wise
	inline	void Multiply(Vec3f&a, const Vec3f b, const Vec3f c)				{ a[0] = b[0] * c[0]; a[1] = b[1] * c[1]; a[2] = b[2] * c[2]; }

	//! calculate outer product of two vectors
	inline	void Cross(Vec3f& result, const Vec3f a, const Vec3f b)				{ result[0] = a[1]*b[2] - a[2]*b[1]; result[1] = a[2]*b[0] - a[0]*b[2]; result[2] = a[0]*b[1] - a[1]*b[0]; }

	//! negate a vector in place
	inline void Negate(Vec3f& a)												{ a[0] *= kN1; a[1] *= kN1; a[2] *= kN1; }

	inline	void Scale(Vec3f& a, Real scale)									{ a[0] *= scale; a[1] *= scale; a[2] *= scale; }

	//! Set a vector to a scale copy of the other
	inline	void SetScaled(Vec3f& a, Real scale, const Vec3f b)					{ a[0] = scale * b[0]; a[1] = scale * b[1]; a[2] = scale * b[2]; }

	//! Create a basis matrix from a quaternion. Only sets upper left 3x3 portion
			void QuatToBasis(Mat44& pResult, const Quaternion a);

	//! Copy one quaternion to another
	inline	void Set(Quaternion& a, const Quaternion b)							{ a[0] = b[0]; a[1] = b[1]; a[2] = b[2]; a[3] = b[3]; }

	//! Set the first 3 elements of the translation column of a matrix
	inline	void SetTranslation(Mat44& pResult, const Vec3f a)					{ pResult[12] = a[0]; pResult[13] = a[1]; pResult[14] = a[2]; }

	//! Set a matrix to identity
	inline	void Identity(Mat44& pResult)										{ for (int i = 1; i < 15; ++i) pResult[i] = k0; 	pResult[0] = pResult[5] = pResult[10] = pResult[15] = k1; }

	//! Update a quaternion's orientation with an angular velocity
			void InputAngularVelocity(Quaternion& result, Real dt, const Vec3f a);

	//! Normalize a quaternion
			void Normalize(Quaternion& result, const Quaternion source);

	//! Multiply two quaternions together
	inline 	void Multiply(Quaternion& result, const Quaternion a, const Quaternion b) {
		Real ax = a[0]; Real ay = a[1]; Real az = a[2]; Real aw = a[3];
		Real bx = b[0]; Real by = b[1]; Real bz = b[2]; Real bw = b[3];
		Real tempx = aw * bx  +  ax * bw  +  ay * bz  -  az * by;
		Real tempy = aw * by  +  ay * bw  +  az * bx  -  ax * bz;
		Real tempz = aw * bz  +  az * bw  +  ax * by  -  ay * bx;
		Real tempw = aw * bw  -  ax * bx  -  ay * by  -  az * bz;
		result[0] = tempx; result[1] = tempy; result[2] = tempz; result[3] = tempw;	// done through temps to allow in-place multiply
	}

	// compound types

	/** @class Plane
	*	Plane encoded as a normal, and the distance of the plane from the origin, along the normal
	*	@todo - vectorize and take functions out of class
	*	for documentation on the math, cf http://www.gamasutra.com/features/19991018/Gomez_1.htm
	*/
	class Plane
	{
	public:
		Plane(const Plane& plane) {
			m_D = plane.m_D;
			Set(m_Normal, plane.m_Normal);
		}

		Plane(const Vec3f point, const Vec3f normal) {
			Set(m_Normal, normal);
			m_D = -Dot(normal, point);
		}

		Plane(const Vec3f p0, const Vec3f p1, const Vec3f p2) {
			Vec3f t1;
			Subtract(t1, p1, p0);
			Vec3f t2;
			Subtract(t2, p2, p0);
			Cross(m_Normal, t1, t2);
			Normalize(m_Normal, m_Normal);
			m_D = - Dot(m_Normal, p0);
		}

		~Plane() { }

		Real DistanceToPoint(const Vec3f a) const {
			return m_D + Dot(m_Normal, a);
		}

		Vec3f	m_Normal;		//!< plane normal
		Real	m_D;			//!< distance of plane from origin, D = -(point dot normal)
	};
}

// no SIMD implementation included with this version of PMath

#ifdef Vec3fSIMD
#undef Vec3fSIMD
#endif

#endif
