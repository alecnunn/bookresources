
/** @file CollisionEngine.h
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

#ifndef _COLLISIONENGINE_H_
#define _COLLISIONENGINE_H_

#include <vector>
#include <list>

#include "PhysicsEngineDef.h"
#include "PMath.h"

/** @namespace CollisionEngine
 *	Collision Engine namespace
 */

namespace Collision {

	/// EngineAux tracks hidden implementation details, such as the Contact free pool
	class EngineAux;
	class Contact;

	/** @class Engine
		Manages collision
	 */

	class Engine
	{
	public:
		Engine();
		~Engine();

		/// set the number of contacts that can be managed by the physics engine
		void SetCapacity(int maxContacts);

		/// call to indicate beginning of collision phase
		void Begin();	

		/// first the physics engine has to submit all pairs of bodies for testing for collision
		Contact* TestCollision(Physics::RigidBody* pBodyA, Physics::RigidBody* pBodyB);		// returns a Contact if in contact, 0 otherwise

		/// clear a contact
		void Clear(Contact*);

		/// resolve an existing contact
		void Resolve(Contact*);

		/// call to indicate end of collision phase
		void End();		

		/// between a Begin and End call, this vector contains all the detected contacts
		std::vector<Contact*>	m_Contacts;
		std::list<Contact*>		m_FreePool;
	};

} // namespace Collision

#endif
