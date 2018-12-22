/* Copyright (C) Dante Treglia II, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dante Treglia II, 2000"
 */
#ifndef _QUATERNION_H
#define _QUATERNION_H
#include "mtxlib.h"

#define ERROR_TOLERANCE 1e-6

class quaternion {
public:
	float x, y, z, w;

public:
	quaternion() : x(0), y(0), z(0), w(1) {};
	quaternion(const quaternion &q) : x(q.x), y(q.y), z(q.z), w(q.w) {};
	quaternion(float xI, float yI,  float zI, float wI) : x(xI), y(yI), 
z(zI), w(wI) {};
	quaternion(vector3 axis, float angle) {
		float d, s;
		d = axis.length();
		assert(d != 0 && "Qauternion Axis is zero!");
		s = sinf(angle * 0.5F) / d;
		SetValues(axis.x * s, axis.y * s, axis.z * s, cosf(angle * 0.5F));
	}
	quaternion(float yaw, float pitch, float roll) {SetEuler(yaw, pitch, 
roll);}

public:
	// Multiply Equal
	quaternion &operator *= (const quaternion &q) {
		quaternion temp = *this * q;
		*this = temp;
		return *this;
	}
	// Factor Equal
	quaternion &operator *= (float s) {
		SetValues(x * s, y * s, z * s, w * s);
		return *this;
	}
	// Add Equal
	quaternion &operator += (const quaternion &q) {
		SetValues(x + q.x, y + q.y, z + q.z, w + q.w);
		return *this;
	}
	// Subtract Equal
	quaternion &operator -= (const quaternion &q) {
		SetValues(x - q.x, y - q.y, z - q.z, w - q.w);
		return *this;
	}
	// Negate
	friend quaternion operator - (const quaternion &a) {
		return quaternion(-a.x, -a.y, -a.z, -a.w);
	}
	// Add
	friend quaternion operator + (const quaternion &a, const quaternion 
&b) {
		return quaternion(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
	}
	// Factor
	friend quaternion operator * (const quaternion &a, float s) {
		return quaternion(a.x * s, a.y * s, a.z * s, a.w * s);
	}
	friend quaternion operator * (float s, const quaternion &a) {
		return quaternion(a.x * s, a.y * s, a.z * s, a.w * s);
	}
	// Multiply
	friend quaternion operator * (const quaternion &a, const quaternion 
&b) {
		return quaternion(
			a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
			a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z,
			a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x,
			a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
		); 
	}
/*
	// Multiply a vector by a quaternion
	friend quaternion operator * (const vector3 &v, const quaternion &q) {
		quaternion t(v, 0);
		return t*q;
	}
*/

public:
	// Values
	quaternion &SetValues(float xI, float yI, float zI, float wI) {
		x = xI; y = yI; z = zI; w = wI;
		return *this;
	}
	// Euler
	quaternion &SetEuler(float yaw, float pitch, float roll) {
		float cosY = cosf(yaw / 2.0F);
		float sinY = sinf(yaw / 2.0F);
		float cosP = cosf(pitch / 2.0F);
		float sinP = sinf(pitch / 2.0F);
		float cosR = cosf(roll / 2.0F);
		float sinR = sinf(roll / 2.0F);
		SetValues(
			cosR * sinP * cosY + sinR * cosP * sinY,
			cosR * cosP * sinY - sinR * sinP * cosY,
			sinR * cosP * cosY - cosR * sinP * sinY,
			cosR * cosP * cosY + sinR * sinP * sinY
		);
		return *this;
	}
	// Normalize
	quaternion &Normalize() {
		return *this *= 1.0F / GetLength();
	}
	float GetLength () {
		return sqrtf(w * w + x * x + y * y + z * z);
	}
	matrix33 GetMatrix33() {
		return matrix33(
			vector3(1 - 2*(y*y) - 2*(z*z), 2*(x*y+w*z), 2*(x*z-w*y)),
			vector3(2*(x*y-w*z), 1 - 2*(x*x) - 2*(z*z), 2*(y*z+w*x)),
			vector3(2*(x*z+w*y), 2*(y*z-w*x), 1 - 2*(x*x) - 2*(y*y))
		);
	}
};

// Dot Product
inline float DotProduct(const quaternion &a, const quaternion &b) {
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// Smooth Linear Interpolate
inline quaternion Slerp(const quaternion &from, const quaternion &to, 
float interp) {
	quaternion temp;
	float omega, cosO, sinO;
	float scale0, scale1;

	cosO = DotProduct(from, to);

	if (cosO < 0.0) {
		cosO = -cosO;
		temp = -to;
	} else  {
		temp = -to;
	}

	if ((1.0 - cosO) > ERROR_TOLERANCE) {
		omega = (float)acos(cosO);
		sinO = sinf(omega);
		scale0 = sinf((1.0F - interp) * omega) / sinO;
		scale1 = sinf(interp * omega) / sinO;
	} else {
		scale0 = 1.0F - interp;
		scale1 = interp;
	}

	return scale0 * from + scale1 * temp;
}

#endif

