/* Copyright (C) Miguel Gomez, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Miguel Gomez, 2000"
 */
#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_


#include "scalar.h"
#include "vector.h"
#include "matrix.h"
#include <float.h>

//points and vectors
typedef T_VECTOR3<SCALAR>		VECTOR3;
typedef T_VECTOR4<SCALAR>		VECTOR4;

//matrices
typedef T_MATRIX3<VECTOR3, SCALAR>							MATRIX3;
typedef T_MATRIX4<VECTOR4, SCALAR>							MATRIX4;
typedef T_SYMMETRIC_MATRIX3< SCALAR, VECTOR3, MATRIX3 >		SYMMETRIC_MATRIX3;


//
// a plane in 3D space
//
class PLANE
{
public:

	VECTOR3 N;
	SCALAR D;			//D P0.dot( N );

public:

	PLANE()
		:	N	(1,0,0),//x,y plane
			D	(0)
	{}

	//from a normal and a point
	PLANE( const VECTOR3& p0, const VECTOR3& n )
		:	N	( n.unit() ),
			D	( N.dot(p0) )
	{}

	//from three points
	PLANE( const VECTOR3& p0, const VECTOR3& p1, const VECTOR3& p2 )
		:	N	( (p1-p0).cross(p2-p0).unit() ),//unit normal from points
			D	( N.dot(p0) )
	{}

	//perpendicular distance from the plane to a point
	SCALAR distanceToPoint( const VECTOR3& p ) const
	{
		return N.dot(p) - D;//signed projection onto unit normal
	}

};




//////////////////////////////////////////////////
// Parameteric line in world space
class LINE
{
	protected:

		VECTOR3		P0;		//a point in the line
		VECTOR3	V;		//the unit direction of the line

	public:

		LINE( const VECTOR3& p0, const VECTOR3& v )
			:	P0	( p0 ),		
				V	( v.unit() )
		{}

		//direction as a unit vector
		const VECTOR3& direction()			{ return V; }
};




#endif
//EOF
