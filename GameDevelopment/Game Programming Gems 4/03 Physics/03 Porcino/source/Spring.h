
/** @file Spring.h
 */

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

namespace Physics {

	class Engine;
	class RigidBody;

	/** @class	Spring
		@brief	A spring for implementing Hooke's law
		*/

	class Spring
	{
	public:
		Spring() 
		{ 
			m_Stiffness = k1;
			m_Damping = k1;
			m_RestLength = k1;
			m_PrevLength = k1;
			PMath::Zero(m_PosA);
			PMath::Zero(m_PosB);
			m_CenterAttachA = true;
			m_CenterAttachB = true;
			m_Compressible = true;
		}

		~Spring() { }

		bool		m_Compressible;		//!< spring pushes if compressed

		Real		m_Stiffness;		//!< k in Hooke's law
		Real		m_Damping;			//!< b in Hooke's law
		Real		m_RestLength;		//!< l in Hooke's law

		uint32			m_BodyA, m_BodyB;	//!< identifiers of body a and body b
		PMath::Vec3f	m_PosA, m_PosB;		//!< attachment points of spring, relative to bodies' centers of mass

		friend class Engine;

	private:
		// the following are derived values
		bool			m_CenterAttachA;	//!< true if attachment point is center of mass, no torque if true
		bool			m_CenterAttachB;	//!< true if attachment point is center of mass, no torque if true
		RigidBody*		m_pBodyA;			//!< cached value
		RigidBody*		m_pBodyB;			//!< cached value
		Real			m_PrevLength;		//!< length during previous timestep
	};


} // end namespace Physics
