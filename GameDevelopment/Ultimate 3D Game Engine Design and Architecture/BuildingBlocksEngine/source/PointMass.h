/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_POINT_MASS_
#define _BB_POINT_MASS_


#include<NameSpace.h>
#include<Defines.h>
#include<PhysicsObject.h>


DECLARE_ENGINE_NAMESPACE


class PointMass : public PhysicsObject
{
   public:
      PointMass()
      {
         m_velocityDamper = 1;
      }

      virtual ~PointMass()
      {

      }

      virtual void Update(float dt);

      virtual scalar GetDragArea() { return 0; }
      virtual scalar GetDragCoefficient() { return (scalar)0.05; }
      virtual scalar GetLiftCoefficient() { return 0; }

      virtual scalar GetVolumeUnderHeight(scalar height)
      {
         if(m_position.y >= height)
            return 0;

         return 1;
      }

      virtual Matrix4x4 GetTransformation();

      void SetVelocityDamper(float damper)
      {
         m_velocityDamper = damper;
      }


   protected:
      float m_velocityDamper;
};

END_ENGINE_NAMESPACE

#endif