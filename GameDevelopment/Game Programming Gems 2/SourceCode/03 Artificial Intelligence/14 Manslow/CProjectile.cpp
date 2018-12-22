//						Tanks
//			(c) Copyright John Manslow
//					  18/04/2001

#include "stdafx.h"
#include "CProjectile.h"
#include "math.h"

//The neural network doesn't need to know these values because it learns their effect on the
//flight of the projectile during training. If they change, however, the network may need to
//be retrained.
const double dGravity=0.1;
const double dFriction=0.997;

CProjectile::CProjectile(const double dNewxPosition,
						 const double dNewyPosition,
						 const double dNewHeadingX,
						 const double dNewHeadingY)
{
	TRACE("\t\tCreating projectile...");
	//Record the parameters of the projectile:
	//Its current position,
	dxPosition=dNewxPosition;
	dyPosition=dNewyPosition;

	//its position at the end of the previous time step,
	dPreviousxPosition=dxPosition;
	dPreviousyPosition=dyPosition;

	//and the x and y components of its velocity.
	dSpeedx=dNewHeadingX;
	dSpeedy=dNewHeadingY;

	TRACE("successful.\n");
}

CProjectile::~CProjectile()
{
	TRACE("\t\tDestroying projectile...");
	TRACE("successful.\n");
}

void CProjectile::TimeStep(const double dWindSpeed)
{
	//Record its current position as its old position
	dPreviousxPosition=dxPosition;
	dPreviousyPosition=dyPosition;

	//Update the projectile's position
	dxPosition+=dSpeedx;
	dyPosition+=dSpeedy;

	//Update the projectile's velocity. If this model changes, the neural network may have to 
	//be retrained
	dSpeedy-=dGravity;
	dSpeedy*=dFriction;
	dSpeedx+=(dWindSpeed-dSpeedx)*(1.0-dFriction);
}