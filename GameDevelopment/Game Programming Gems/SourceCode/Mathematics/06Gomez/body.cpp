/* Copyright (C) Miguel Gomez, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Miguel Gomez, 2000"
 */
#include "body.h"



						//////////////////////
						// RIGID_BODY_STATE //
						//////////////////////


void RIGID_BODY_STATE::calculateInertiaTensor( const VECTOR3& p )
{
	const VECTOR3 ip( 1/p.x, 1/p.y, 1/p.z );

		//since it's a symmetric matrix, there are only 6 moments and products
		this->I.xx = p.x*R[0][0]*R[0][0] + p.y*R[1][0]*R[1][0] + p.z*R[2][0]*R[2][0];
		this->I.yy = p.x*R[0][1]*R[0][1] + p.y*R[1][1]*R[1][1] + p.z*R[2][1]*R[2][1];
		this->I.zz = p.x*R[0][2]*R[0][2] + p.y*R[1][2]*R[1][2] + p.z*R[2][2]*R[2][2];
		this->I.xy = p.x*R[0][0]*R[0][1] + p.y*R[1][0]*R[1][1] + p.z*R[2][0]*R[2][1];
		this->I.xz = p.x*R[0][0]*R[0][2] + p.y*R[1][0]*R[1][2] + p.z*R[2][0]*R[2][2];
		this->I.yz = p.x*R[0][1]*R[0][2] + p.y*R[1][1]*R[1][2] + p.z*R[2][1]*R[2][2];

		this->I_inv.xx = ip.x*R[0][0]*R[0][0] + ip.y*R[1][0]*R[1][0] + ip.z*R[2][0]*R[2][0];
		this->I_inv.yy = ip.x*R[0][1]*R[0][1] + ip.y*R[1][1]*R[1][1] + ip.z*R[2][1]*R[2][1];
		this->I_inv.zz = ip.x*R[0][2]*R[0][2] + ip.y*R[1][2]*R[1][2] + ip.z*R[2][2]*R[2][2];
		this->I_inv.xy = ip.x*R[0][0]*R[0][1] + ip.y*R[1][0]*R[1][1] + ip.z*R[2][0]*R[2][1];
		this->I_inv.xz = ip.x*R[0][0]*R[0][2] + ip.y*R[1][0]*R[1][2] + ip.z*R[2][0]*R[2][2];
		this->I_inv.yz = ip.x*R[0][1]*R[0][2] + ip.y*R[1][1]*R[1][2] + ip.z*R[2][1]*R[2][2];
}



//EOF
