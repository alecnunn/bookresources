/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: point4.h
 *    Desc: 4-dimensional point class
 *          Subset of the functionality in point3
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _POINT4_H
#define _POINT4_H

#include <math.h>

#include "mathGlobal.h"
#include "point3.h"

struct point4 {
	union {
        struct {
            float x,y,z,w;
        };
        float	v[4];// for looping
    };

	point4(){}

	point4( float X, float Y, float Z, float W ) :
		x(X), y(Y), z(Z), w(W)
	{
	
	}

	point4( point3& in ) :
		x(in.x), y(in.y), z(in.z), w(1.f)
	{
	}

	void Assign( float X, float Y, float Z, float W )
	{
		x=X;	y=Y;	z=Z;	w=W;
	}

	void Negate()
	{
		x=-x;	y=-y;	z=-z;	w=-w;
	}

	float Mag(){
		return (float)sqrt( x*x + y*y + z*z + w*w );
	}

	void Normalize(){
		float foo=1/Mag();	x*=foo;	y*=foo;	z*=foo; w*=foo;
	}

}; 

//returns point4+point4
inline const point4 operator+(point4 const &a, point4 const &b)
{
	return point4(a.x+b.x,a.y+b.y,a.z+b.z,a.w+b.w);
}; 


//returns point4-point4
inline const point4 operator-(point4 const &a, point4 const &b)
{
	return point4(a.x-b.x,a.y-b.y,a.z-b.z,a.w-b.w);
}; 


//returns point4*float
inline const point4 operator*(point4 const &a, float const &b)
{
	return point4(a.x*b,a.y*b,a.z*b,a.w*b);
}; 


//returns float*point4
inline const point4 operator*(float  const &a, point4 const &b)
{
	return point4(a*b.x,a*b.y,a*b.z,a*b.w);
}; 


//returns point4/float
inline const point4 operator/(point4 const &a, float const &b)
{
	return point4(a.x/b,a.y/b,a.z/b,a.w/b);
}; 

//returns true if point4==point4
inline bool operator==(point4 const &a, point4 const &b)
{
	if(fabs(a.x-b.x)<EPSILON)
		if(fabs(a.y-b.y)<EPSILON)
			if(fabs(a.z-b.z)<EPSILON)
				if(fabs(a.w-b.w)<EPSILON)
					return true;
	return false;
};

// returns a dot b
inline float operator*(point4 const &a, point4 const &b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
} 


#endif /*_POINT4_H*/