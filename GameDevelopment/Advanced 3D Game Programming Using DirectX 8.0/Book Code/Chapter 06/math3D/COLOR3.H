/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: color3.h
 *    Desc: RGB color structure, using floats
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _COLOR3_H
#define _COLOR3_H

#include "mathGlobal.h"

struct color3
{
	union {
		struct 
		{
			float r, g, b; // Red, Green, and Blue color data
        };
		float c[3];
	};

	color3(){}

	color3( float inR, float inG, float inB ) :
		r( inR ), g( inG ), b( inB )
	{
	}

	void Assign( float inR, float inG, float inB )
	{
		r = inR;
		g = inG;
		b = inB;
	}

	unsigned long MakeDWord()
	{
		unsigned long iR = (int)(r * 255.f ) << 16;
		unsigned long iG = (int)(g * 255.f ) << 8;
		unsigned long iB = (int)(b * 255.f );
		return 0xff000000 | iR | iG | iB;
	}

	unsigned long MakeDWordSafe()
	{
		color3 temp = *this;
		temp.Sat();
		return temp.MakeDWord();
	}

	// if any of the values are >1, cap them.
	void Sat()
	{
		if( r > 1 ) 
			r = 1.f;
		if( g > 1 ) 
			g = 1.f;
		if( b > 1 ) 
			b = 1.f;
		if( r < 0 ) 
			r = 0.f;
		if( g < 0 ) 
			g = 0.f;
		if( b < 0 ) 
			b = 0.f;
	}

	color3& operator += ( const color3& in );
	color3& operator -= ( const color3& in );
	color3& operator *= ( const float& in );
	color3& operator /= ( const float& in );

	// some basic colors.
	static const color3 Black;
	static const color3 Gray;
	static const color3 White;
	static const color3 Red;
	static const color3 Green;
	static const color3 Blue;
	static const color3 Magenta;
	static const color3 Cyan;
	static const color3 Yellow;
};

//==========--------------------------  color3 Operators


/**
 * Accumulative addition of two colors
 */
inline color3& color3::operator += ( const color3& in )
{
	r += in.r;   
	g += in.g;   
	b += in.b;
	return *this;
}


/**
 * Accumulative subtraction of two colors
 */
inline color3& color3::operator -= ( const color3& in )
{
	r -= in.r;   
	g -= in.g;   
	b -= in.b;
	return *this;
}


/**
 * Accumulative multiplication of a color by a scalar
 */
inline color3& color3::operator *= ( const float& in )
{
	r *= in;   
	g *= in;   
	b *= in;
	return *this;
}


/**
 * Accumulative division of a color by a scalar
 */
inline color3& color3::operator /= ( const float& in )
{
	float inv = 1.f / in;
	r *= inv;   
	g *= inv;   
	b *= inv;
	return *this;
}


/**
 * Adds two colors together: ret = a + b
 */
inline color3 operator+(color3 const &a, color3 const &b)
{
	return color3
	(
		a.r+b.r,
		a.g+b.g,
		a.b+b.b
	);
}; 
 

/**
 * Subtracts two colors : ret = a - b
 */
inline color3 operator-(color3 const &a, color3 const &b)
{
	return color3
	(
		a.r-b.r,
		a.g-b.g,
		a.b-b.b
	);
}; 


/**
 * Scales a color by a float : ret = a * b
 */
inline color3 operator*(color3 const &a, float const &b)
{
	return color3
	(
		a.r*b,
		a.g*b,
		a.b*b
	);
}; 


/**
 * Scales a color by a float : ret = a * b
 */
inline color3 operator*(float  const &a, color3 const &b)
{
	return color3
	(
		a*b.r,
		a*b.g,
		a*b.b
	);
}; 


/**
 * Divides a color by a float : ret = a / b
 */
inline color3 operator/(color3 const &a, float const &b)
{
	float inv = 1.f / b;
	return color3
	(
		a.r*inv,
		a.g*inv,
		a.b*inv
	);
}; 


/**
 * color Equality, epsilon used due to numerical imprecision
 */
inline bool operator==(color3 const &a, color3 const &b)
{
	if(FastFabs(a.r-b.r)<EPSILON)
		if(FastFabs(a.g-b.g)<EPSILON)
			if(FastFabs(a.b-b.b)<EPSILON)
				return true;
	return false;
};


#endif //_COLOR3_H