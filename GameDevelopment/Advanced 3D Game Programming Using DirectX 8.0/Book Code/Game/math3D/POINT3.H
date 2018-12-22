/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: point3.h
 *    Desc: declaration of a generic 3D vector class, with 
 *          operators aplenty
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _POINT3_H
#define _POINT3_H

#include <math.h>
#include "mathGlobal.h"

struct point4;

struct point3 
{

	union 
	{
		// This structure here is nameless, which really isn't 
		// legal C++... most compilers will deal with it,
		// however.
		struct 
		{
            float x,y,z; // 3 real components of the vector
        };
        float	v[3]; // Array access useful in loops
    };

	// Default constructor, defined for template classes
	point3()
	{
	}

	// Construct a point with 3 given inputs
	point3( float X, float Y, float Z ) : 
		x(X), y(Y), z(Z)
	{
	}

	point3( const point4& in );

	// Construct a point using spherical coordinates
	static const point3 Spherical( float theta, float rho, float phi )
	{
		return point3(
			(float)(phi*cos(rho)*sin(theta)), 
			(float)(phi*sin(rho)), 
			(float)(phi*cos(rho)*cos(theta))
		);
	}

	// Reassign a point without making a temporary structure
	void Assign( float X, float Y, float Z );

	// Return the magnitude of a point
	float Mag() const;

	// The length of the vector squared (to avoid the sqrt() )
	float MagSquared() const;

	// Normalizes a vector (makes it's length one)
	void Normalize();

	// returns a Normalized version of this
	const point3 Normalized() const;

	// Computes the distance between two vectors
	static float Dist( const point3 &a, const point3 &b )
	{
		// can't use the subtraction operator (it's not defined yet!)
		point3 distVec( b.x - a.x, b.y - a.y, b.z - a.z );
		return distVec.Mag();
	}

	point3& operator += ( const point3& in );
	point3& operator -= ( const point3& in );
	point3& operator *= ( const float& in );
	point3& operator /= ( const float& in );
	point3  operator -  (); // this is plain old negation

	/**
	 * Constant vectors
	 */
	static const point3 Zero; // <0,0,0>
	static const point3 i; // <1,0,0>
	static const point3 j; // <0,1,0>
	static const point3 k; // <0,0,1>

}; 

//==========--------------------------  point3 Operators

/**
 * Accumulative addition of two vectors
 */
inline point3& point3::operator += ( const point3& in )
{
	x += in.x;   
	y += in.y;   
	z += in.z;
	return *this; // This is so we can chain operations together
}


/**
 * Accumulative subtraction of two vectors
 */
inline point3& point3::operator -= ( const point3& in )
{
	x -= in.x;   
	y -= in.y;   
	z -= in.z;
	return *this; // This is so we can chain operations together
}


/**
 * Accumulative multiplication of a vector by a scalar
 */
inline point3& point3::operator *= ( const float& in )
{
	x *= in;   
	y *= in;   
	z *= in;
	return *this; // This is so we can chain operations together
}


/**
 * Accumulative division of a vector by a scalar
 */
inline point3& point3::operator /= ( const float& in )
{
	float inv = 1.f / in; // do one (expensive) divide and 3 (cheap) mults
	x *= inv;   
	y *= inv;   
	z *= inv;
	return *this; // This is so we can chain operations together 
}

inline point3 point3::operator - () 
{
	return point3(
		-x,
		-y,
		-z
	);
}



/**
 * Adds two points together: ret = a + b
 */
inline const point3 operator+(point3 const &a, point3 const &b)
{
	return point3
	(
		a.x+b.x,
		a.y+b.y,
		a.z+b.z
	);
}; 
 

/**
 * Subtracts to points : ret = a - b
 */
inline const point3 operator-(point3 const &a, point3 const &b)
{
	return point3
	(
		a.x-b.x,
		a.y-b.y,
		a.z-b.z
	);
}; 


/**
 * Scales a vector by a float : ret = a * b
 */
inline const point3 operator*(point3 const &a, float const &b)
{
	return point3
	(
		a.x*b,
		a.y*b,
		a.z*b
	);
}; 


/**
 * Scales a vector by a float : ret = a * b
 */
inline const point3 operator*(float  const &a, point3 const &b)
{
	return point3
	(
		a*b.x,
		a*b.y,
		a*b.z
	);
}; 


/**
 * Divides a vector by a float : ret = a / b
 */
inline const point3 operator/(point3 const &a, float const &b)
{
	float inv = 1.f / b;
	return point3
	(
		a.x*inv,
		a.y*inv,
		a.z*inv
	);
}; 


/**
 * Performs a vector cross product : ret = a cross b
 */
inline const point3 operator^(point3 const &a, point3 const &b)
{
	return point3
	(
		(a.y*b.z-a.z*b.y),
		(a.z*b.x-a.x*b.z),
		(a.x*b.y-a.y*b.x)
	);
} 


/**
 * Performs a vector dot product : ret = a dot b
 */
inline const float operator*(point3 const &a, point3 const &b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
} 


/**
 * Vector Equality, epsilon used due to numerical imprecision
 */
inline bool   operator==(point3 const &a, point3 const &b)
{
	if(fabs(a.x-b.x)<EPSILON)
	{
		if(fabs(a.y-b.y)<EPSILON)
		{
			if(fabs(a.z-b.z)<EPSILON)
			{
				return true;
			}
		}
	}
	return false;
};

//==========--------------------------  Inline functions
inline void point3::Assign( float X, float Y, float Z )
{
	x=X;	
	y=Y;	
	z=Z;
}

inline float point3::Mag() const
{
	return (float)sqrt( (*this) * (*this) );
}

inline float point3::MagSquared() const
{ 
	return ( (*this) * (*this) );
}

inline void point3::Normalize()
{
	float invMag = 1.f/Mag();	

	(*this) *= invMag;
}

inline const point3 point3::Normalized() const
{
	float invMag = 1.f/Mag();	
	return (*this) * invMag;
}

#endif /*_POINT3_H*/