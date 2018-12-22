#ifndef   INCLUDED_Vector3
#define   INCLUDED_Vector3
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MathTypes.h"

class Matrix4x4;

const float VEC3_COMPARE_EPSILON = 0.00001f;

class Vector3
{
public:
	Vector3() {};
	Vector3( float fX, float fY, float fZ ) : x(fX), y(fY), z(fZ) {};
	Vector3( ZeroType ) : x(0), y(0), z(0) {};
	Vector3( MinimumType ) : x(-FLT_MAX), y(-FLT_MAX), z(-FLT_MAX) {};
	Vector3( MaximumType ) : x(FLT_MAX), y(FLT_MAX), z(FLT_MAX) {};

	static Vector3 Zero;
	static Vector3 Min;
	static Vector3 Max;
	
	void Set(float fX, float fY, float fZ) { x = fX; y = fY; z = fZ; }
		
    bool operator ==( const Vector3 &v ) const { return x == v.x && y == v.y && z == v.z; }
    bool operator !=( const Vector3 &v ) const { return x != v.x || y != v.y || z != v.z; }

	Vector3& operator *= ( const Vector3 &v )	{ x *= v.x; y *= v.y; z *= v.z; return *this; }
	Vector3& operator *= ( const float f )		{ x *= f; y *= f; z *= f; return *this; }

	Vector3& operator += ( const Vector3 &v )	{ x += v.x; y += v.y; z += v.z; return *this; }
	Vector3& operator += ( const float f )		{ x += f; y += f; z += f; return *this; }

	Vector3& operator -= ( const Vector3 &v )	{ x -= v.x; y -= v.y; z -= v.z; return *this; }
	Vector3& operator -= ( const float f )		{ x -= f; y -= f; z -= f; return *this; }

	Vector3& operator /= ( const Vector3 &v )	{ x /= v.x; y /= v.y; z /= v.z; return *this; }
	Vector3& operator /= ( const float f )		{ x /= f; y /= f; z /= f; return *this; }

    float x, y, z;
};

// global operators
inline Vector3 operator*( float f, const Vector3& rhs ) 
{
	return Vector3(rhs.x * f, rhs.y * f, rhs.z * f);
}

inline Vector3 operator*( const Vector3& lhs, float f ) 
{
	return Vector3(lhs.x * f, lhs.y * f, lhs.z * f);
}

inline Vector3 operator+( const Vector3& lhs, const Vector3& rhs ) 
{
	return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

inline Vector3 operator-( const Vector3& lhs, const Vector3& rhs ) 
{
	return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}


#endif // INCLUDED_Vector3
