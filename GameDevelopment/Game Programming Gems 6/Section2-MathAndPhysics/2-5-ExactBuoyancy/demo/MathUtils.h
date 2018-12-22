/*
 * Copyright (c) 2005 Erin Catto http://www.gphysics.com
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies.
 * Erin Catto makes no representations about the suitability 
 * of this software for any purpose.  
 * It is provided "as is" without express or implied warranty.
 */

#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <math.h>

struct Vec3
{
	Vec3() {}
	Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

	void SetZero() { x = y = z = 0.0f; }

	Vec3& operator *= (float s)
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	Vec3& operator += (const Vec3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	float Length() const
	{
		return sqrtf(x*x + y*y + z*z);
	}

	void Normalize()
	{
		float norm = Length();
		if (norm)
		{
			float inv = 1.0f/norm;
			x *= inv; y *= inv; z *= inv;
		}
	}

	float x, y, z;
};


inline Vec3 operator + (const Vec3& a, const Vec3& b)
{
	return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline Vec3 operator - (const Vec3& a, const Vec3& b)
{
	return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline Vec3 operator * (float s, const Vec3& a)
{
	return Vec3(s*a.x, s*a.y, s*a.z);
}

// Dot product
inline float operator * (const Vec3& a, const Vec3& b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

// Cross product
inline Vec3 operator % (const Vec3& a, const Vec3& b)
{
	return Vec3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}


struct Quaternion
{
	Quaternion() {}
	Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

	Quaternion& operator += (const Quaternion& q)
	{
		x += q.x; y += q.y; z += q.z; w += q.w;
		return *this;
	}

	void Normalize()
	{
		float len = sqrtf(x*x + y*y + z*z + w*w);
		if (len)
		{
			x /= len; y /= len; z /= len; w /= len;
		}
		else
		{
			x = 0; y = 0; z = 0; w = 1;
		}
	}

	Quaternion Conjugate() const
	{
		return Quaternion(-x, -y, -z, w);
	}

	Vec3 Rotate(const Vec3& v) const
	{
		Vec3 s(x,y,z);
		return v + 2.0f*(s % (s % v + w*v));
	}

	float x, y, z, w;
};

// Quaternion multiplication
inline Quaternion operator * (const Quaternion& a, const Quaternion& b)
{
	Quaternion result;

	float aW = a.w, aX = a.x, aY = a.y, aZ = a.z;
	float bW = b.w, bX = b.x, bY = b.y, bZ = b.z;

	result.x = aW*bX + bW*aX + aY*bZ - aZ*bY;
	result.y = aW*bY + bW*aY + aZ*bX - aX*bZ;
	result.z = aW*bZ + bW*aZ + aX*bY - aY*bX;
	result.w = aW*bW - (aX*bX + aY*bY + aZ*bZ);

	return result;
}

inline Quaternion operator * (float s, const Quaternion& q)
{
	return Quaternion(s*q.x, s*q.y, s*q.z, s*q.w);
}

struct Mat33
{
	Mat33() {}
	Mat33(const Quaternion& q)
	{
		float x = q.x, y = q.y, z = q.z, w = q.w;
		float x2 = x+x, y2 = y+y, z2 = z+z;
		float xx = x*x2, xy = x*y2, xz = x*z2;
		float yy = y*y2, yz = y*z2, zz = z*z2;
		float wx = w*x2, wy = w*y2, wz = w*z2;
		
		col1.x = 1 - (yy + zz);
		col2.x =      xy - wz;
		col3.x =      xz + wy;

		col1.y =      xy + wz;
		col2.y = 1 - (xx + zz);
		col3.y =      yz - wx;
		
		col1.z =       xz - wy;
		col2.z =       yz + wx;
		col3.z = 1 - (xx + yy);
	}

	float& operator ()(int row, int col) { return *( (float*)this + col*3 + row); }
	float operator ()(int row, int col) const { return *( (float*)this + col*3 + row); }

	Vec3 col1, col2, col3;
};

struct RigidBody
{
	Vec3 x;		// world position of center of mass
	Quaternion q;	// rotation
	Vec3 v;		// velocity of center of mass
	Vec3 omega;	// angular velocity
	Vec3 F;		// force at center of mass
	Vec3 T;		// torque
	Vec3 I;		// rotational inertia
	float mass;

};

#endif
