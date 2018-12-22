/* Copyright (C) Miguel Gomez, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Miguel Gomez, 2000"
 */
#ifndef _MATHPHYS_
#define _MATHPHYS_


#include <cmath>
#include <cfloat>
#include <stdlib.h>




// --- CONSTANTS --- //

const double PI			= 4.0 * atan(1.0);
const double TWO_PI		= (2 * PI);
const double PI_2		= (PI / 2);
const double PI_3		= (PI / 3);
const double PI_4		= (PI / 4);
const double PI_6		= (PI / 6);

// --- CONVERSIONS --- //

const double kDegreesPerRadian = 180 / PI;				//180 deg per pi radians
const double kRadiansPerDegree = 1 / kDegreesPerRadian;



// --- FORMULAS --- //


template< class T >
	inline T	Min( const T& a, const T& b )		{ return a<b ? a:b; }	//minimum of a and b

template< class T >
	inline T	Max( const T& a, const T& b )		{ return a>b ? a:b; }	//maximum of a and b

template< class T >
	inline void Swap( T& a, T& b )//swap the values of a and b
	{
		const T temp = a;

			a = b;
			b = temp;
	}

///////////////////////////////////////////////////////////////////////////
// Return true if r1 and r2 are real
inline bool quadratic_formula
(
	const double a,
	const double b,
	const double c,
	double& r1,		//first
	double& r2		//and second roots
)
{
	const double q = b*b - 4*a*c;

		if( q >= 0 )
		{
			const double sq = sqrt(q);
			const double d = 1 / (2*a);

			r1 = ( -b + sq ) * d;
			r2 = ( -b - sq ) * d;

			return true;//real roots
		}
		else
		{
			return false;//complex roots
		}
}



//linearly interpolate between two affine quantities
template < class SCALAR, class AFFINE >
inline AFFINE lerp
(
	const SCALAR& da,
	const AFFINE& A,
	const SCALAR& db,
	const AFFINE& B
)
{
	return (A*db - B*da) / (db - da);//works for integer and floating point
}


// spherically linearly interpolate between two vectors
template < class SCALAR, class VECTOR >
inline VECTOR slerp
(
	const VECTOR& a,
	const VECTOR& b,
	const SCALAR& theta,
	const SCALAR& u
)
{
	const double t = 1 / sin(theta);
	return t * sin((1-u)*theta) * a + t * sin(u*theta) * b;
}


//round to nearest integer
template < class SCALAR >
inline long round( const SCALAR r )
{
	return (long)(r + 0.5);
}


//random float between [0,1]
inline const float random_float()
{
	static const float rm = 1.f / RAND_MAX;//only divide once

	return rm * rand();
}

//cast this to a long to properly generate a random integer
inline const float random_float( const float min, const float max )
{
	const float u = random_float();//[0,1]

	//"Graphics Gems I", pp. 438-439
	return (min-0.499999f) + u*(max - min + 0.999999);
}


#endif

//EOF
