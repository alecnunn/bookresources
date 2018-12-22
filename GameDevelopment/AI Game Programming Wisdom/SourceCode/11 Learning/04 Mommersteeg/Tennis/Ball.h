//----------------------------------------------------------------------------------------------
// Sequential Prediction Demo: The positioning pattern
// 
// Author:  Fri Mommersteeg
// Date:    10-09-2001
// File:    Ball.h
//----------------------------------------------------------------------------------------------

#ifndef __TENNISBALL_H
#define __TENNISBALL_H

//----------------------------------------------------------------------------------------------
// Include files
//----------------------------------------------------------------------------------------------

#include "directdraw.h"
#include "observer.h"

//----------------------------------------------------------------------------------------------
// Defined constants
//----------------------------------------------------------------------------------------------

#define GROUND_ACCELERATION		0.1f
#define GROUND_FRICTION			1.5f
#define BALL_SIZE				5
#define OWNER_SERVICE			0
#define UPWARD_BOUNCE_FORCE		3

//----------------------------------------------------------------------------------------------
// CTennisBall: Represents the tennis ball
//----------------------------------------------------------------------------------------------

class CTennisBall : public CSubject {
public:
				CTennisBall() { 
					px = py = pz = vx = vy = vz = 0;
				}

				~CTennisBall() {
				}

	void		Update();
	void		SetPosition(float x, float y, float z) { px = x; py = y; pz = z; }
	void		SetVelocity(float x, float y, float z) { vx = x; vy = y; vz = -z; }
	void		Paint(LPDDS lpdds);
	void		GetBoundingRect(RECT * lpRect);
	int			GetHeight() { return (int)pz; }
	void		GetPosition(POINT & pt) { pt.x = (int)px; pt.y = (int)py; }
	void		SetOwner(int BallOwner) { owner = BallOwner; }
	int			GetOwner() { return owner; }	
	void		GetBouncePosition(POINT & pt) { pt = BouncePosition; } // used by AI
	void		BounceBall(float x, float y);

protected:
	
	void		ApplyVelocity();
	void		ApplyGroundForce();
	void		InterpolateToBouncePosition(POINT & pt); 

protected:
	int			owner;
	float		px, py, pz;
	float		vx, vy, vz;	
	POINT		BouncePosition;
};

//----------------------------------------------------------------------------------------------
#endif // __TENNISBALL_H

