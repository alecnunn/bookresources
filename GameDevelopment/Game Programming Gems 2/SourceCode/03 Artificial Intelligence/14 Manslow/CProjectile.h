//						Tanks
//			(c) Copyright John Manslow
//					  18/04/2001

#ifndef _CProjectile_
#define _CProjectile_

class CProjectile
{
public:
	CProjectile(	const double,			//Initial x position of the projectile
					const double,			//Initial y position of the projectile
					const double,			//Initial x component of its velocity
					const double			//Initial y component of its velocity
					);

	~CProjectile();

		//Called once per time step to update the projectile's velocity and location
	void TimeStep(const double);

		//The projectile's current position
	double dxPosition,dyPosition;

		//Its current velocity
	double dSpeedx,dSpeedy;

		//Its previous position
	double dPreviousxPosition,dPreviousyPosition;

};

#endif