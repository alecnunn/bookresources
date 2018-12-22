/* Copyright (C) Miguel Gomez, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Miguel Gomez, 2000"
 */
#include "coordinate_frame.h"


					//////////////////////
					// ROTATION_MATRIX3 //
					//////////////////////


///////////////////////////////////////////////////////////////////////////
// form a quaternion rotation matrix for revolving about the axis 'u'
// by the angle theta
ROTATION_MATRIX::ROTATION_MATRIX( const SCALAR theta, const VECTOR3& u )
{
	//rotate around the line
	SCALAR w = cosf(0.5f * theta);
	//TODO:  shouldn't have to normalize u
	VECTOR3 v = u * sinf(0.5f * theta);
	SCALAR x=v.x, y=v.y, z=v.z;

	//	
	// quaternion formula
	// taken from "Computer Graphics",
	// Hearn and Baker, 2nd Edition, p. 420
	//

		//assign columns
		C[0] = VECTOR3(	1 - 2*y*y - 2*z*z,
						2*x*y + 2*w*z,
						2*x*z - 2*w*y
						);

		C[1] = VECTOR3(	2*x*y - 2*w*z,
						1 - 2*x*x - 2*z*z,
						2*y*z + 2*w*x
						);

		C[2] = VECTOR3(	2*x*z + 2*w*y,
						2*y*z - 2*w*x,
						1 - 2*x*x - 2*y*y
						);

}



					///////////
					// BASIS //
					///////////




const QUATERNION BASIS::quaternion()
{
	QUATERNION q;
	float tr, s;

	tr = R[0][0] + R[1][1] + R[2][2];

	if( tr >= 0 )
	{
		s = sqrtf( tr + 1 );
		q.r = 0.5f * s;
		s = 0.5f / s;
		q.i = (R[2][1] - R[1][2]) * s;
		q.j = (R[0][2] - R[2][0]) * s;
		q.k = (R[1][0] - R[0][1]) * s;
	}
	else
	{
		if( R[1][1] > R[0][0] )//case 1
		{
			s = sqrtf( (R[1][1] - (R[2][2] + R[0][0])) + 1 );
			q.j = 0.5f * s;
			s = 0.5f / s;
			q.k = (R[1][2] + R[2][1]) * s;
			q.i = (R[0][1] + R[1][0]) * s;
			q.r = (R[0][2] - R[2][0]) * s;
		}
		else if( R[2][2] > R[1][1] )//case 2
		{
			s = sqrtf( (R[2][2] - (R[0][0] + R[1][1])) + 1 );
			q.k = 0.5f * s;
			s = 0.5f / s;
			q.i = (R[2][0] + R[0][2]) * s;
			q.j = (R[1][2] + R[2][1]) * s;
			q.r = (R[1][0] - R[0][1]) * s;
		}
		else//case 0
		{
			s = sqrtf( (R[0][0] - (R[1][1] + R[2][2])) + 1 );
			q.i = 0.5f * s;
			s = 0.5f / s;
			q.j = (R[0][1] + R[1][0]) * s;
			q.k = (R[2][0] + R[0][2]) * s;
			q.r = (R[2][1] - R[1][2]) * s;
		}
	}

	return q;
}


///////////////////////////////////////////////////////////////////////////
void BASIS::rotateAboutZ( const SCALAR a )
{
	if( 0 != a )//don't rotate by 0
	{
		//don't over-write basis before calculation is done
		VECTOR3 b0 = this->X()*cosf(a) + this->Y()*sinf(a);	//rotate x
		VECTOR3 b1 = -this->X()*sinf(a) + this->Y()*cosf(a);	//rotate y

			//set basis
			this->R[0] = b0;
			this->R[1] = b1;
			//z is unchanged
	}
}


///////////////////////////////////////////////////////////////////////////
void BASIS::rotateAboutX( const SCALAR a )
{
	if( 0 != a )//don't rotate by 0
	{
		VECTOR3 b1 = this->Y()*cosf(a) + this->Z()*sinf(a);
		VECTOR3 b2 = -this->Y()*sinf(a) + this->Z()*cosf(a);

			//set basis
			this->R[1] = b1;
			this->R[2] = b2;
			//x is unchanged
	}
}


///////////////////////////////////////////////////////////////////////////
void BASIS::rotateAboutY( const SCALAR a )
{
	if( 0 != a )//don't rotate by 0
	{
		VECTOR3 b2 = this->Z()*cosf(a) + this->X()*sinf(a);	//rotate z
		VECTOR3 b0 = -this->Z()*sinf(a) + this->X()*cosf(a);	//rotate x

			//set basis
			this->R[2] = b2;
			this->R[0] = b0;
			//y is unchanged
	}
}


///////////////////////////////////////////////////////////////////////////
void BASIS::rotate( const SCALAR theta, const VECTOR3& u )
{
	if( 0 != theta )//don't rotate by 0
	{
		const ROTATION_MATRIX r( theta, u );

			//rotate each basis vector
			this->R[0] = r * this->R[0];
			this->R[1] = r * this->R[1];
			this->R[2] = r * this->R[2];
	}
}


///////////////////////////////////////////////////////////////////////////
void BASIS::rotate( const VECTOR3& v )
{
	SCALAR theta = v.dot(v);//angle to rotate by

		if( 0 != theta )//don't rotate by 0
		{
			theta = sqrtf( theta );
			this->rotate( theta, v * (1/theta) );//unit vector is axis
		}
}


//EOF
