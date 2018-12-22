/* Copyright (C) Miguel Gomez, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Miguel Gomez, 2000"
 */
#ifndef _COORDINATE_FRAME_
#define _COORDINATE_FRAME_


#include "quaternion.h"


//////////////////////////////////////////////////
//a specific kind of matrix for rotating about an axis
class ROTATION_MATRIX : public MATRIX3
{
public:
	ROTATION_MATRIX( const SCALAR theta, const VECTOR3& v );
};




// Constants
const VECTOR3 kX(1,0,0), kY(0,1,0), kZ(0,0,1);



//
// Set of orthonormal basis vectors with respect to a parent
//
class BASIS
{
public:

	MATRIX3	R;

public:

	BASIS()
	{}

	BASIS( 
		const VECTOR3&	v0,
		const VECTOR3&	v1,
		const VECTOR3&	v2
		)
		:	R( v0, v1, v2 )
	{}

	BASIS( const MATRIX3& m )
		:	R( m )
	{}

	BASIS( const QUATERNION& q )
		:	R(	VECTOR3(	1 - 2*q.j*q.j - 2*q.k*q.k,	2*q.i*q.j - 2*q.r*q.k,		2*q.i*q.k + 2*q.r*q.j ),
				VECTOR3(	2*q.i*q.j + 2*q.r*q.k,		1 - 2*q.i*q.i - 2*q.k*q.k,	2*q.j*q.k - 2*q.r*q.i ),
				VECTOR3(	2*q.i*q.k - 2*q.r*q.j,		2*q.j*q.k + 2*q.r*q.i,		1 - 2*q.i*q.i - 2*q.j*q.j )	)
	{
	}

	const VECTOR3& operator [] ( long i ) const		{ return R.C[i]; }

	const VECTOR3& X() const		{ return R.C[0]; }
	const VECTOR3& Y() const		{ return R.C[1]; }
	const VECTOR3& Z() const		{ return R.C[2]; }

	const QUATERNION quaternion();
	const MATRIX3& matrix() const			{ return R; }

	void basis( const VECTOR3& v0, const VECTOR3& v1, const VECTOR3& v2 )
	{
		this->R[0] = v0;
		this->R[1] = v1;
		this->R[2] = v2;
	}

	// Right-Handed Rotations
	void rotateAboutX	( const SCALAR a );
	void rotateAboutZ	( const SCALAR a );
	void rotateAboutY	( const SCALAR a );

	//rotate the basis about the unit axis u by theta (radians)
	void rotate( const SCALAR theta, const VECTOR3& u );

	//rotate, length of da is theta, unit direction of da is u
	void rotate( const VECTOR3& da );


	// Transformations
	const VECTOR3 transformVectorToLocal( const VECTOR3& v ) const
	{
		return VECTOR3( R.C[0].dot(v), R.C[1].dot(v), R.C[2].dot(v) );
	}

	const VECTOR3 transformVectorToParent( const VECTOR3& v ) const
	{
		return R.C[0] * v.x + R.C[1] * v.y + R.C[2] * v.z;
	}
};




//
// A coordinate frame (basis and origin) with respect to a parent
//
class COORD_FRAME : public BASIS
{
public:

	VECTOR3 O;		//this coordinate frame's origin, relative to its parent frame

public:

	COORD_FRAME()
	{}

	COORD_FRAME(
				const VECTOR3&	o,
				const VECTOR3&	v0,
				const VECTOR3&	v1,
				const VECTOR3&	v2
				)
	:	O		( o ),
		BASIS	( v0, v1, v2 )
	{}

	COORD_FRAME(
				const MATRIX3& basis,
				const VECTOR3& origin
				)
		:	O		( origin ),
			BASIS	( basis )
	{}

	const VECTOR3& position() const					{ return O; }
	void position( float x, float y, float z )		{ O = VECTOR3(x,y,z); }
	void position( const VECTOR3& p )				{ O = p; }

	const VECTOR3 transformPointToLocal(  const VECTOR3& p ) const
	{
		//translate to this frame's origin, then project onto this basis
		return transformVectorToLocal( p - O );
	}

	const VECTOR3 transformPointToParent( const VECTOR3& p ) const
	{
		//transform the coordinate vector and translate by this origin
		return transformVectorToParent( p ) + O;
	}

	//translate the origin by the given vector
	void translate( const VECTOR3& v )
	{
		O += v;
	}
};



#endif
//EOF
