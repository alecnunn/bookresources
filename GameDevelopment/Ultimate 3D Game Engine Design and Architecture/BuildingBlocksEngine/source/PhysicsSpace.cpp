/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<PhysicsSpace.h>


DECLARE_ENGINE_NAMESPACE


PhysicsSpace::PhysicsSpace()
{
   m_density = 0;
   m_surfaceHeight = 0;
   m_isAir = false;
}


PhysicsSpace::~PhysicsSpace()
{

}


void PhysicsSpace::InfluenceObject(PhysicsObject *obj, scalar influencePct)
{
   if(obj == 0)
      return;

   Vector3D velocity, totalForces(0, 0, 0);
   scalar speed = obj->GetSpeed();

   if(speed != 0.0f)
      velocity = obj->GetVelocity() / speed;

   if(m_isAir != true)
      totalForces += CalBuoyancy(obj->GetVolume());

   totalForces += m_gravity;

   if(speed != 0.0f )
      {
         totalForces += CalDrag(velocity, speed,
                                obj->GetDragArea(),
                                obj->GetDragCoefficient());
      }

   if(obj->GetLift() > 0.0f)
      {
         totalForces += CalLift(velocity, obj->GetLiftVec(),
                                speed, obj->GetLift(),
                                obj->GetLiftCoefficient());
      }

   obj->AddForce(totalForces * influencePct);
}


PhysicsVec PhysicsSpace::CalDrag(PhysicsVec dir, scalar V, scalar A, scalar CW)
{
   PhysicsVec drag;

   if(V != 0)
      {
         scalar d = (scalar)0.5 * m_density * (V * V) * A * CW;
         drag = (dir * -1) * d;
      }

   return drag;
}


PhysicsVec PhysicsSpace::CalLift(PhysicsVec dir, PhysicsVec liftNormal,
                                 scalar V, scalar A, scalar CL)
{
   PhysicsVec drag, lift;

   if(V != 0)
      {
         PhysicsVec cross;

         drag = dir * -1;

         cross = drag.CrossProduct(liftNormal);
         lift = cross.CrossProduct(drag);
         lift.Normalize();

         scalar l = (V * V) * ((scalar)0.5 * m_density) * A * CL;
         lift = lift * l;
      }

   return lift;
}


PhysicsVec PhysicsSpace::CalBuoyancy(scalar volume)
{
   if(m_isAir ==  true)
      return PhysicsVec();

   return m_gravity * (m_density * volume * -1);
}


END_ENGINE_NAMESPACE