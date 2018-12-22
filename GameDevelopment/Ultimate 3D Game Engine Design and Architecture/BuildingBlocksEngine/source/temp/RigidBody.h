/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_RIGID_BODY_H_
#define _BB_RIGID_BODY_H_


#include<NameSpace.h>
#include<Defines.h>
#include<PhysicsObject.h>
#include<CollisionReport.h>
#include<Vector3D.h>
#include<Matrix.h>
#include<Quaternion.h>


DECLARE_ENGINE_NAMESPACE


class RigidBody : public PhysicsObject
{
   public:
      RigidBody();
      virtual ~RigidBody();

      virtual void ResetObject();
      virtual void Update(float dt);

      virtual void SetMass(float m);

      virtual void AddForce(Vector3D &force, Vector3D &incidence,
                            BB_DISTRUBUTION dis);

      virtual void ApplyImpulse(Vector3D &linear,
                                Vector3D &angular);

      virtual Vector3D GetAngularForce();
      virtual Vector3D GetAngularVelocity();
      virtual int GetTotalAngularForces() { return m_totalForces; }

      virtual Quaternion GetOrientation() { return m_orientation; }
      virtual void SetOrientation(Quaternion &q);
      virtual void SetOrientation(Matrix4x4 &m);
    
      virtual float GetRestitution() { return m_restitution; }
      virtual void SetRestitution(float r) { m_restitution = r; }

      virtual Vector3D GetAngularMomentum()
      {
         return m_angularMomentum;
      }

      virtual void ResetAngularMomentum()
      {
         m_angularMomentum = Vector3D();
      }

      virtual void ApplyAngularMomentum(Vector3D &v)
      {
         m_angularMomentum += v;
      }

      virtual void SetAngularForce(Vector3D &f,
                                   Vector3D &incidence);

      virtual void SetAngularMomentum(Vector3D &v)
      {
         m_angularMomentum = v;
      }

      virtual Matrix4x4	GetWorldMatrix() { return m_worldMat; }

      virtual scalar GetDragArea() = 0;
      virtual scalar GetDragCoefficient() = 0;
      virtual scalar GetLiftCoefficient() = 0;
      virtual scalar GetVolumeUnderHeight(float height) = 0;

      virtual CollisionReport GetCollisionReport()
      {
         return m_report;
      }

   protected:
      virtual void ApplyCollisionImpulse(Vector3D &relVel,
                                         Vector3D &relAng,
                                         float restitution);

      virtual void ApplyCollisionImpulse(RigidBody &rb,
                                         Vector3D &relVel,
                                         Vector3D &relAng,
                                         Vector3D &rbRelAng);

      virtual Matrix4x4 GetInverseWorldTensor();
      virtual void CalcInertiaTensor() = 0;

   protected:
      Matrix4x4 m_inertiaTensor;
      Matrix4x4 m_inertiaTensorInv;
      bool m_recalInertiaTensor;

      Matrix4x4 m_worldMat;
      Quaternion m_orientation;

      Vector3D m_angularVelocity;
      Vector3D m_angularMomentum;

      Vector3D m_impuseLinearMomentum;
      Vector3D m_impuseAngMomentum;

      Vector3D m_angularForces;
      Vector3D m_incidenceTotal;
      int m_totalForces;

      float m_restitution;
      float m_radiusSqr;

      CollisionReport m_report;
      float m_collisionTime;
};


END_ENGINE_NAMESPACE


#endif