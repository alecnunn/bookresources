/* Copyright (C) Miguel Gomez, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Miguel Gomez, 2000"
 */
#ifndef _QUATERNION_H_
#define _QUATERNION_H_


#include "geometry.h"


//a quaternion has all the functions of a 4D vector, plus some more
struct QUATERNION
{
	SCALAR r;		//real component
	SCALAR i, j, k;	//complex components


	QUATERNION()
		:	r(0), i(0), j(0), k(0)
	{}

	QUATERNION( const SCALAR r, const SCALAR i, const SCALAR j, const SCALAR k )
		:	r(r), i(i), j(j), k(k)
	{}

	//assign
	const QUATERNION& operator = ( const QUATERNION& q )
	{
		r = q.r;
		i = q.i;
		j = q.j;
		k = q.k;

		return *this;
	}

	//compare
	int operator == ( const QUATERNION& q ) const
	{
		return (q.r==r && q.i==i && q.j==j && q.k==k);
	}

	int operator != ( const QUATERNION& q ) const
	{
		return (q.r!=r || q.i!=i || q.j!=j || q.k!=k);
	}

	//negate
	QUATERNION operator - () const
	{
		return QUATERNION( -r, -i, -j, -k );
	}

	void operator += ( const QUATERNION& q ) 
	{
		r += q.r;
		i += q.i;
		j += q.j;
		k += q.k;
	} 

	void operator -= ( const QUATERNION& q ) 
	{
		r -= q.r;
		i -= q.i;
		j -= q.j;
		k -= q.k;
	} 

	void operator *= ( const SCALAR s )
	{
		r *= s;
		i *= s;
		j *= s;
		k *= s;
	} 

	void operator /= ( SCALAR s )
	{
		s = 1/s;
		r *= s;
		i *= s;
		j *= s;
		k *= s;
	} 

	//add
	const QUATERNION operator + ( const QUATERNION& q ) const
	{
		return QUATERNION( r + q.r, i + q.i, j + q.j, k + q.k );
	}

	//subtract
	const QUATERNION operator - ( const QUATERNION& q ) const
	{
		return QUATERNION( r - q.r, i - q.i, j - q.j, k - q.k );
	}

	//multiply
	const QUATERNION operator * ( const SCALAR s ) const
	{
		return QUATERNION( r * s, i * s, j * s, k * s );
	}

	//pre - multiply
	friend inline const QUATERNION operator * ( const SCALAR s, const QUATERNION& v )
	{
		return v * s;
	}

	//divide
	const QUATERNION operator / ( SCALAR s ) const
	{
		s = 1/s;
		return QUATERNION( r*s, i*s, j*s, k*s );
	}

	//dot product
	const SCALAR dot( const QUATERNION& q ) const
	{
		return( r*q.r + i*q.i + j*q.j + k*q.k );
	}

	//magnitude
	const SCALAR length() const
	{
		return( (SCALAR)sqrt( (double)(r*r + i*i + j*j + k*k) ) );
	}

	//unit vector
	const QUATERNION unit() const
	{
		return (*this) / length();
	}

	//make this a unit vector
	void normalize()
	{
		(*this) /= length();
	}

	//equal within an error 'r'
	bool nearlyEquals( const QUATERNION& q, const SCALAR e ) const
	{
		return fabs(r-q.r)<e && fabs(i-q.i)<e && fabs(j-q.j)<e && fabs(k-q.k)<e;
	}

	/*
	//quaternion multiply
	const QUATERNION& operator *= ( const QUATERNION& q )
	{
		return *this;
	}

	QUATERNION operator * ( const QUATERNION& q ) const
	{
		return QUATERNION();
	}

	//for some reason, need to redefine these to work
	QUATERNION operator * ( const SCALAR s ) const
	{
		return this->operator * (s);
	}
	//*/
};

//spherically linearly interpolate between two unit quaternions, a and b
inline const QUATERNION Slerp( const QUATERNION& a, const QUATERNION& b, const float u )
{
	const float theta = acosf( a.dot( b ) );//angle between two unit quaternions
	const float t = 1 / sinf( theta );

	return  t * (a * sinf( (1-u)*theta ) + b * sinf( u*theta ) );
}


#endif
//EOF
