
/** @file PhysicsEngineDef.h
	@brief This file contains forward declarations
*/

#ifndef _PHYSICSENGINEDEF_H_
#define _PHYSICSENGINEDEF_H_

#include "PMath.h"


namespace Physics {
	enum	EInertialKind { kI_Immobile, kI_Box, 
							kI_Sphere, kI_Ellipsoid, kI_HollowSphere, kI_Hemisphere, kI_HemisphereBottomHeavy,
							kI_Cylinder, kI_CylinderBottomHeavy, kI_CylinderThinShell, kI_CylinderThinShellBottomHeavy,
							kI_Torus, kI_Hoop };

	class RigidBody;
	class Engine;

	enum	ECollisionKind	{ kC_Plane = 0, kC_Sphere, kC_BoundedPlane, kC_Box, kC_ConvexHull, kC_Mesh }; 

	/**	@class	ICallback
		@brief	An application receives collision callbacks via this class
	*/

	class ICallback {
	public:
		ICallback() { }
		virtual ~ICallback() { }

		/**
		* Override this function to respond to collisions as a they occur
		* 
		* @param bodyA  The first participant in the collision
		* @param bodyB  The second participant in the collision
		* @param pos    The position of the contact
		* @param normal The normal of the contact, relative to bodyA
		* @param forceA The force that was applied to bodyA to resolve the collision
		* @param forceB The force that was applied to bodyB to resolve the collision
		*
		* @return true if the collision should be resolved, false if it should be ignored
		*/
		virtual bool CollisionOccurred(uint32 bodyA, uint32 bodyB, 
			PMath::Vec3f pos, PMath::Vec3f normal, PMath::Vec3f forceA, PMath::Vec3f forceB) = 0;
	};


	/** @class ICollide
	*	Collision data baseclass
	*/
	class IGeometry
	{
	public:
		IGeometry() { }
		virtual ~IGeometry() { }

		virtual uint32 GetKind() = 0;
	};

	/** @class Plane : public ICollide
	 *  plane for collision purposes
	 */
	class Plane : public IGeometry
	{
	public:
		Plane(const PMath::Plane& plane) : m_Plane(plane) { }
		virtual ~Plane() { }
		PMath::Plane m_Plane;

		uint32 GetKind() { return kC_Plane; }
	};

	/** @class Sphere : public ICollide
	* Sphere for collision purposes
	*/
	class Sphere : public IGeometry
	{
	public:
		Sphere(const Real radius) : m_Radius(radius) { }
		virtual ~Sphere() { }
		Real m_Radius;

		uint32 GetKind() { return kC_Sphere; }
	};


}

#define APILOG printf

#endif
