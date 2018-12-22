/*
---------------------------------------------------------------------------------------------------
Meshula Physics Demo
Created for Games Programming Gems IV
Copyright (c) 2003 Nick Porcino, http://meshula.net

The MIT License: http://www.opensource.org/licenses/mit-license.php

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or 
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

---------------------------------------------------------------------------------------------------
*/

#include <map>

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif
#if defined(__APPLE__) && defined(__MACH__)
	#include <OpenGL/gl.h>		// Header File For The OpenGL32 Library
	#include <OpenGL/glu.h>	// Header File For The GLu32 Library
#else
	#include <GL/gl.h>		// Header File For The OpenGL32 Library
	#include <GL/glu.h>	// Header File For The GLu32 Library
#endif

#include "CollisionEngine.h"
#include "PhysicsEngine.h"
#include "RigidBody.h"
#include "Spring.h"

// hoists

using std::map;
using namespace PMath;
using namespace Collision;
using Physics::Engine;
using Physics::RigidBody;
using Physics::Spring;

// typedefs 

namespace Physics {
	typedef std::map<int, RigidBody*> RigidBodyMap;		//!< maps unique IDs to RigidBody pointers.
	typedef std::map<int, Spring*> SpringMap;
}


// Physics

static uint32 UniqueID()
{
	static uint32 id = 0;
	return ++id;
}

#define VERIFY_RIGIDBODY(value) {if (m_pAux->m_Bodies


/** @todo instrument all of physics api with APILOG routines */
/** @todo	change API log to output actual source code, so that a complete physics set up can be 
			built up from API calls, so that someone can debug physics code without having to 
			have an entire game along for the ride
 */

#define BOOLSTRING(a) ((a==true) ? "true" : "false")

static char* BOOLPROPSTRING(Physics::Engine::ERigidBodyBool prop) {
	switch (prop) {
	case Physics::Engine::propActive:				return "propActive";
	case Physics::Engine::propUseGravity:			return "propUseGravity";
	case Physics::Engine::propCollidable:			return "propCollidable";
	case Physics::Engine::propSpinnable:				return "propSpinnable";
	case Physics::Engine::propTranslatable:			return "propTranslatable";
	}
	return "unknown";
}

static char* SCALARPROPSTRING(Physics::Engine::ERigidBodyScalar prop) {
	switch (prop) {
	case Physics::Engine::propAngularVelocityDamp:	return "propAngularVelocityDamp";
	case Physics::Engine::propLinearVelocityDamp:	return "propLinearVelocityDamp";
	case Physics::Engine::propMass:					return "propMass";
	}
	return "unknown";
}

static char* Vec3fPROPSTRING(Physics::Engine::ERigidBodyVector prop) {
	switch (prop) {
	case Physics::Engine::propDimensions:			return "propDimensions";
	case Physics::Engine::propPosition:				return "propPosition";
	case Physics::Engine::propVelocity:				return "propVelocity";
	}
	return "unknown";
}


namespace Physics {
/** @class PEAux
	The auxiliary data structures, hidden from the user
 */
	class PEAux 
	{
	public:
		PEAux() : m_pCollisionCallback(0) { 
			m_Gravity[0]	= k0;
			m_Gravity[1]	= k0;
			m_Gravity[2]	= Real(0.98);
			m_MinTimeStep	= 1.0f / 50.0f;
		}

		~PEAux() { }

		Real					m_MinTimeStep;
		Vec3f					m_Gravity;
		Physics::RigidBodyMap	m_Bodies;				//!< contains all the bodies in the simulation
		Physics::SpringMap		m_Springs;				//!< contains all the springs in the simulation
		ICallback*				m_pCollisionCallback;
		Collision::Engine		m_CollisionEngine;
	};
}

Physics::Engine::Engine()
{
	m_pAux = new PEAux();

	//--------------------------------------------------------------
	APILOG("------------------------\n");
	APILOG("Physics();\n");
	//--------------------------------------------------------------
}

Physics::Engine::~Engine()
{
	delete m_pAux;
	m_pAux = 0;

	//--------------------------------------------------------------
	APILOG("~Physics();\n");
	APILOG("------------------------\n");
	//--------------------------------------------------------------
}

void Physics::Engine::SetGravity(PMath::Vec3f val)
{
	Set(m_pAux->m_Gravity, val);

	//--------------------------------------------------------------
	APILOG("SetGravity(%f, %f, %f);\n", val[0], val[1], val[2]);
	//--------------------------------------------------------------
}

void Physics::Engine::SetCollisionCallback(ICallback* pCB)
{
	m_pAux->m_pCollisionCallback = pCB;

	//--------------------------------------------------------------
	APILOG("SetCollisionCallback();\n");
	//--------------------------------------------------------------
}

uint32 Physics::Engine::AddRigidBodySphere(Real radius)
{
	uint32 id				= UniqueID();
	RigidBody* pBody		= new RigidBody();
	IGeometry* pCollide		= new Sphere(radius);
	m_pAux->m_Bodies[id]	= pBody;

	pBody->SetInertialKind(kI_Sphere);
	pBody->SetCollisionObject(pCollide);
	pBody->SetRender(new GraphObj::Sphere());

	//--------------------------------------------------------------
	APILOG("%d = AddRigidBodySphere(%f)\n", id, radius);
	//--------------------------------------------------------------

	return id;
}

uint32 Physics::Engine::AddRigidBodyPlane(PMath::Plane& plane)
{
	uint32 id				= UniqueID();
	RigidBody* pBody		= new RigidBody();
	IGeometry* pCollide		= new Plane(plane);
	m_pAux->m_Bodies[id]	= pBody;

	pBody->SetInertialKind(kI_Immobile);
	pBody->SetCollisionObject(pCollide);
	pBody->SetSpinnable(false);
	pBody->SetTranslatable(false);
	pBody->SetRender(new GraphObj::Plane());

	//--------------------------------------------------------------
	APILOG("%d = AddRigidBodyPlane()\n", id);
	//--------------------------------------------------------------

	return id;
}

/*
	Before removing a rigid body, any springs that it is attached to must also bite the dust
 */

bool Physics::Engine::RemoveRigidBody(uint32 id)
{
	bool retval = false;

	if (m_pAux->m_Bodies.count(id) != 0) {

		// scan all springs, and remove any that are attached to the current object
		Physics::SpringMap::iterator iter;
		for (iter = m_pAux->m_Springs.begin(); iter != m_pAux->m_Springs.end(); ++iter) {
			Spring* pSpring = iter->second;
			if (pSpring->m_BodyA == id || pSpring->m_BodyB == id) {
				APILOG("RemoveRigidBody side-effect: Removing Spring %d\n", iter->first);
				RemoveSpring(iter->first);
				iter = m_pAux->m_Springs.begin();
			}
		}

		RigidBody* pBody = m_pAux->m_Bodies[id];
		m_pAux->m_Bodies.erase(id);
		delete pBody;
		retval = true;
	}

	//--------------------------------------------------------------
	APILOG("%s = RemoveRigidBody(%d)\n", BOOLSTRING(retval), id);
	//--------------------------------------------------------------

	return retval;
}

void Physics::Engine::RemoveAll()
{
	Physics::RigidBodyMap::iterator rbIter;
	Physics::SpringMap::iterator	springIter;

	for (rbIter = m_pAux->m_Bodies.begin(); rbIter != m_pAux->m_Bodies.end(); ++rbIter) {
		RigidBody* pBody = rbIter->second;
		delete pBody;
	}
	for (springIter = m_pAux->m_Springs.begin(); springIter != m_pAux->m_Springs.end(); ++ springIter) {
		Spring* pSpring = springIter->second;
		delete pSpring;
	}

	m_pAux->m_Bodies.clear();
	m_pAux->m_Springs.clear();
}

uint32 Physics::Engine::AddSpring()
{
	uint32 id = UniqueID();
	Spring* pSpring = new Spring();
	m_pAux->m_Springs[id] = pSpring;

	//--------------------------------------------------------------
	APILOG("%d = AddSpring()\n", id);
	//--------------------------------------------------------------

	return id;
}

bool Physics::Engine::RemoveSpring(uint32 id)
{
	bool retval = false;
	if (m_pAux->m_Springs.count(id) != 0) {
		Spring* pSpring = m_pAux->m_Springs[id];
		m_pAux->m_Springs.erase(id);
		delete pSpring;
		retval = true;
	}

	//--------------------------------------------------------------
	APILOG("%s = RemoveSpring(%d)\n", BOOLSTRING(retval), id);
	//--------------------------------------------------------------

	return retval;
}


void Physics::Engine::SetRigidBodyBool(uint32 id, ERigidBodyBool prop, bool value)
{
	if (m_pAux->m_Bodies.count(id) != 0) {
		RigidBody* pBody = m_pAux->m_Bodies[id];
		switch (prop) {
		case propActive:		pBody->SetActive(value);		break;
		case propUseGravity:	pBody->SetGravity(value);		break;
		case propCollidable:	pBody->SetCollidable(value);	break;
		case propSpinnable:		pBody->SetSpinnable(value);		break;
		case propTranslatable:	pBody->SetTranslatable(value);	break;
		}
	}

	//--------------------------------------------------------------
	APILOG("SetBoolProperty(%d, %s, %s)\n", id, BOOLPROPSTRING(prop), BOOLSTRING(value));
	//--------------------------------------------------------------
}

bool Physics::Engine::GetRigidBodyBool(uint32 id, ERigidBodyBool prop)
{
	bool retval = false;
	if (m_pAux->m_Bodies.count(id) != 0) {
		RigidBody* pBody = m_pAux->m_Bodies[id];
		switch (prop) {
		case propActive:		retval = pBody->GetActive();		break;
		case propUseGravity:	retval = pBody->GetGravity();		break;
		case propCollidable:	retval = pBody->GetCollidable();	break;
		case propSpinnable:		retval = pBody->GetSpinnable();		break;
		case propTranslatable:	retval = pBody->GetTranslatable();	break;
		}
	}
	return retval;
}


void Physics::Engine::SetRigidBodyScalar(uint32 id, ERigidBodyScalar prop, Real value)
{
	if (m_pAux->m_Bodies.count(id) != 0) {
		RigidBody* pBody = m_pAux->m_Bodies[id];
		switch (prop) {
		case propAngularVelocityDamp:	pBody->SetAngularVelocityDamp(value); 		break;
		case propLinearVelocityDamp:	pBody->SetLinearVelocityDamp(value);		break;
		case propMass:					
			pBody->SetMass(value);		
			if (value > 0.2f) {
				APILOG("Mass less than 0.2 will not collide properly due to float resolution\n");
			}
			break;
		}
	}

	//--------------------------------------------------------------
	APILOG("SetScalarProperty(%d, %s, %f)\n", id, SCALARPROPSTRING(prop), value);
	//--------------------------------------------------------------
}

Real Physics::Engine::GetRigidBodyScalar(uint32 id, ERigidBodyScalar prop)
{
	Real retval = k0;
	if (m_pAux->m_Bodies.count(id) != 0) {
		RigidBody* pBody = m_pAux->m_Bodies[id];
		switch (prop) {
		case propAngularVelocityDamp:	retval = pBody->GetAngularVelocityDamp();	break;
		case propLinearVelocityDamp:	retval = pBody->GetLinearVelocityDamp();	break;
		case propMass:					retval = pBody->GetMass();					break;
		}
	}
	return retval;
}

void Physics::Engine::SetRigidBodyVec3f(uint32 id, ERigidBodyVector prop, Vec3f value)
{
	if (m_pAux->m_Bodies.count(id) != 0) {
		RigidBody* pBody = m_pAux->m_Bodies[id];
		switch (prop) {
		case propDimensions:
			Set(pBody->m_Extent, value);
			if (pBody->GetInertialKind() == kI_Sphere) {
				GraphObj::Sphere* pSphere = (GraphObj::Sphere*) pBody->m_pRender;
				pSphere->m_Radius = value[0] * kHalf;
				Sphere* pCSphere = (Sphere*) pBody->m_pCollideGeo;
				pCSphere->m_Radius = value[0] * kHalf;
			}
			break;

		case propPosition:				Set(pBody->m_StateT1.m_Position, value);	break;
		case propVelocity:				Set(pBody->m_StateT1.m_Velocity, value);	break;
		}
	}

	//--------------------------------------------------------------
	APILOG("SetVec3fProperty(%d, %s, %f, %f, %f)\n", id, Vec3fPROPSTRING(prop), value[0], value[1], value[2]);
	//--------------------------------------------------------------
}

static Vec3f vdummy = { k0, k0, k0 };

Vec3f* Physics::Engine::GetRigidBodyVec3fPtr(uint32 id, ERigidBodyVector prop)
{
	Vec3f* retval = &vdummy;
	if (m_pAux->m_Bodies.count(id) != 0) {
		RigidBody* pBody = m_pAux->m_Bodies[id];
		switch (prop) {
		case propDimensions:			retval = &pBody->m_Extent;					break;
		case propPosition:				retval = &pBody->m_StateT1.m_Position;		break;
		case propVelocity:				retval = &pBody->m_StateT1.m_Velocity;		break;
		}
	}
	return retval;
}


void Physics::Engine::SetRigidBodyQuat(uint32 id, ERigidBodyQuat prop, Quaternion value)
{
	if (m_pAux->m_Bodies.count(id) != 0) {
		RigidBody* pBody = m_pAux->m_Bodies[id];
		switch (prop) {
		case propOrientation:			Set(pBody->m_StateT1.m_Orientation, value);	break;
		}
	}
}

static Quaternion qdummy = { k0, k0, k0, k1 };

Quaternion* Physics::Engine::GetRigidBodyQuatPtr(uint32 id, ERigidBodyQuat prop)
{
	Quaternion* retval = &qdummy;
	if (m_pAux->m_Bodies.count(id) != 0) {
		RigidBody* pBody = m_pAux->m_Bodies[id];
		switch (prop) {
		case propOrientation:			retval = &pBody->m_StateT1.m_Orientation;	break;
		}
	}
	return retval;
}

void Physics::Engine::GetRigidBodyTransformMatrix(uint32 id, Mat44& pResult)
{
	if (m_pAux->m_Bodies.count(id) != 0) {
		RigidBody* pBody = m_pAux->m_Bodies[id];
		if (pBody->GetSpinnable()) {
			QuatToBasis(pResult, pBody->m_StateT1.m_Orientation);
			pResult[3] = k0; pResult[7] = k0; pResult[11] = k0; pResult[15] = k1;
		}
		else {
			Identity(pResult);
		}
		SetTranslation(pResult, pBody->m_StateT1.m_Position);
	}
}

void Physics::Engine::SetSpringBool(uint32 id, ESpringBool prop, bool value)
{
	if (m_pAux->m_Springs.count(id) != 0) {
		Spring* pSpring = m_pAux->m_Springs[id];
		if (prop == propSpringCompressible) {
			pSpring->m_Compressible = value;
		}
	}
}

bool Physics::Engine::GetSpringBool(uint32 id, ESpringBool prop)
{
	bool retval = false;
	if (m_pAux->m_Springs.count(id) != 0) {
		Spring* pSpring = m_pAux->m_Springs[id];
		if (prop == propSpringCompressible) {
			retval = pSpring->m_Compressible;
		}
	}
	return retval;
}

void Physics::Engine::SetSpringUInt32(uint32 id, ESpringUint32 prop,	uint32 value)
{
	if (m_pAux->m_Springs.count(id) != 0) {
		Spring* pSpring = m_pAux->m_Springs[id];
		if (prop == propBodyA) {
			if (m_pAux->m_Bodies.count(value) != 0) {
				pSpring->m_BodyA = value;
				pSpring->m_pBodyA = m_pAux->m_Bodies[value];
			}
			else {
				APILOG("Can't attach spring to nonexistant rigid body\n");
			}
		}
		else if (prop == propBodyB) {
			if (m_pAux->m_Bodies.count(value) != 0) {
				pSpring->m_BodyB = value;
				pSpring->m_pBodyB = m_pAux->m_Bodies[value];
			}
			else {
				APILOG("Can't attach spring to nonexistant rigid body\n");
			}
		}
	}
}

uint32 Physics::Engine::GetSpringUInt32(uint32 id, ESpringUint32 prop)
{
	int retval = 0;
	if (m_pAux->m_Springs.count(id) != 0) {
		Spring* pSpring = m_pAux->m_Springs[id];
		if (prop == propBodyA) {
			retval = pSpring->m_BodyA;
		}
		else if (prop == propBodyB) {
			retval = pSpring->m_BodyB;
		}
	}
	return retval;
}

void Physics::Engine::SetSpringScalar(uint32 id, ESpringScalar prop,	Real value)
{
	if (m_pAux->m_Springs.count(id) != 0) {
		Spring* pSpring = m_pAux->m_Springs[id];
		switch (prop) {
			case propSpringStiffness:	
				pSpring->m_Stiffness = value;	
				break;

			case propSpringRestLength:	
				pSpring->m_RestLength = value;	
				pSpring->m_PrevLength = value;
				break;

			case propSpringDamping:		
				pSpring->m_Damping = value;		
				if (value > k1 || value < k0) {
					APILOG("Illegal damping value: %f (0 is no damping, 1 is critical damping)\n", value);
				}
				break;
		}
	}
}

Real Physics::Engine::GetSpringScalar(uint32 id, ESpringScalar prop)
{
	Real retval = k0;
	if (m_pAux->m_Springs.count(id) != 0) {
		Spring* pSpring = m_pAux->m_Springs[id];
		switch (prop) {
			case propSpringStiffness:	retval = pSpring->m_Stiffness;	break;
			case propSpringDamping:		retval = pSpring->m_Damping;	break;
			case propSpringRestLength:	retval = pSpring->m_RestLength;	break;
		}
	}
	return retval;
}

void Physics::Engine::SetSpringVec3f(uint32 id, ESpringVector prop,	PMath::Vec3f value)
{
	if (m_pAux->m_Springs.count(id) != 0) {
		Spring* pSpring = m_pAux->m_Springs[id];
		if (prop == propAttachPointA) {
			Set(pSpring->m_PosA, value);
			pSpring->m_CenterAttachA = IsZero(value);
		}
		else if (prop == propAttachPointB) {
			Set(pSpring->m_PosB, value);
			pSpring->m_CenterAttachB = IsZero(value);
		}
	}
}

PMath::Vec3f* Physics::Engine::GetSpringVec3fPtr(uint32 id, ESpringVector prop)
{
	PMath::Vec3f* retval = 0;
	if (m_pAux->m_Springs.count(id) != 0) {
		Spring* pSpring = m_pAux->m_Springs[id];
		if (prop == propAttachPointA) {
			retval = &pSpring->m_PosA;
		}
		else if (prop == propAttachPointB) {
			retval = &pSpring->m_PosB;
		}
	}
	return retval;
}


void Physics::Engine::AddImpulse(uint32 id, Vec3f force)
{
	if (m_pAux->m_Bodies.count(id) != 0) {
		RigidBody* pBody = m_pAux->m_Bodies[id];
		if (pBody->GetTranslatable()) {
			pBody->m_Acc.AddForce(force);
		}
	}
}

void Physics::Engine::AddTwist(uint32 id, Vec3f torque)
{
	if (m_pAux->m_Bodies.count(id) != 0) {
		RigidBody* pBody = m_pAux->m_Bodies[id];
		if (pBody->GetSpinnable()) {
			pBody->m_Acc.AddTorque(torque);
		}
	}
}

void Physics::Engine::StopMoving(uint32 id)
{
	if (m_pAux->m_Bodies.count(id) != 0) {
		RigidBody* pBody = m_pAux->m_Bodies[id];
		if (pBody->GetSpinnable()) {
			Zero(pBody->m_Acc.m_Force);
			Zero(pBody->m_StateT0.m_Velocity);
			Zero(pBody->m_StateT1.m_Velocity);
		}
	}
}

void Physics::Engine::StopSpinning(uint32 id)
{
	if (m_pAux->m_Bodies.count(id) != 0) {
		RigidBody* pBody = m_pAux->m_Bodies[id];
		if (pBody->GetSpinnable()) {
			Zero(pBody->m_Acc.m_Torque);
			Zero(pBody->m_StateT0.m_AngularVelocity);
			Zero(pBody->m_StateT1.m_AngularVelocity);
		}
	}
}

void Physics::Engine::SetMinTimeStep(Real dt)
{
	m_pAux->m_MinTimeStep = dt;
}

void Physics::Engine::Simulate(Real dt)
{
	/// @todo calculate timestep for numerical stability
	// if the framerate is less than 50Hz, subdivide the time step
	// 50Hz matches the stability requirement for stiffness with the demo's springs
	// in reality, we would calculate the time step

	int steps;

	if (dt > m_pAux->m_MinTimeStep) {
		steps = 1 + (int) (dt / m_pAux->m_MinTimeStep);
		dt /= (float) steps;
	}
	else {
		steps = 1;
	}

	for (int i = 0; i < steps; ++i) {
		Physics::RigidBodyMap::iterator rbIter;
		Physics::SpringMap::iterator	springIter;

		// reset simulation

		for (rbIter = m_pAux->m_Bodies.begin(); rbIter != m_pAux->m_Bodies.end(); ++rbIter) {
			RigidBody* pBody = rbIter->second;

			// if the body is taking part in the simulation (asleep or awake)
			if (pBody->GetActive()) {
				/// @todo reawaken objects which have been put to sleep but which can't sleep any more
				/// @todo bail early if asleep
				pBody->ResetForNextTimeStep();
			}
		}

		// loop over all objects,
		//			if not asleep
		//				integrate first half of time step

		for (rbIter = m_pAux->m_Bodies.begin(); rbIter != m_pAux->m_Bodies.end(); ++rbIter) {
			RigidBody* pBody = rbIter->second;

			// if the body is taking part in the simulation (asleep or awake)
			if (pBody->GetActive()) {
				/// @todo do any initial constraint set up here
				/// @todo reset the collided with an immovable object flag here
				pBody->Integrate1(dt, m_pAux->m_Gravity);
			}
		}

		// loop over all springs
		//		add forces to appropriate bodies

		for (springIter = m_pAux->m_Springs.begin(); springIter != m_pAux->m_Springs.end(); ++ springIter) {
			Spring* pSpring = springIter->second;

			Vec3f* pPosA = &pSpring->m_pBodyA->m_StateT1.m_Position;
			Vec3f* pPosB = &pSpring->m_pBodyB->m_StateT1.m_Position;

			Vec3f direction;
			Subtract(direction, *pPosA, *pPosB);
			Real length = Length(direction);

			Real x = length - pSpring->m_RestLength;

			if (pSpring->m_Compressible || (x > k0)) {
				Real force = pSpring->m_Stiffness * x;

				// damping -b * difference in length between this frame and previous
				Real v = (pSpring->m_PrevLength - x);
				force += v * pSpring->m_Damping;

				pSpring->m_PrevLength = length;

				MultiplyAccumulate(pSpring->m_pBodyA->m_Acc.m_Force, -force, direction);
				MultiplyAccumulate(pSpring->m_pBodyB->m_Acc.m_Force,  force, direction);
			}
		}

		// loop over all objects,
		//			if not asleep
		//				integrate second half of time step

		for (rbIter = m_pAux->m_Bodies.begin(); rbIter != m_pAux->m_Bodies.end(); ++rbIter) {
			RigidBody* pBody = rbIter->second;

			// if the body is taking part in the simulation (asleep or awake)
			if (pBody->GetActive()) {
				/// @todo do any initial constraint set up here
				/// @todo reset the collided with an immovable object flag here
				pBody->Integrate2(dt, m_pAux->m_Gravity);
			}
		}

		// loop over all objects,
		//		if active, 
		//			satisfy constraints

		// loop over all objects,
		//		if active, 
		//			detect and resolve collisions

		m_pAux->m_CollisionEngine.Begin();

		for (rbIter = m_pAux->m_Bodies.begin(); rbIter != m_pAux->m_Bodies.end(); ++rbIter) {
			RigidBody* pBodyA = rbIter->second;
			if (pBodyA->GetCollidable()) {
				Physics::RigidBodyMap::iterator rbIter2;
				rbIter2 = rbIter;
				++rbIter2;													// get the rigid body beyond the current outer loop
				for ( ; rbIter2 != m_pAux->m_Bodies.end(); ++rbIter2) {
					RigidBody* pBodyB = rbIter2->second;
					m_pAux->m_CollisionEngine.TestCollision(pBodyA, pBodyB);			
				}
			}
		}

	//
	// this demo is intended to demonstrate integration, not collisiond detection and integration,
	// so this resolution function is a toy.
	//
	// this simple version assumes all collisions must be resolved, which doesn't work in real life.
	// It only works in this demo because there are very few objects, and few simultaneous
	// collisions (usually zero or one) on any particular object at any particular time.
	//
	// the simplest possible solution usable in a game or other simulation 
	// is the Quake style resolution described in 
	//
	// Generic Collision Detection for Games Using Ellipsoids
	// by Paul Nettle (do a Google search)
	//
	// The basic idea of Paul's system is to search for the earliest collision for each object,
	//	and resolve only that one. After the resolution, re-detect collisions, and loop until 
	//	no more collisions exist.
	//
	// a real physics engine would need a system such as that described in
	//
	// Fast Contact Reduction for Dynamics Simulation
	// Adam Moravanszky, and Pierre Terdiman
	// Games Programming Gems IV
	//
	// If a robust scheme such as that one is used, a constraint system or a temporary spring
	// solution can be used to resolve instead.
	//
	// Nonetheless, whatever solution you choose, you should attempt, if possible, to use one
	// of the resolvers here, because they are analytic, correct, and as fast as possible if
	// multiple simultaneous collisions on the same object don't need to be resolved, and if
	// the involved objects are combinations of spheres and planes.
	//

		std::vector<Contact*>::iterator contactIter;

		for (contactIter = m_pAux->m_CollisionEngine.m_Contacts.begin(); contactIter != m_pAux->m_CollisionEngine.m_Contacts.end(); ++contactIter) {
			m_pAux->m_CollisionEngine.Resolve(*contactIter);
		}

		m_pAux->m_CollisionEngine.End();

		// loop over all objects,
		//		if active, 
		//			renormalize states

		for (rbIter = m_pAux->m_Bodies.begin(); rbIter != m_pAux->m_Bodies.end(); ++rbIter) {
			RigidBody* pBody = rbIter->second;
			pBody->Renormalize();
		}
	}
}

void Physics::Engine::Render()
{
	Physics::RigidBodyMap::iterator iter;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0f, 0.3f, 0.3f);

	glEnable(GL_LIGHTING);
	for (iter = m_pAux->m_Bodies.begin(); iter != m_pAux->m_Bodies.end(); ++iter) {
		RigidBody* pBodyA = iter->second;
		pBodyA->Render();
	}

	glDisable(GL_LIGHTING);
	glColor3f(0.6f, 0.0f, 0.0f);
	Physics::SpringMap::iterator sIter;
	for (sIter = m_pAux->m_Springs.begin(); sIter != m_pAux->m_Springs.end(); ++sIter) {
		Spring* pSpring = sIter->second;
		glBegin(GL_LINES);
		Vec3f* pPosA = &pSpring->m_pBodyA->m_StateT1.m_Position;
		Vec3f* pPosB = &pSpring->m_pBodyB->m_StateT1.m_Position;
		glVertex3f((*pPosA)[0], (*pPosA)[1], (*pPosA)[2]);
		glVertex3f((*pPosB)[0], (*pPosB)[1], (*pPosB)[2]);
		glEnd();
	}

	glPopMatrix();
}
