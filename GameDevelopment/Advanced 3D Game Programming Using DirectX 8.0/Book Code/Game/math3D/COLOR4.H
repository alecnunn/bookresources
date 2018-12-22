/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: color4.h
 *    Desc: RGBA color structure, using floats
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _COLOR4_H
#define _COLOR4_H

#include "mathGlobal.h"

#include "color3.h"

struct color4
{
	union {
		struct 
		{
			float r, g, b, a; // Red, Green, and Blue color data
        };
		float c[4];
	};

	color4(){}

	color4( float inR, float inG, float inB, float inA ) :
		r( inR ), g( inG ), b( inB ), a( inA )
	{
	}

	color4( const color3& in, float alpha = 1.f )
	{
		r = in.r;
		g = in.g;
		b = in.b;
		a = alpha;
	}

	color4( unsigned long color )
	{
		b = (float)(color&255) / 255.f;
		color >>= 8;
		g = (float)(color&255) / 255.f;
		color >>= 8;
		r = (float)(color&255) / 255.f;
		color >>= 8;
		a = (float)(color&255) / 255.f;
	}


	void Assign( float inR, float inG, float inB, float inA )
	{
		r = inR;
		g = inG;
		b = inB;
		a = inA;
	}

	unsigned long MakeDWord()
	{
		unsigned long iA = (int)(a * 255.f ) << 24;
		unsigned long iR = (int)(r * 255.f ) << 16;
		unsigned long iG = (int)(g * 255.f ) << 8;
		unsigned long iB = (int)(b * 255.f );
		return iA | iR | iG | iB;
	}

	unsigned long MakeDWordSafe()
	{
		color4 temp = *this;
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
		if( a > 1 ) 
			a = 1.f;
		if( r < 0 ) 
			r = 0.f;
		if( g < 0 ) 
			g = 0.f;
		if( b < 0 ) 
			b = 0.f;
		if( a < 0 ) 
			a = 0.f;
	}

	color4& operator += ( const color4& in );
	color4& operator -= ( const color4& in );
	color4& operator *= ( const color4& in );
	color4& operator /= ( const color4& in );
	color4& operator *= ( const float& in );
	color4& operator /= ( const float& in );

	// some basic colors.
	static const color4 Black;
	static const color4 Gray;
	static const color4 White;
	static const color4 Red;
	static const color4 Green;
	static const color4 Blue;
	static const color4 Magenta;
	static const color4 Cyan;
	static const color4 Yellow;

};


//==========--------------------------  color4 Operators


/**
 * Accumulative addition of two colors
 */
inline color4& color4::operator += ( const color4& in )
{
	r += in.r;   
	g += in.g;   
	b += in.b;
	a += in.a;
	return *this;
}


/**
 * Accumulative subtraction of two colors
 */
inline color4& color4::operator -= ( const color4& in )
{
	r -= in.r;   
	g -= in.g;   
	b -= in.b;
	a -= in.a;
	return *this;
}


/**
 * Accumulative multiplication of a color by a color
 */
inline color4& color4::operator *= ( const color4& in )
{
	r *= in.r;   
	g *= in.g;   
	b *= in.b;
	a *= in.a;
	return *this;
}


/**
 * Accumulative division of a color by a color
 */
inline color4& color4::operator /= ( const color4& in )
{
	r /= in.r;   
	g /= in.g;   
	b /= in.b;
	a /= in.a;
	return *this;
}


/**
 * Accumulative multiplication of a color by a scalar
 */
inline color4& color4::operator *= ( const float& in )
{
	r *= in;   
	g *= in;   
	b *= in;
	a *= in;
	return *this;
}


/**
 * Accumulative division of a color by a scalar
 */
inline color4& color4::operator /= ( const float& in )
{
	float inv = 1.f / in;
	r *= inv;   
	g *= inv;   
	b *= inv;
	a *= inv;
	return *this;
}


/**
 * Adds two colors together: ret = a + b
 */
inline color4 operator+(color4 const &a, color4 const &b)
{
	return color4
	(
		a.r+b.r,
		a.g+b.g,
		a.b+b.b,
		a.a+b.a
	);
}; 
 

/**
 * Subtracts two colors : ret = a - b
 */
inline color4 operator-(color4 const &a, color4 const &b)
{
	return color4
	(
		a.r-b.r,
		a.g-b.g,
		a.b-b.b,
		a.a-b.a
	);
}; 


/**
 * Scales a color by a float : ret = a * b
 */
inline color4 operator*(color4 const &a, float const &b)
{
	return color4
	(
		a.r*b,
		a.g*b,
		a.b*b,
		a.a*b
	);
}; 


/**
 * Scales a color by a float : ret = a * b
 */
inline color4 operator*(float  const &a, color4 const &b)
{
	return color4
	(
		a*b.r,
		a*b.g,
		a*b.b,
		a*b.a
	);
}; 


/**
 * Divides a color by a float : ret = a / b
 */
inline color4 operator/(color4 const &a, float const &b)
{
	float inv = 1.f / b;
	return color4
	(
		a.r*inv,
		a.g*inv,
		a.b*inv,
		a.a*inv
	);
}; 

/**
 * Divides a color by a color (piecewise): ret = a / b
 */
inline color4 operator/(color4 const &a, color4 const &b)
{
	return color4
	(
		a.r / b.r,
		a.g / b.g,
		a.b / b.b,
		a.a / b.a
	);
}; 

/**
 * Multiplies a color by a color (piecewise): ret = a / b
 */
inline color4 operator*(color4 const &a, color4 const &b)
{
	return color4
	(
		a.r * b.r,
		a.g * b.g,
		a.b * b.b,
		a.a * b.a
	);
}; 


/**
 * color Equality, epsilon used due to numerical imprecision
 */
inline bool operator==(color4 const &a, color4 const &b)
{
	if(FastFabs(a.r-b.r)<EPSILON)
		if(FastFabs(a.g-b.g)<EPSILON)
			if(FastFabs(a.b-b.b)<EPSILON)
				if(FastFabs(a.a-b.a)<EPSILON)
					return true;
	return false;
};


#endif //_COLOR4_H