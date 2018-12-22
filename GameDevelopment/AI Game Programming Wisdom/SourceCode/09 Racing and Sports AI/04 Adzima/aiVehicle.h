#ifndef VEHICLE_H
#define VEHICLE_H

#include "aiMath.h"
#include "aiObstacle.h"

class Vehicle : public aiObstacle 
{
	public:
		Vehicle(){};
		void Reset();
		void Init(int nId,char* pGeoFile);

		bool Stuck();			// return true when the vehicle is stuck.

		void SetSteering(float fSteering);
		void SetBrake(float m_fBrake);
		void SetThrottle(float m_fThrottle);
		void SetTransmissionToReverse();

		Matrix34& GetMatrix();	// Return the vehicle's orientation/position matrix
		Vector3 GetBoxMin();	// Get the bounding box information.
		Vector3 GetBoxMax();
		float Speed();			// Return the vehicles current speed

		int GetRoomId();		// Return the current room that this vehicle is in.

};

#endif
