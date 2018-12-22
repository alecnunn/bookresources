
/** @file RigidBody.h

	an internal implementation file
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

#ifndef _RIGIDBODY_H_
#define _RIGIDBODY_H_

#include "GraphObj.h"
#include "PhysicsEngineDef.h"

namespace Physics {

/** @class RigidAccumulator
	An accumulator for forces and torques in a single frame
 */

class RigidAccumulator
{
public:
	RigidAccumulator()
	{
		Clear();
	}

	~RigidAccumulator()
	{
	}

	void Clear()
	{
		PMath::Zero(m_Force);
		PMath::Zero(m_Torque);
	}

	void AddForce(PMath::Vec3f a)		{ PMath::Add(m_Force, a); }
	void AddTorque(PMath::Vec3f a)	{ PMath::Add(m_Torque, a); }

	PMath::Vec3f	m_Force;
	PMath::Vec3f	m_Torque;
};

/** The state of a rigid body
 */

class DynamicState
{
public:
	DynamicState()
	{
		SetAtRest();
	}

	~DynamicState() { }

	DynamicState& operator=(const DynamicState& right)
	{	// assign _Right
		if (this != &right) {
			PMath::Set(m_Orientation,		right.m_Orientation);
			PMath::Set(m_Position,			right.m_Position);
			PMath::Set(m_Velocity,			right.m_Velocity);
			PMath::Set(m_AngularVelocity,	right.m_AngularVelocity);
			PMath::Set(m_AngularMomentum,	right.m_AngularMomentum);
		}
		return (*this);
	}

	void	SetAtRest()
	{
		m_Orientation[0] = Real(0.0f);
		m_Orientation[0] = Real(0.0f);
		m_Orientation[0] = Real(0.0f);
		m_Orientation[0] = Real(1.0f);
		PMath::Zero(m_Position);
		PMath::Zero(m_Velocity);
		PMath::Zero(m_AngularVelocity);
		PMath::Zero(m_AngularMomentum);
	}

	PMath::Quaternion	m_Orientation;
	PMath::Vec3f		m_Position;
	PMath::Vec3f		m_Velocity;
	PMath::Vec3f		m_AngularVelocity;
	PMath::Vec3f		m_AngularMomentum;
};

/** @class	RigidBody
	A rigid body interface class

	RigidBody objects are automatically active when they are first added
 */

class RigidBody
{
public:
	RigidBody();
	virtual ~RigidBody();

	virtual bool			ResetForNextTimeStep();							//!< reset body for next time step. @return true fell asleep, false = didn't fall asleep
			void			Integrate1(Real dt, PMath::Vec3f gravity);
			void			Integrate2(Real dt, PMath::Vec3f gravity);

	virtual void			SetCollisionObject(Physics::IGeometry* collide) { m_pCollideGeo = collide; }
	virtual void			SetInertialKind(EInertialKind ikind);
	inline	EInertialKind	GetInertialKind() const { return m_InertialKind; }
	virtual void			SetMass(Real mass);
	inline	Real			GetMass() const { return m_Mass; }
	inline	Real			GetOOMass() const { return m_OOMass; }
			void			CalculateInertiaTensor();

	virtual	void			Renormalize() { }
			void			SetDefaults();

	inline	void			SetGravity(bool val)		{ m_Gravity = val;			}
	inline	void			SetActive(bool val)			{ m_Active = val;			}
	inline	void			SetCollidable(bool val)		{ m_Collidable = val;		}
	inline	void			SetSpinnable(bool val)		{ m_Spinnable = val;		}
	inline	void			SetTranslatable(bool val)	{ m_Translatable = val;		}
	inline	bool			GetSpinnable()		const	{ return m_Spinnable;		}
	inline	bool			GetTranslatable()	const	{ return m_Translatable;	}
	inline	bool			GetActive()			const	{ return m_Active;			}
	inline	bool			GetGravity()		const	{ return m_Gravity;			}
	inline	bool			GetCollidable()		const	{ return m_Collidable;		}

	inline	void			SetAngularVelocityDamp(Real value)	{ m_AngularVelocityDamp = value * Real(0.995); }		// ensure there's a tiny bit of damping to compensate for our simple integrator
	inline	void			SetLinearVelocityDamp(Real value)	{ m_LinearVelocityDamp = value * Real(0.995f);	}		// ensure there's a tiny bit of damping to compensate for our simple integrator

	inline	Real			GetAngularVelocityDamp() const		{ return m_AngularVelocityDamp * Real(1.0f / 0.995f); }	// return the value the app actually input in the first place
	inline	Real			GetLinearVelocityDamp() const		{ return m_LinearVelocityDamp * Real(1.0f / 0.995f); }	// return the value the app actually input in the first place

			void			SetRender(GraphObj::Base * pObj) { m_pRender = pObj; }			//!< attach a render object, for debugging only
			void			Render();														//!< render physics body, for debugging only

	GraphObj::Base*			m_pRender;

	RigidAccumulator		m_Acc;					//!< accumulated forces and torques
	DynamicState			m_StateT0;				//!< state at the beginning of the time step
	DynamicState			m_StateT1;				//!< state at the end of the time step
	PMath::Vec3f			m_Extent;				//!< extent in each dimension from local origin

	Physics::IGeometry*		m_pCollideGeo;			//!< pointer to collision geometry
	PMath::Vec3f			m_InertiaITD;			//!< Inverse of Inertia Tensor Diagonal
	bool					m_Collided;				//!< indicates collided during the frame

protected:
	Real					m_LinearVelocityDamp;	//!< linear velocity damping can be used to control friction-like effects
	Real					m_AngularVelocityDamp;	//!< can slow a spinning object to simulate viscous drag

	PMath::Vec3f			m_AccelPrev;			//!< the acceleration applied in the previous time step
	PMath::Vec3f			m_TorquePrev;			//!< the torque applied in the previous time step

	EInertialKind			m_InertialKind;			//!< indicates the inertia calculation to use (point, sphere, diagonal tensor matrix, tensor matrix)

	bool					m_Active;				//!< true if participates in simulation
	bool					m_Spinnable;			//!< can spin
	bool					m_Translatable;			//!< can move
	bool					m_Collidable;			//!< participates in collisions
	bool					m_Gravity;				//!< affected by gravity

	Real					m_Mass;					//!< mass of the object
	Real					m_OOMass;				//!< reciprocal of the object's mass, cached to avoid lots of divisions

};

}	// end Physics namespace

#endif
