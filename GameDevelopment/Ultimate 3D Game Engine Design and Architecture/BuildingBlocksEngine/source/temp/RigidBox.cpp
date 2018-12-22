/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<RigidBox.h>


DECLARE_ENGINE_NAMESPACE


RigidBox::RigidBox()
{
   m_width = 0.0f;
   m_height = 0.0f;
   m_depth = 0.0f;

   m_obb.SetAxis1(Vector3D(1.0f, 0.0f, 0.0f));
   m_obb.SetAxis2(Vector3D(0.0f, 1.0f, 0.0f));
   m_obb.SetAxis3(Vector3D(0.0f, 0.0f, 1.0f));
   m_obb.SetCenter(Vector3D(0.0f, 0.0f, 0.0f));
}


RigidBox::RigidBox(float width, float height, float depth)
{
   m_width = width;
   m_height = height;
   m_depth = depth;

   m_obb.SetAxis1(Vector3D(1.0f, 0.0f, 0.0f));
   m_obb.SetAxis2(Vector3D(0.0f, 1.0f, 0.0f));
   m_obb.SetAxis3(Vector3D(0.0f, 0.0f, 1.0f));
   m_obb.SetCenter(Vector3D(0.0f, 0.0f, 0.0f));
}


RigidBox::~RigidBox()
{

}


void RigidBox::SetDimensions(float width, float height, float depth)
{
   m_width = width;
   m_height = height;
   m_depth = depth;

   m_volume = m_width * m_height * m_depth;

   Vector3D half(width * 0.5f, height * 0.5f, depth * 0.5f);
   m_radiusSqr = half.Magnitude();

   m_obb.SetHalfAxis1(width * 0.5f);
   m_obb.SetHalfAxis2(height * 0.5f);
   m_obb.SetHalfAxis3(depth * 0.5f);

   m_recalInertiaTensor = true;
}


OBB RigidBox::GetOBBWorldSpace()
{
   OBB obb;
   obb.ObjectTransform(m_obb, m_worldMat);
   return obb;
}

scalar RigidBox::GetDragArea()
{
   //
   return 0;
}


scalar RigidBox::GetDragCoefficient()
{
   return 0.95f;
}


scalar RigidBox::GetLiftCoefficient()
{
   return 0;
}


scalar RigidBox::GetVolumeUnderHeight(float height)
{
   //
   return 0;
}


void RigidBox::CalcInertiaTensor()
{
   scalar w2 = m_width * m_width;
	scalar h2 = m_height * m_height;
	scalar d2 = m_depth * m_depth;

	m_inertiaTensor.matrix[0] =  m_mass * (h2 + d2);
	m_inertiaTensor.matrix[5]  = m_mass * (w2 + d2);
	m_inertiaTensor.matrix[10] = m_mass * (w2 + h2);

	m_inertiaTensorInv.matrix[0] = 1 / m_inertiaTensor.matrix[0];
	m_inertiaTensorInv.matrix[5] = 1 / m_inertiaTensor.matrix[5];
	m_inertiaTensorInv.matrix[10] = 1 / m_inertiaTensor.matrix[10];
}


END_ENGINE_NAMESPACE