/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<PointMass.h>


DECLARE_ENGINE_NAMESPACE


void PointMass::Update(float dt)
{
   m_oldPosition = m_position;

   // The a = F * (1 / m) is the same as a = F / m.
   m_acceleration = m_force * m_massInv;

   m_velocity += m_acceleration * dt;
   m_velocity = m_velocity * m_velocityDamper;
   m_position += m_velocity * dt;

   m_speed = m_velocity.Magnitude();

   m_force = Vector3D(0, 0, 0);
}


Matrix4x4 PointMass::GetTransformation()
{
   Matrix4x4 matrix;
   matrix.Translate(m_position);

   return matrix;
}


END_ENGINE_NAMESPACE