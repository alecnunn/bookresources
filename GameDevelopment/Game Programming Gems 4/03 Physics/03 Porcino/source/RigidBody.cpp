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

/*
	RigidBody.cpp
 */

//////////////////// standard includes

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

//////////////////// physics demo includes

#include "PMath.h"
#include "PhysicsEngine.h"
#include "RigidBody.h"
#include "CollisionEngine.h"

//////////////////// namespace hoists

using namespace PMath;
using Physics::RigidBody;

//////////////////// constructor/destructor

RigidBody::RigidBody() : m_Active(true), m_Spinnable(false), m_Translatable(false), m_Collidable(false), m_pCollideGeo(0), m_pRender(0),
	m_Collided(false)
{
	SetDefaults();
}

RigidBody::~RigidBody() { delete m_pCollideGeo; delete m_pRender; }

void RigidBody::SetDefaults()
{
	m_LinearVelocityDamp = Real(0.9995);
	m_AngularVelocityDamp = Real(0.9995);
	m_Mass = k1;
	m_OOMass = k1;
	m_Extent[0] = kHalf;	// object extends to 1/2 from origin; ie, object is of unit size
	m_Extent[1] = kHalf;
	m_Extent[2] = kHalf;
	m_Spinnable = false;
	m_StateT0.SetAtRest();
	m_StateT1 = m_StateT0;
	m_Acc.Clear();
	m_Collided = false;
	Zero(m_AccelPrev);
}

//////////////////// sets/gets

void RigidBody::SetInertialKind(Physics::EInertialKind ikind) 
{ 
	m_InertialKind = ikind; 
	CalculateInertiaTensor();
}

void RigidBody::SetMass(Real mass)
{
	m_Mass = mass;
	m_OOMass = k1 / mass;

	CalculateInertiaTensor();
}

//////////////////// utility

void RigidBody::CalculateInertiaTensor()
{
	if (m_InertialKind == Physics::kI_Sphere) {
		m_InertiaITD[0] = k5 / (k2 * m_Mass * Sqr(m_Extent[0]));		// I = (2Mr*r) / 5
	}
	else {
		Real x2 = Sqr(m_Extent[0]);			// get the radius squared
		Real y2 = Sqr(m_Extent[1]);
		Real z2 = Sqr(m_Extent[2]);
		
		switch (m_InertialKind)
		{
		case Physics::kI_Box:
			m_InertiaITD[0] = k12 / (m_Mass * k4 * (y2 + z2));	// multiply each dimension by 2 squared because x2 etc are half extent
			m_InertiaITD[1] = k12 / (m_Mass * k4 * (x2 + z2));
			m_InertiaITD[2] = k12 / (m_Mass * k4 * (x2 + y2));
			break;
				
		case Physics::kI_Ellipsoid:
			m_InertiaITD[0] = k5 / (m_Mass * (y2 + z2));
			m_InertiaITD[1] = k5 / (m_Mass * (x2 + z2));
			m_InertiaITD[2] = k5 / (m_Mass * (x2 + y2));
			break;

		case Physics::kI_Cylinder:
			m_InertiaITD[0] =												// x
			m_InertiaITD[2] = k12 / (m_Mass * (k3 * x2 + y2));				// z
			m_InertiaITD[1] = k2  / (m_Mass * x2);							// y
			break;
		
		case Physics::kI_CylinderBottomHeavy:
			m_InertiaITD[0]  =												// x
			m_InertiaITD[2]  = k12 / (m_Mass * (k3 * x2 + k4 * y2));			// z
			m_InertiaITD[1]  = k2  / (m_Mass * x2);							// y
			break;
			
		case Physics::kI_Hemisphere:
			m_InertiaITD[0] =												// x
			m_InertiaITD[2] = Real(320.0f) / (m_Mass * (Real(83.0f) * x2));	// z
			m_InertiaITD[1] = k5           / (m_Mass * (k2 * x2));			// y
			break;

		case Physics::kI_HemisphereBottomHeavy:
			m_InertiaITD[0]  = 
			m_InertiaITD[1]  = 
			m_InertiaITD[2]  = k5 / (m_Mass * (k2 * x2));
			break;
			
		case Physics::kI_HollowSphere:
			{
				Real a3 = y2 * m_Extent[1];
				Real a5 = a3 * y2;
				Real b3 = x2 * m_Extent[0];
				Real b5 = b3 * x2;
				m_InertiaITD[0]  = 
				m_InertiaITD[1]  = 
				m_InertiaITD[2]  = (k5 * (a3 - b3)) / (k2 * m_Mass * (a5 - b5));
			}
			break;
			
		case Physics::kI_Hoop:	// radius squared in x2
			m_InertiaITD[0] =														// x
			m_InertiaITD[2] = k1 / (m_Mass * x2);										// z
			m_InertiaITD[1] = k2 / (m_Mass * x2);										// y
			break;
			
		case Physics::kI_CylinderThinShell:	// x2 = radius squared, y2 = height squared
			m_InertiaITD[0] =														// x
			m_InertiaITD[2] = Real(12.0f) / (m_Mass * (Real(6.0f) * x2 + y2));		// z
			m_InertiaITD[1] = k1          / (m_Mass * x2);							// y
			break;
			
		case Physics::kI_CylinderThinShellBottomHeavy:	// x2 = radius squared, y2 = height squared
			m_InertiaITD[0] =														// x
			m_InertiaITD[2] = Real(6.0f) / (m_Mass * (k3 * x2 + k2 * y2));			// z
			m_InertiaITD[1] = k1         / (m_Mass * x2);								// y
			break;
			
		case Physics::kI_Torus:	// x2 = radius, y2 = secondary radius
			m_InertiaITD[0] =														// x
			m_InertiaITD[2] = k8 / (m_Mass * (k5 * y2 + k4 * x2));					// z
			m_InertiaITD[1] = k4 / (m_Mass * k3 * y2 + k4 * x2);						// y
			break;
		}
	}
}


void RigidBody::Render() 
{ 
	if (m_pRender) {
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		if (m_pCollideGeo->GetKind() == Physics::kC_Sphere) {
			glTranslatef(m_StateT1.m_Position[0], m_StateT1.m_Position[1], m_StateT1.m_Position[2]);
			float scale = (float) ((Physics::Sphere*) m_pCollideGeo)->m_Radius;
			glScalef(scale, scale, scale);
		}
		m_pRender->Render(); 

		glPopMatrix();
	}
}

/*
              ___       _                       _   _
             |_ _|_ __ | |_ ___  __ _ _ __ __ _| |_(_) ___  _ __
              | || '_ \| __/ _ \/ _` | '__/ _` | __| |/ _ \| '_ \
              | || | | | ||  __/ (_| | | | (_| | |_| | (_) | | | |
             |___|_| |_|\__\___|\__, |_|  \__,_|\__|_|\___/|_| |_|
                                |___/
 */

bool RigidBody::ResetForNextTimeStep()
{
	bool fellAsleep = false;

	// normally in a Verlet integrator, you need the previous state in order to derive velocity
	// in this engine, since we are tracking velocity explicitly, we can simply copy t1 over t0
	// if you were to switch the engine to derive acceleration from velocity, you would need a different approach
	m_StateT0 = m_StateT1;

	if (m_Translatable) {
		if (m_Collided) {
			// much more interesting friction is anisotropic; for example, a tire has much more transverse than lateral friction
			// coulomb friction - a force opposing motion

			Real vel = Length(m_StateT0.m_Velocity);
			MultiplyAccumulate(m_Acc.m_Force, Real(-0.95f) * vel, m_StateT0.m_Velocity);
		}
		else {
			// linear velocity damping, use to simulate viscosity
			Real forceSquared = Dot(m_Acc.m_Force, m_Acc.m_Force);
			if (forceSquared < kEps) {
				Scale(m_StateT0.m_Velocity, m_LinearVelocityDamp);
			}
		}

		//! @todo put body to sleep, if it is moving at less than v = sqrt(2*g*eps), the velocity
		// of an object initially at rest would have falling through the collision envelope [Mirtich95]
		// and if the body was in contact with an immobile object in the last frame
	}

	if (m_Spinnable) {
		Real aDamp = -m_Mass * m_AngularVelocityDamp;
		SetScaled(m_Acc.m_Torque, aDamp, m_StateT0.m_AngularVelocity);	// input a torque opposing the angular velocity
	}

	m_Collided = false;

	return fellAsleep;
}

// this is called before springs are calculated
void RigidBody::Integrate1(Real dt, Vec3f gravity)
{
	// if the body's position can change:
	if (m_Translatable) {
		MultiplyAccumulate(m_StateT1.m_Velocity, dt * kHalf,		m_AccelPrev);					// v += 1/2 a * t
		MultiplyAccumulate(m_StateT1.m_Position, dt,				m_StateT1.m_Velocity);			// pos += v * dt
		MultiplyAccumulate(m_StateT1.m_Position, kHalf * dt * dt,	m_AccelPrev);					// pos += 1/2 a * t * t
	}

	// if the body can spin:			
	if (m_Spinnable) {		
		MultiplyAccumulate(m_StateT1.m_AngularMomentum, dt * kHalf, m_TorquePrev);					// L += 1/2 T * t
																									// update angular velocity from momentum
		if (m_InertialKind == Physics::kI_Sphere) {													// spheres are common enough to treat specially
			SetScaled(m_StateT1.m_AngularVelocity, m_InertiaITD[0], m_StateT1.m_AngularMomentum);
		}
		else {	// must use the inertia tensor diagonal instead
			// note, since the tensor is diagonal, world space and local space tensor is the same, so this works.
			Multiply(m_StateT1.m_AngularVelocity, m_InertiaITD, m_StateT1.m_AngularMomentum);
		}
		//else { 
		/// @todo if using full matrix for tensor, must do worldTensor = orientation * inverseinertiatensor * transpose(orientation) before multiplying
		//} 

		InputAngularVelocity(m_StateT1.m_Orientation, dt, m_StateT1.m_AngularVelocity);				// R += t * angular velocity
	}
}

// this is called after springs are calculated
void RigidBody::Integrate2(Real dt, Vec3f gravity)
{
	// if the body's position can change:
	if (m_Translatable) {
		SetScaled(m_AccelPrev, m_OOMass, m_Acc.m_Force);									// dvdt = f / m     works for gravity? yes:  f = mg    a = mg / m = g
		if (m_Gravity) {
			Add(m_AccelPrev, gravity);
		}

		MultiplyAccumulate(m_StateT1.m_Velocity, dt * kHalf, m_AccelPrev);					// v += 1/2 a * t
		Zero(m_Acc.m_Force);																// clear out the force accumulator
	}

	// if the body can spin:
	if (m_Spinnable) {
		Set(m_TorquePrev, m_Acc.m_Torque);
		MultiplyAccumulate(m_StateT1.m_AngularMomentum, dt * kHalf, m_Acc.m_Torque);				// L += 1/2 T * t

		Zero(m_Acc.m_Torque);																		// clear the torque accumulator
	}
}
