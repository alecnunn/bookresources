/* Copyright (C) Miguel Gomez, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Miguel Gomez, 2000"
 */
#ifndef _BODY_H_
#define _BODY_H_


#include "coordinate_frame.h"


//
// Describe the dynamic state of a rigid body
//
class RIGID_BODY_STATE : public COORD_FRAME
{
public:

	SYMMETRIC_MATRIX3 I;		//inertia tensor with respect to parent frame, kg*m^2
	SYMMETRIC_MATRIX3 I_inv;	//inverse inertia tensor
	VECTOR3		V;		//linear velocity, meters/sec
	VECTOR3		W;		//angular velocity, radians/sec

public:

	RIGID_BODY_STATE()
	{}

	RIGID_BODY_STATE( const VECTOR3& v, const VECTOR3& w )
		:	V (v),
			W (w)
	{}

	//calculate the inertia tensor and its
	//inverse from the current orientation
	//and the principal moments of inertia
	void calculateInertiaTensor( const VECTOR3& ip );
};



//
// Describe a rigid body
//
class RIGID_BODY : public RIGID_BODY_STATE
{
public:

	//previous dynamic state
	RIGID_BODY_STATE	PrevState;

	//mass and inertia
	SCALAR	M;		//mass, kg
	VECTOR3	Ip;		//principal moments of inertia, kg * m^2

	//bounding box dimensions
	VECTOR3 Dim;

public:

	RIGID_BODY()
		:	M(0)
	{}

	RIGID_BODY(	const SCALAR m, const VECTOR3& d, const VECTOR3 ip = VECTOR3(0,0,0) )
	:	M	(m),
		Dim	(d),
		Ip	(ip)
	{
		//if no moments were passed in,
		//give the moments for a box
		if( ip.x==0 && ip.y==0 && ip.z==0 )
		{
			this->Ip = (m/12) * VECTOR3(d.y*d.y + d.z*d.z,
										d.x*d.x + d.z*d.z,
										d.x*d.x + d.y*d.y);
		}

		//before any physics are done
		this->calculateInertiaTensor( this->Ip );
	}

	//physics
	void mass( const SCALAR m )							{ M = m; }
	const SCALAR mass() const							{ return M; }

	void principalInertia( const VECTOR3& ip )			{ Ip = ip; }
	const VECTOR3& principalInertia() const				{ return Ip; }

	const RIGID_BODY_STATE currentState() const			{ return *this; }
	const RIGID_BODY_STATE previousState() const		{ return PrevState; }

	//time derivative of angular velocity
	const VECTOR3 dwdt( const VECTOR3& T ) const
	{
		return this->I_inv * ( T - this->W.cross(this->I*this->W) );
	}

	void rotate( const VECTOR3& v )
	{
		this->BASIS::rotate( v );
		this->calculateInertiaTensor( this->Ip );
	}
};


#endif

//EOF
