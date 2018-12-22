
#ifndef __PHYSICSDEMO_H_
#define __PHYSICSDEMO_H_
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

#include "pmath.h"
#include "application.h"
#include "PhysicsEngine.h"

/** @class PhysicsDemo
	@brief	This class implements a demo for the physics engine
	*/

class PhysicsDemo : public VOpenGLMain {
public:

#define kMaxSprings 100
#define kMaxSpheres 100

	PhysicsDemo() : m_Mousex(0.0f), m_Mousey(0.0f), VOpenGLMain(),
	m_Demo(0)
	{
	}

	virtual ~PhysicsDemo();

	void Reset();
	void ExecutiveToyReset();
	void Update(Real dt);

	bool HandleKey(int key);
	void MouseMotion(int x, int y);
	void MouseClick(eMouseButton button);
	void MouseUnclick(eMouseButton button);

protected:
	int				m_Demo;
	Physics::Engine	m_Phys;
	float			m_Mousex, m_Mousey;
	uint32			m_GroundID;
	uint32			m_Sphere[kMaxSpheres];
	uint32			m_Spring[kMaxSprings];
};

#endif
