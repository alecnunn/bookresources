/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_PHYSICS_SPACE_H_
#define _BB_PHYSICS_SPACE_H_


#include<NameSpace.h>
#include<Defines.h>
#include<PhysicsObject.h>


DECLARE_ENGINE_NAMESPACE


class PhysicsSpace
{
   public:
      PhysicsSpace();
      virtual ~PhysicsSpace();

      void InfluenceObject(PhysicsObject *obj, scalar influencePct);

      void SetDensity(scalar density) { m_density = density; }
      scalar GetDensity() { return m_density; }

      void SetSurfaceHeight(scalar height) { m_surfaceHeight = height; }
      scalar GetSurfaceHeight() { return m_surfaceHeight; }

      void SetGravity(PhysicsVec g) { m_gravity = g; }
      PhysicsVec GetGravity() { return m_gravity; }

      void SetIsAirAtmosphere(bool val) { m_isAir = val; }
      bool isAirAtmosphere() { return m_isAir; }


   private:
      PhysicsVec CalDrag(PhysicsVec dir, scalar V, scalar A, scalar CW);
      PhysicsVec CalLift(PhysicsVec dir, PhysicsVec liftNormal,
                         scalar V, scalar A, scalar CL);
      PhysicsVec CalBuoyancy(scalar volume);
      

   protected:
      scalar m_density;
      scalar m_surfaceHeight;

      PhysicsVec m_gravity;

      bool m_isAir;
};

END_ENGINE_NAMESPACE

#endif