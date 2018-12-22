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

#include "PMath.h"
#include "RigidBody.h"
#include "CollisionEngine.h"

using namespace PMath;
using Physics::RigidBody;

namespace Collision {

	/** @class Contact
	 */

	class Contact
	{
	public:
		Contact();
		PMath::Vec3f		m_Position;
		PMath::Vec3f		m_Normal;
		Real				m_ContactTime;
		Real				m_PenetrationDepth;
		Physics::RigidBody*	m_pBodyA;
		Physics::RigidBody*	m_pBodyB;
	};

	Contact::Contact() {
	}


/*
                       ____      _ _ _     _
                      / ___|___ | | (_)___(_) ___  _ __
                     | |   / _ \| | | / __| |/ _ \| '_ \
                     | |__| (_) | | | \__ \ | (_) | | | |
                   ___\____\___/|_|_|_|___/_|\___/|_| |_|
                  |  _ \  ___| |_ ___  ___| |_(_) ___  _ __
                  | | | |/ _ \ __/ _ \/ __| __| |/ _ \| '_ \
                  | |_| |  __/ ||  __/ (__| |_| | (_) | | | |
                  |____/ \___|\__\___|\___|\__|_|\___/|_| |_|
 */


typedef bool (*collfn)(Contact*, RigidBody* pBodyA, RigidBody* pBodyB);
typedef void (*resfn) (Contact*);



bool Collide_InfPlane_InfPlane(Contact* pContact, RigidBody* pBodyA, RigidBody* pBodyB)
{
	bool collided;

	// if neither body can move return true if they are not parallel
	if (!(pBodyA->GetTranslatable() & pBodyA->GetSpinnable() & pBodyB->GetTranslatable() & pBodyB->GetSpinnable())) {
		collided = !Equal(pBodyA->m_Extent, pBodyB->m_Extent, kEps);
	}
	else {
		// if either can move, we must assume that the chance that they have not collided is exceedingly small
		collided = true;
	}

	return collided;
}
// cf: www.gamasutra.com/features/19991018/Gomez_1.htm

bool Collide_InfPlane_Sphere(Contact* pContact, RigidBody* pPlaneBody, RigidBody* pSphere)
{
	PMath::Plane* pPlane = &((Physics::Plane*) pPlaneBody->m_pCollideGeo)->m_Plane;

	Vec3f c0, c1;
	PMath::Set(c0, pSphere->m_StateT0.m_Position);
	PMath::Set(c1, pSphere->m_StateT1.m_Position);
	Real d0 = pPlane->DistanceToPoint(c0);
	Real d1 = pPlane->DistanceToPoint(c1);
	Real radius = ((Physics::Sphere*) pSphere->m_pCollideGeo)->m_Radius;

	bool retval = false;

	if (PMath::Abs(d0) <= radius) {
		Set(pContact->m_Normal, pPlane->m_Normal);
		pContact->m_ContactTime = k0;
		retval = true;							// not supposed to get here; this engine never allows intersected positions
	}
	else if (d0 > radius && d1 < radius) {		// if penetrated this frame
		Set(pContact->m_Normal, pPlane->m_Normal);
		Real u = (d0 - radius) / (d0 - d1);		// normalized time of first contact
		Scale(c1, u);
		Scale(c0, k1 - u);
		Add(pContact->m_Position, c0, c1);		// calc center of sphere at point of first contact
		retval = true;
		pContact->m_ContactTime = u;
	}
	return retval;
}

bool Collide_Sphere___InfPlane(Contact* pContact, RigidBody* pSphere, RigidBody* pPlane)
{
	return Collide_InfPlane_Sphere(pContact, pPlane, pSphere);
}

// Quadratic Formula from http://www.gamasutra.com/features/19991018/Gomez_2.htm
// returns true if both roots are real

inline bool QuadraticFormula(Real a, Real b, Real c, Real& root1, Real& root2) 
{
	Real q = b*b - k4 * a * c;
	if (q >= k0) {
		Real sq = Sqrt(q);
		Real d = k1 / (k2 * a);
		root1 = (-b + sq) * d;
		root2 = (-b - sq) * d;
		return true;			// real roots
	}
	else {
		return false;			// complex roots
	}
}

bool Collide_Sphere___Sphere(Contact* pContact, RigidBody* pBodyA, RigidBody* pBodyB)
{
	bool	retval = false;

	Real	u0, u1;
	Real	radiusA = ((Physics::Sphere*) pBodyA->m_pCollideGeo)->m_Radius;
	Real	radiusB = ((Physics::Sphere*) pBodyB->m_pCollideGeo)->m_Radius;
	Vec3f va;
			Subtract(va, pBodyA->m_StateT1.m_Position, pBodyA->m_StateT0.m_Position);
	Vec3f vb;
			Subtract(vb, pBodyB->m_StateT1.m_Position, pBodyB->m_StateT0.m_Position);
	Vec3f vab;
			Subtract(vab, vb, va);
	Vec3f ab;
			Subtract(ab, pBodyB->m_StateT0.m_Position, pBodyA->m_StateT0.m_Position);
	Real	rab = radiusA + radiusB;
	Real	a = Dot(vab, vab);				// u*u coefficient
	Real	b = k2 * Dot(vab, ab);			// u coefficient
	Real	c = Dot(ab, ab) - rab * rab;	// constant term

	// check if they are overlapping
	if (c <= k0) {
		u0 = k0;
		u1 = k0;
		retval = true;
	}

	if (!retval) {
		// they didn't hit if they are moving perfectly parallel and at the same speed
		if (Abs(a) < kEps) {
			retval = false;
		}

		// check if they hit
		else if (QuadraticFormula(a, b, c, u0, u1)) {
			if (u0 > u1) {
				Real temp = u0;
				u0 = u1;
				u1 = temp;
			}
			if (u0 < k0 && u1 < k0) retval = false;
			else if (u1 > k1 && u1 > k1) retval = false;
			else retval = true;
		}
	}

	if (retval) {
		Subtract(pContact->m_Normal, pBodyA->m_StateT1.m_Position, pBodyB->m_StateT1.m_Position);
		Normalize(pContact->m_Normal, pContact->m_Normal);
	}

	return retval;
}

collfn CollisionFunctions[2][2] = {

		// infinite plane			sphere
	Collide_InfPlane_InfPlane,	Collide_InfPlane_Sphere,	// infinite plane
	Collide_Sphere___InfPlane,	Collide_Sphere___Sphere,	// sphere

};

Contact* Collision::Engine::TestCollision(RigidBody* pBodyA, RigidBody* pBodyB)
{
	Contact* pRetVal = 0;

	// get a contact pointer
	if (m_FreePool.size() == 0) {
		APILOG("Contact FreePool is empty\n");
	}

	Contact* pContact = m_FreePool.front();		// get first available
	m_FreePool.pop_front();						// and drop it out of the pool

	if (CollisionFunctions[pBodyA->m_pCollideGeo->GetKind()][pBodyB->m_pCollideGeo->GetKind()](pContact, pBodyA, pBodyB)) {
		m_Contacts.push_back(pContact);
		pContact->m_pBodyA = pBodyA;
		pContact->m_pBodyB = pBodyB;
		pRetVal = pContact;						// point to current contact
	}
	else {
		m_FreePool.push_back(pContact);			// didn't need it, give it back
	}

	return pRetVal;
}

/*
                       ____      _ _ _     _
                      / ___|___ | | (_)___(_) ___  _ __
                     | |   / _ \| | | / __| |/ _ \| '_ \
                     | |__| (_) | | | \__ \ | (_) | | | |
                 ____ \____\___/|_|_|_|___/_|\___/|_| |_|
                |  _ \ ___ ___  ___ | |_   _| |_(_) ___  _ __
                | |_) / _ Y __|/ _ \| | | | | __| |/ _ \| '_ \
                |  _ <  __|__ \ (_) | | |_| | |_| | (_) | | | |
                |_| \_\___|___/\___/|_|\__,_|\__|_|\___/|_| |_|
 */




void Resolve_InfPlane_InfPlane(Contact* pContact)
{
}



/** @todo - check for the condition where an object can be put to sleep
			since it has hit a stationary object, it may be able to come to
			rest upon it
 */

void _ResolveSphereVsPlane(Contact* pContact)
{
	RigidBody* pPlane = pContact->m_pBodyA;
	RigidBody* pSphere = pContact->m_pBodyB;
	Vec3f temp;

	// calculate point of contact on sphere
	Vec3f contact;
	Physics::Sphere* pCSphere = (Physics::Sphere*) pSphere->m_pCollideGeo;
	SetScaled(contact, -pCSphere->m_Radius, pContact->m_Normal);
	
	Vec3f velocityA;
	Set(velocityA, pSphere->m_StateT1.m_Velocity);

	// if object can spin, calculate velocity of the contact point
	if (pSphere->GetSpinnable()) {
		Cross(temp, pSphere->m_StateT1.m_AngularVelocity, contact);
		Add(velocityA, temp);
	}
	
	// velocity is the direction of the collision normal (v dot n)
	Real velNormalComponent = Dot(velocityA, pContact->m_Normal);

	// make sure velocity is towards static object (negative)
	if (velNormalComponent < k0) {
		// impulses change velocities directly without waiting for integration
		// so calculate impulse to fix the velocities
		// we'll use Newton's Law of Restitution for Instaneous Collisions with No Friction
		// as per Chris Hecker's articles in Game Developer magazine

		// the coefficient of restitution relates incoming and outgoing relative normal velocities
		//! @todo restitution coefficient is supposed to be in the PhysicsBody
		#undef RESTITUTION
		#define RESTITUTION Real(0.60f)
		Real impulseNumerator = -(k1 + RESTITUTION) * velNormalComponent;

		Cross(temp, contact, pContact->m_Normal);
		
		if (pSphere->GetInertialKind() == Physics::kI_Sphere) {
			Scale(temp, pSphere->m_InertiaITD[0]);
		}
		else {
			Multiply(temp, temp, pSphere->m_InertiaITD);
		}

		Vec3f temp2;
		Cross(temp2, temp, contact);
		
		// changing impulseDenominator and result to double will make the equations work for objects down to a mass of 0.1
		// with Real, the equations work down to mass of 0.2. 

		Real impulseDenominator = pSphere->GetOOMass() + Dot(temp2, pContact->m_Normal);
		
		// final velocity = initial velocity + (impulse / mass) * normal
		Vec3f impulse;
		Real result = (pSphere->GetOOMass() * impulseNumerator) / impulseDenominator;
		SetScaled(impulse, (Real) result, pContact->m_Normal);
		Add(pSphere->m_StateT1.m_Velocity, impulse);
		
		// calculate angular impulse
		// final angular momentum = init ang vel + (collision point DOT impulse * normal) / inertia tensor
		Cross(temp2, pContact->m_Normal, impulse);
		Add(pSphere->m_StateT1.m_AngularMomentum, temp2);

		// angular velocity will be recalculate on next time step
	}
}




void Resolve_InfPlane_Sphere(Contact* pContact)
{
	RigidBody* pPlaneBody = pContact->m_pBodyA;
	RigidBody* pSphere = pContact->m_pBodyB;
	Vec3f adjust;
	PMath::Set(adjust, pSphere->m_StateT1.m_Position);
	PMath::Subtract(adjust, pContact->m_Position);					// this is the motion removed from the object
	Real residual = PMath::Length(adjust);							// left over motion

	PMath::Set(pSphere->m_StateT1.m_Position, pContact->m_Position);	// move sphere out of trouble

	_ResolveSphereVsPlane(pContact);

	PMath::Set(adjust, pSphere->m_StateT1.m_Velocity);
	PMath::Normalize(adjust, adjust);
	PMath::MultiplyAccumulate(pSphere->m_StateT1.m_Position, residual, adjust);		// add residual motion along the new velocity vector
}

void Resolve_Sphere___InfPlane(Contact* pContact)
{
	RigidBody* pTemp = pContact->m_pBodyA;
	pContact->m_pBodyA = pContact->m_pBodyB;
	pContact->m_pBodyB = pTemp;
	Negate(pContact->m_Normal);

	Resolve_InfPlane_Sphere(pContact);					/// @todo swap!!!
}

void Resolve_Sphere___Sphere(Contact* pContact)
{
	RigidBody* pBodyA = pContact->m_pBodyA;
	RigidBody* pBodyB = pContact->m_pBodyB;

	Vec3f temp,		temp2;
	Vec3f contactA,	contactB;
	Vec3f velocityA,	velocityB, velocityAB;

	// calculate point of contact on body A and add in angular contribution
	Physics::Sphere* pCSphereA = (Physics::Sphere*) pBodyA->m_pCollideGeo;
	Physics::Sphere* pCSphereB = (Physics::Sphere*) pBodyB->m_pCollideGeo;

	SetScaled(contactA, -pCSphereA->m_Radius, pContact->m_Normal);
	SetScaled(contactB,  pCSphereB->m_Radius, pContact->m_Normal);
	Set      (velocityA, pBodyA->m_StateT1.m_Velocity);
	Set      (velocityB, pBodyB->m_StateT1.m_Velocity);

	// add tangential velocity at collision point if the body can spin
	if (pBodyA->GetSpinnable()) {
		Cross(temp, pBodyA->m_StateT0.m_AngularVelocity, contactA);
		Add(velocityA, temp);
	}

	if (pBodyB->GetSpinnable()) {
		Cross(temp, pBodyB->m_StateT0.m_AngularVelocity, contactB);
		Add(velocityB, temp);
	}

	Subtract(velocityAB, velocityA, velocityB);							// calculate relative velocity
	Real velNormalComponent = Dot(velocityAB, pContact->m_Normal);		// get velocity component along collision normal

	if (velNormalComponent < k0) {	// only accept if the two objects are moving towards each other
		// impulses change velocities directly without waiting for integration
		// so calculate impulse to fix the velocities
		// we'll use Newton's Law of Restitution for Instaneous Collisions with No Friction

		// the coefficient of restitution relates incoming and outgoing relative normal velocities		
		//! @todo restitution coefficient is supposed to be in the PhysicsBody
		#undef RESTITUTION
		#define RESTITUTION Real(0.80f)
		Real impulseNumerator = -(k1 + RESTITUTION) * velNormalComponent;

		Cross(temp, contactA, pContact->m_Normal);
		
		if (pBodyA->GetInertialKind() == Physics::kI_Sphere) {
			Scale(temp, pBodyA->m_InertiaITD[0]);
		}
		else {
			Multiply(temp, temp, pBodyA->m_InertiaITD);
		}

		Cross(temp2, temp, contactA);
		
		Real impulseDenominator = pBodyA->GetOOMass() + Dot(temp2, pContact->m_Normal) + pBodyB->GetOOMass();
		
		if (pBodyB->GetInertialKind() == Physics::kI_Sphere) {
			Scale(temp, pBodyB->m_InertiaITD[0]);
		}
		else {
			Multiply(temp, temp, pBodyB->m_InertiaITD);
		}

		Cross(temp2, temp, contactB);
		impulseDenominator += Dot(temp2, pContact->m_Normal);
		
		// final velocity = initial velocity + (impulse / mass) * normal
		Vec3f impulse;
		Real result = (pBodyA->GetOOMass() * impulseNumerator) / impulseDenominator;
		SetScaled(impulse, (Real) result, pContact->m_Normal);
		Add(pBodyA->m_StateT1.m_Velocity, impulse);
		
		// apply opposite impulse to body B
		Vec3f negImpulse;
		SetScaled(negImpulse, kN1 * pBodyB->GetOOMass(), impulse);
		Add(pBodyB->m_StateT1.m_Velocity, negImpulse);

		// calculate angular impulse
		// final angular momentum = init ang vel + (collision point DOT impulse * normal) / inertia tensor
		Cross(temp2, pContact->m_Normal, impulse);
		Add(pBodyA->m_StateT1.m_AngularMomentum, temp2);
		Scale(temp2, kN1);
		Add(pBodyB->m_StateT1.m_AngularMomentum, temp2);

		// angular velocity will be recalculated on next time step
	}
}

resfn ResolveFunctions[2][2] = {

	// infinite plane			sphere
	Resolve_InfPlane_InfPlane,	Resolve_InfPlane_Sphere,	// infinite plane
	Resolve_Sphere___InfPlane,	Resolve_Sphere___Sphere,	// sphere
};

void Engine::Resolve(Contact* pContact)
{
	ResolveFunctions[pContact->m_pBodyA->m_pCollideGeo->GetKind()][pContact->m_pBodyB->m_pCollideGeo->GetKind()](pContact);
	pContact->m_pBodyA->m_Collided = true;
	pContact->m_pBodyB->m_Collided = true;
}

Engine::Engine()
{
	SetCapacity(1024);		// default maximum capacity
}

Engine::~Engine()
{
}

void Engine::Begin()
{
}

void Engine::End()
{
	while (m_Contacts.size() > 0) {
		Contact* pContact = m_Contacts.back();
		m_Contacts.pop_back();
		m_FreePool.push_back(pContact);
	}
}

// capacity can only increase

void Engine::SetCapacity(int maxContacts)
{
	int numToAdd = maxContacts - (int) m_FreePool.size();
	while (--numToAdd >= 0) {
		m_FreePool.push_back(new Contact());
	}
}

}	// end namespace Collision

