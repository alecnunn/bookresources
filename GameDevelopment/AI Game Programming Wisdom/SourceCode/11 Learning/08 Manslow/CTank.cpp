//Tanks
//Copyright John Manslow
//29/09/2001

#include "stdafx.h"
#include "CTank.h"
#include "CProjectile.h"

const double pi=3.1415926535;

CTank::CTank()
{
	TRACE("\t\tCreating tank...");
	//Initialise member variables to reasonable values
	//The values will be set by the calling function so aren't too critical
	dInclination=45.0*pi/180.0;
	dBarrelx=1.0;
	dBarrely=0.0;
	dxPosition=0;
	dyPosition=0;
	TRACE("successful.\n");
}

CTank::~CTank()
{
	TRACE("\t\tDestroying tank...");
	TRACE("successful.\n");
}

//This function tests for a collision between the projectile and a horizontal 
//line at the base of the tank. Because of this simplistic model, you often 
//can't hit a tank by firing a low flat shot at it. Fortunately, such shots
//are usually prevented by the landscape.
int CTank::nTestForProjectileTankCollision(const CProjectile * const pProjectile)
{
	//Record the movement of the projectile in the last time step
	double dx=pProjectile->dxPosition-pProjectile->dPreviousxPosition;
	double dy=pProjectile->dyPosition-pProjectile->dPreviousyPosition;

	//Compute the time the projectile would have the same y position as the tank
	double dTimeOfIntersection=(dyPosition-pProjectile->dPreviousyPosition)/dy;

	//If that time is not within the current time step a collision cannot have happened
	if(	dTimeOfIntersection<0.0 || 
		dTimeOfIntersection>1.0)
	{
		//Return false to indicate no hit.
		return 0;
	}

	//We know that the projectile passed through the same vertical position as occupied by
	//the tank in the last time step but we need to know if the projectile was close to the
	//tank's horizontal position when it did so.

	//Calculate the projectile's x position when its y position was the same as that of the 
	//tank
	double xAtIntersection=pProjectile->dPreviousxPosition+dx*dTimeOfIntersection;

	//Test to see if it is within the length of the tank
	if(	xAtIntersection<dxPosition-10.0 || 
		xAtIntersection>dxPosition+12.0)
	{
		//If not, return 0 to indicate that there was no hit.
		return 0;
	}

	//Otherwise, a hit has happened, so return 1.
	return 1;
}