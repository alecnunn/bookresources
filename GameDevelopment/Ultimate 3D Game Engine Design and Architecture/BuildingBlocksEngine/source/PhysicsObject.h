/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_PHYSICS_OBJECT_H_
#define _BB_PHYSICS_OBJECT_H_


#include<NameSpace.h>
#include<Defines.h>
#include<Vector3D.h>
#include<Matrix.h>


DECLARE_ENGINE_NAMESPACE


typedef Vector3D PhysicsVec;


class PhysicsObject
{
   public:
      PhysicsObject()
      {
         m_mass = 1;
         m_volume = 0;
         m_lift = 0;
         m_speed = 0;
      }

      virtual ~PhysicsObject()
      {

      }

      virtual void Update(float dt) = 0;

      virtual scalar GetDragArea() = 0;
      virtual scalar GetDragCoefficient() = 0;
      virtual scalar GetLiftCoefficient() = 0;

      virtual scalar GetVolumeUnderHeight(scalar height) = 0;

      virtual void SetMass(scalar m) { m_mass = m; m_massInv = 1 / m; }
      virtual scalar GetMass() { return m_mass; }
      virtual scalar GetMassInv() { return m_massInv; }

      virtual scalar GetVolume() { return m_volume; }

      virtual void SetLift(scalar lVal, PhysicsVec lVec) { m_lift = lVal; m_liftVector = lVec; }
      virtual scalar GetLift() { return m_lift; }
      virtual PhysicsVec GetLiftVec() { return m_liftVector; }

      virtual scalar GetSpeed() { return m_speed; }

      virtual PhysicsVec GetOldPosition() { return m_oldPosition; }

      virtual void SetPosition(PhysicsVec pos) { m_position = pos; }
      virtual PhysicsVec GetPosition() { return m_position; }

      virtual void SetForce(PhysicsVec f) { m_force = f; }
      virtual PhysicsVec GetForce() { return m_force; }
      virtual void AddForce(PhysicsVec f) { m_force += f; }

      virtual void SetVelocity(PhysicsVec v) { m_velocity = v; }
      virtual PhysicsVec GetVelocity() { return m_velocity; }
      virtual void AddVelocity(PhysicsVec v) { m_velocity += v; }

      virtual PhysicsVec GetAcceleration() { return m_acceleration; }

      virtual Matrix4x4 GetTransformation() = 0;

   protected:
      scalar m_mass;
      scalar m_massInv;
      scalar m_volume;
      scalar m_lift;
      scalar m_speed;

      PhysicsVec m_oldPosition;
      PhysicsVec m_position;

      PhysicsVec m_force;
      PhysicsVec m_velocity;
      PhysicsVec m_acceleration;

      PhysicsVec m_liftVector;
};

END_ENGINE_NAMESPACE

#endif