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
#include "PhysicsDemo.h"

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <GL/glaux.h>
#endif
#if defined(__APPLE__) && defined(__MACH__)
	#include <OpenGL/gl.h>		// Header File For The OpenGL32 Library
	#include <OpenGL/glu.h>	// Header File For The GLu32 Library
#else
	#include <GL/gl.h>		// Header File For The OpenGL32 Library
	#include <GL/glu.h>	// Header File For The GLu32 Library
#endif




using namespace PMath;

PhysicsDemo::~PhysicsDemo()
{
	m_Phys.RemoveAll();
}

// note, the run a proper executive toy, multiple simultaneous collisions would have to be
// handled by the collision engine. as it is, the sim devolves quickly due to the ultra-simplistic
// collision handling scheme.

void PhysicsDemo::ExecutiveToyReset()
{
	m_Phys.SetMinTimeStep(1.0f/120.0f);

	Vec3f			spherePos = {-5.0f, k0, k2};					// start 2 meters up
	Vec3f			origin	= {k0, k0, k0};
	Vec3f			up		= {k0, k0, k1};
	PMath::Plane	ground(origin, up);
	m_GroundID =	m_Phys.AddRigidBodyPlane(ground);			// ground plane corresponding to XY plane
					m_Phys.SetRigidBodyBool(m_GroundID,			Physics::Engine::propCollidable, true);

	#define EXBALLS 5				
	int i;

	// the swinging balls
	for (i = 0; i < EXBALLS; ++i) {
		m_Sphere[i] = m_Phys.AddRigidBodySphere(0.1f);

		m_Phys.SetRigidBodyVec3f(m_Sphere[i],			Physics::Engine::propPosition,		spherePos);
		m_Phys.SetRigidBodyScalar(m_Sphere[i],			Physics::Engine::propMass,			Real(0.25f));
		m_Phys.SetRigidBodyBool(m_Sphere[i],			Physics::Engine::propTranslatable,	true);
		m_Phys.SetRigidBodyBool(m_Sphere[i],			Physics::Engine::propCollidable,	true);
		spherePos[0] += 0.5f;
	}

	// the anchor balls
	spherePos[0] = -5.0f;
	spherePos[2] = 8.0f;
	for (i = EXBALLS; i < EXBALLS + EXBALLS; ++i) {
		m_Sphere[i] = m_Phys.AddRigidBodySphere(0.5f);

		m_Phys.SetRigidBodyVec3f(m_Sphere[i],			Physics::Engine::propPosition,		spherePos);
		m_Phys.SetRigidBodyScalar(m_Sphere[i],			Physics::Engine::propMass,			Real(1.0f));
		m_Phys.SetRigidBodyBool(m_Sphere[i],			Physics::Engine::propTranslatable,	false);
		m_Phys.SetRigidBodyBool(m_Sphere[i],			Physics::Engine::propCollidable,	false);
		spherePos[0] += 0.5f;
	}

	// the links
	for (i = 0; i < EXBALLS; ++i) {
		m_Spring[i] = m_Phys.AddSpring();
		m_Phys.SetSpringUInt32(m_Spring[i],			Physics::Engine::propBodyA,				m_Sphere[i]);
		m_Phys.SetSpringUInt32(m_Spring[i],			Physics::Engine::propBodyB,				m_Sphere[i+EXBALLS]);

		Vec3f* pPosA = m_Phys.GetRigidBodyVec3fPtr(m_Sphere[i], Physics::Engine::propPosition);
		Vec3f* pPosB = m_Phys.GetRigidBodyVec3fPtr(m_Sphere[i+EXBALLS], Physics::Engine::propPosition);
		Real length = Distance(*pPosA, *pPosB);

		m_Phys.SetSpringBool(m_Spring[i],			Physics::Engine::propSpringCompressible, true);
		m_Phys.SetSpringScalar(m_Spring[i],			Physics::Engine::propSpringRestLength,	length);
		m_Phys.SetSpringScalar(m_Spring[i],			Physics::Engine::propSpringStiffness,	Real(100));		// make the spring too stiff, and the system will explode. 200 is ok
		m_Phys.SetSpringScalar(m_Spring[i],			Physics::Engine::propSpringDamping,		Real(0.5));		// 0 means no damping, 1 means critical damping
	}

	// pull the first sphere back a bit
	spherePos[0] = -11.0f;
	spherePos[2] = 8.0f;
	m_Phys.SetRigidBodyVec3f(m_Sphere[0],			Physics::Engine::propPosition,		spherePos);
}

void PhysicsDemo::Reset()
{
	m_Phys.SetMinTimeStep(1.0f/50.0f);
	m_Phys.RemoveAll();
	Vec3f			gravity = {k0, k0, Real(-9.8f)};
					m_Phys.SetGravity(gravity);

	int demo = m_Demo;

	if (demo == 6) {
		ExecutiveToyReset();
		return;
	}

	Vec3f* pPosA;
	Vec3f* pPosB;
	Real length;

	Vec3f			origin	= {k0, k0, k0};
	Vec3f			up		= {k0, k0, k1};
	PMath::Plane	ground(origin, up);
	m_GroundID =	m_Phys.AddRigidBodyPlane(ground);			// ground plane corresponding to XY plane
					m_Phys.SetRigidBodyBool(m_GroundID,			Physics::Engine::propCollidable, true);

	m_Sphere[0] =	m_Phys.AddRigidBodySphere(kHalf);			// one meter diameter sphere
	Vec3f			spherePos = {k0, k0, k10};					// start 10 meters up
					m_Phys.SetRigidBodyVec3f(m_Sphere[0],		Physics::Engine::propPosition,		spherePos);
					m_Phys.SetRigidBodyBool(m_Sphere[0],		Physics::Engine::propTranslatable,	demo != 0);
					m_Phys.SetRigidBodyBool(m_Sphere[0],		Physics::Engine::propCollidable,	true);

					Vec3f			sphereSize = { 1.75f, 1.75f, 1.75f };
	m_Sphere[1] =	m_Phys.AddRigidBodySphere(kHalf);			// one meter diameter sphere
					spherePos[0] += 0.25f;
					spherePos[1] += 0.1f;
					spherePos[2] += k4;
					m_Phys.SetRigidBodyVec3f(m_Sphere[1],		Physics::Engine::propPosition,		spherePos);
					m_Phys.SetRigidBodyScalar(m_Sphere[1],		Physics::Engine::propMass,			Real(2.1f));
					m_Phys.SetRigidBodyVec3f(m_Sphere[1],		Physics::Engine::propDimensions,	sphereSize);
					m_Phys.SetRigidBodyBool(m_Sphere[1],		Physics::Engine::propTranslatable,	true);
					m_Phys.SetRigidBodyBool(m_Sphere[1],		Physics::Engine::propCollidable,	true);

	m_Sphere[2] =	m_Phys.AddRigidBodySphere(kHalf);			// one meter diameter sphere
					spherePos[0] += 0.25f;
					spherePos[1] += k4;
					spherePos[2] += k1;
					m_Phys.SetRigidBodyVec3f(m_Sphere[2],		Physics::Engine::propPosition,		spherePos);
					m_Phys.SetRigidBodyBool(m_Sphere[2],		Physics::Engine::propTranslatable,	true);
					m_Phys.SetRigidBodyBool(m_Sphere[2],		Physics::Engine::propCollidable,	true);

	m_Sphere[3] =	m_Phys.AddRigidBodySphere(kHalf);			// one meter diameter sphere
					spherePos[0] += k2;
					spherePos[1] -= k2;
					spherePos[2] -= k2;
					m_Phys.SetRigidBodyVec3f(m_Sphere[3],		Physics::Engine::propPosition,		spherePos);
					m_Phys.SetRigidBodyBool(m_Sphere[3],		Physics::Engine::propTranslatable,	true);
					m_Phys.SetRigidBodyBool(m_Sphere[3],		Physics::Engine::propCollidable,	true);


	if (demo == 0) {
		int i;
		pPosA = m_Phys.GetRigidBodyVec3fPtr(m_Sphere[0],	Physics::Engine::propPosition);
		pPosB = m_Phys.GetRigidBodyVec3fPtr(m_Sphere[3],	Physics::Engine::propPosition);
		sphereSize[0] = 0.5f; sphereSize[1] = 0.2f; sphereSize[2] = 0.2f;

		for (i = 0; i < 10; ++i) {
			Lerp(spherePos, ((Real) i+6) * (1.0f/20.0f), *pPosA, *pPosB);

			m_Sphere[4 + i] = m_Phys.AddRigidBodySphere(0.2f);

			m_Phys.SetRigidBodyVec3f(m_Sphere[4 + i],			Physics::Engine::propPosition,		spherePos);
			m_Phys.SetRigidBodyScalar(m_Sphere[4 + i],			Physics::Engine::propMass,			Real(0.2f));
			m_Phys.SetRigidBodyVec3f(m_Sphere[4 + i],			Physics::Engine::propDimensions,	sphereSize);
			m_Phys.SetRigidBodyBool(m_Sphere[4 + i],			Physics::Engine::propTranslatable,	true);
			m_Phys.SetRigidBodyBool(m_Sphere[4 + i],			Physics::Engine::propCollidable,	false);
		}

		for (i = 0; i < 9; ++i) {
			m_Spring[i] = m_Phys.AddSpring();
			m_Phys.SetSpringUInt32(m_Spring[i],			Physics::Engine::propBodyA,				m_Sphere[4+i]);
			m_Phys.SetSpringUInt32(m_Spring[i],			Physics::Engine::propBodyB,				m_Sphere[4+i+1]);

			m_Phys.SetSpringBool(m_Spring[i],			Physics::Engine::propSpringCompressible, false);
			m_Phys.SetSpringScalar(m_Spring[i],			Physics::Engine::propSpringRestLength,	Real(0.35f));
			m_Phys.SetSpringScalar(m_Spring[i],			Physics::Engine::propSpringStiffness,	Real(100));		// make the spring too stiff, and the system will explode. 200 is ok
			m_Phys.SetSpringScalar(m_Spring[i],			Physics::Engine::propSpringDamping,		Real(0.1));		// 0 means no damping, 1 means critical damping
		}

		m_Spring[9] = m_Phys.AddSpring();
		m_Phys.SetSpringUInt32(m_Spring[9],				Physics::Engine::propBodyA,				m_Sphere[0]);
		m_Phys.SetSpringUInt32(m_Spring[9],				Physics::Engine::propBodyB,				m_Sphere[13]);

		pPosA = m_Phys.GetRigidBodyVec3fPtr(m_Sphere[0], Physics::Engine::propPosition);
		pPosB = m_Phys.GetRigidBodyVec3fPtr(m_Sphere[13], Physics::Engine::propPosition);
		length = Distance(*pPosA, *pPosB);

		m_Phys.SetSpringScalar(m_Spring[9],				Physics::Engine::propSpringRestLength,	length);
		m_Phys.SetSpringScalar(m_Spring[9],				Physics::Engine::propSpringStiffness,	Real(100));		// make the spring too stiff, and the system will explode. 200 is ok
		m_Phys.SetSpringScalar(m_Spring[9],				Physics::Engine::propSpringDamping,		Real(0.5));		// 0 means no damping, 1 means critical damping

		m_Spring[10] = m_Phys.AddSpring();
		m_Phys.SetSpringUInt32(m_Spring[10],			Physics::Engine::propBodyA,				m_Sphere[4]);
		m_Phys.SetSpringUInt32(m_Spring[10],			Physics::Engine::propBodyB,				m_Sphere[3]);

		pPosA = m_Phys.GetRigidBodyVec3fPtr(m_Sphere[4], Physics::Engine::propPosition);
		pPosB = m_Phys.GetRigidBodyVec3fPtr(m_Sphere[3], Physics::Engine::propPosition);
		length = Distance(*pPosA, *pPosB);

		m_Phys.SetSpringScalar(m_Spring[10],			Physics::Engine::propSpringRestLength,	length);
		m_Phys.SetSpringScalar(m_Spring[10],			Physics::Engine::propSpringStiffness,	Real(100));		// make the spring too stiff, and the system will explode. 200 is ok
		m_Phys.SetSpringScalar(m_Spring[10],			Physics::Engine::propSpringDamping,		Real(0.5));		// 0 means no damping, 1 means critical damping

	}

	else {
		if (--demo > 0) {
			m_Spring[1]	=	m_Phys.AddSpring();
							m_Phys.SetSpringUInt32(m_Spring[1],			Physics::Engine::propBodyA,				m_Sphere[0]);
							m_Phys.SetSpringUInt32(m_Spring[1],			Physics::Engine::propBodyB,				m_Sphere[1]);

							pPosA = m_Phys.GetRigidBodyVec3fPtr(m_Sphere[0], Physics::Engine::propPosition);
							pPosB = m_Phys.GetRigidBodyVec3fPtr(m_Sphere[1], Physics::Engine::propPosition);
							length = Distance(*pPosA, *pPosB);

							// spring integration is stable only if the Courant condition is met, see Kacic et al, 2003 for an explanation
							// Kacic-Alesic, Zoran, Marcus Nordenstam, David Bullock, "A Practical Dynamics System",
							// Eurographics/SIGGRAPH Symposium on Computer Animation (2003)

							m_Phys.SetSpringScalar(m_Spring[1],			Physics::Engine::propSpringRestLength,	length);
							m_Phys.SetSpringScalar(m_Spring[1],			Physics::Engine::propSpringStiffness,	Real(200));		// make the spring too stiff, and the system will explode. 200 is ok
							m_Phys.SetSpringScalar(m_Spring[1],			Physics::Engine::propSpringDamping,		Real(1.0));		// 0 means no damping, 1 means critical damping

		}

		if (--demo > 0) {
			m_Spring[2]	=	m_Phys.AddSpring();
							m_Phys.SetSpringUInt32(m_Spring[2],			Physics::Engine::propBodyA,				m_Sphere[0]);
							m_Phys.SetSpringUInt32(m_Spring[2],			Physics::Engine::propBodyB,				m_Sphere[2]);

							pPosA = m_Phys.GetRigidBodyVec3fPtr(m_Sphere[0], Physics::Engine::propPosition);
							pPosB = m_Phys.GetRigidBodyVec3fPtr(m_Sphere[2], Physics::Engine::propPosition);
							length = Distance(*pPosA, *pPosB);

							m_Phys.SetSpringScalar(m_Spring[2],			Physics::Engine::propSpringRestLength,	length);
							m_Phys.SetSpringScalar(m_Spring[2],			Physics::Engine::propSpringStiffness,	Real(200));		// make the spring too stiff, and the system will explode. 200 is ok
							m_Phys.SetSpringScalar(m_Spring[2],			Physics::Engine::propSpringDamping,		Real(1.0));		// 0 means no damping, 1 means critical damping
		}

		if (--demo > 0) {
			m_Spring[3]	=	m_Phys.AddSpring();
							m_Phys.SetSpringUInt32(m_Spring[3],			Physics::Engine::propBodyA,				m_Sphere[1]);
							m_Phys.SetSpringUInt32(m_Spring[3],			Physics::Engine::propBodyB,				m_Sphere[2]);

							pPosA = m_Phys.GetRigidBodyVec3fPtr(m_Sphere[1], Physics::Engine::propPosition);
							pPosB = m_Phys.GetRigidBodyVec3fPtr(m_Sphere[2], Physics::Engine::propPosition);
							length = Distance(*pPosA, *pPosB);

							m_Phys.SetSpringScalar(m_Spring[3],			Physics::Engine::propSpringRestLength,	length);
							m_Phys.SetSpringScalar(m_Spring[3],			Physics::Engine::propSpringStiffness,	Real(200));		// make the spring too stiff, and the system will explode. 200 is ok
							m_Phys.SetSpringScalar(m_Spring[3],			Physics::Engine::propSpringDamping,		Real(1.0));		// 0 means no damping, 1 means critical damping
		}

		if (--demo > 0) {
			m_Spring[4]	=	m_Phys.AddSpring();
							m_Phys.SetSpringUInt32(m_Spring[4],			Physics::Engine::propBodyA,				m_Sphere[0]);
							m_Phys.SetSpringUInt32(m_Spring[4],			Physics::Engine::propBodyB,				m_Sphere[3]);

							pPosA = m_Phys.GetRigidBodyVec3fPtr(m_Sphere[0], Physics::Engine::propPosition);
							pPosB = m_Phys.GetRigidBodyVec3fPtr(m_Sphere[3], Physics::Engine::propPosition);
							length = Distance(*pPosA, *pPosB);

							m_Phys.SetSpringScalar(m_Spring[4],			Physics::Engine::propSpringRestLength,	length);
							m_Phys.SetSpringScalar(m_Spring[4],			Physics::Engine::propSpringStiffness,	Real(200));		// make the spring too stiff, and the system will explode. 200 is ok
							m_Phys.SetSpringScalar(m_Spring[4],			Physics::Engine::propSpringDamping,		Real(1.0));		// 0 means no damping, 1 means critical damping

			m_Spring[5] =	m_Phys.AddSpring();
							m_Phys.SetSpringUInt32(m_Spring[5],			Physics::Engine::propBodyA,				m_Sphere[1]);
							m_Phys.SetSpringUInt32(m_Spring[5],			Physics::Engine::propBodyB,				m_Sphere[3]);

							pPosA = m_Phys.GetRigidBodyVec3fPtr(m_Sphere[1], Physics::Engine::propPosition);
							pPosB = m_Phys.GetRigidBodyVec3fPtr(m_Sphere[3], Physics::Engine::propPosition);
							length = Distance(*pPosA, *pPosB);

							m_Phys.SetSpringScalar(m_Spring[5],			Physics::Engine::propSpringRestLength,	length);
							m_Phys.SetSpringScalar(m_Spring[5],			Physics::Engine::propSpringStiffness,	Real(200));		// make the spring too stiff, and the system will explode. 200 is ok
							m_Phys.SetSpringScalar(m_Spring[5],			Physics::Engine::propSpringDamping,		Real(1.0));		// 0 means no damping, 1 means critical damping

			m_Spring[6]	=	m_Phys.AddSpring();
							m_Phys.SetSpringUInt32(m_Spring[6],			Physics::Engine::propBodyA,				m_Sphere[2]);
							m_Phys.SetSpringUInt32(m_Spring[6],			Physics::Engine::propBodyB,				m_Sphere[3]);

							pPosA = m_Phys.GetRigidBodyVec3fPtr(m_Sphere[2], Physics::Engine::propPosition);
							pPosB = m_Phys.GetRigidBodyVec3fPtr(m_Sphere[3], Physics::Engine::propPosition);
							length = Distance(*pPosA, *pPosB);

							m_Phys.SetSpringScalar(m_Spring[6],			Physics::Engine::propSpringRestLength,	length);
							m_Phys.SetSpringScalar(m_Spring[6],			Physics::Engine::propSpringStiffness,	Real(200));		// make the spring too stiff, and the system will explode. 200 is ok
							m_Phys.SetSpringScalar(m_Spring[6],			Physics::Engine::propSpringDamping,		Real(1.0));		// 0 means no damping, 1 means critical damping
		}
	}
}

void PhysicsDemo::Update(Real dt)
{
	m_Phys.Simulate(dt);

	int width, height;
	GetWindowSize(width, height);

    glViewport(0, 0, width, height);// resets the viewport to new dimensions.
    glMatrixMode(GL_PROJECTION);    // Sets the projection matrix.
    glLoadIdentity();               // Reset the modelview matrix.

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

    // calculate the aspect ratio of the window.
    gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 1.0f, 1000.0f);

	gluLookAt(0,-20,5, 0,0,3.5, 0,0,1);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	m_Phys.Render();
}


bool PhysicsDemo::HandleKey(int key)
{
	/*
		This method of switching demos is really clumsy, please study the physics, not the demo shell :-)
	*/

	switch (key) {
		case '0': m_Demo = 0; break;
		case '1': m_Demo = 1; break;
		case '2': m_Demo = 2; break;
		case '3': m_Demo = 3; break;
		case '4': m_Demo = 4; break;
		case '5': m_Demo = 5; break;
		case '6': m_Demo = 6; break;

		default:
			++m_Demo;
			if (m_Demo > 6)
				m_Demo = 0;
			break;
	}

	Reset();
	return false;
}

void PhysicsDemo::MouseMotion(int x, int y)
{
}

void PhysicsDemo::MouseClick(eMouseButton button)
{
}

void PhysicsDemo::MouseUnclick(eMouseButton button)
{
}
