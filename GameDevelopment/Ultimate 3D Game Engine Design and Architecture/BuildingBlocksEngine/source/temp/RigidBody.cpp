/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<RigidBody.h>


DECLARE_ENGINE_NAMESPACE


RigidBody::RigidBody()
{

}


RigidBody::~RigidBody()
{

}


void RigidBody::ResetObject()
{

}


void RigidBody::Update(float dt)
{

}


void RigidBody::SetMass(float m)
{
   m_recalInertiaTensor = true;
   PhysicsObject::SetMass(m);
}


void RigidBody::AddForce(Vector3D &force, Vector3D &incidence, BB_DISTRUBUTION dis)
{

}


void RigidBody::ApplyImpulse(Vector3D &linear, Vector3D &angular)
{

}

Vector3D RigidBody::GetAngularForce()
{
   return Vector3D();
}


void RigidBody::SetAngularForce(Vector3D &f, Vector3D &incidence)
{

}


Vector3D RigidBody::GetAngularVelocity()
{
   return Vector3D();
}


void RigidBody::SetOrientation(Quaternion &q)
{
   //m_orientation = q;
   //m_worldMat = m_orientation.MatrixRH();
}


void RigidBody::SetOrientation(Matrix4x4 &m)
{
   //m_orientation = Quaternion(m);
   //m_worldMat = m;
}


void RigidBody::ApplyCollisionImpulse(Vector3D &relVel,
                                      Vector3D &relAng,
                                      float restitution)
{

}


void RigidBody::ApplyCollisionImpulse(RigidBody &rb, Vector3D &relVel,
                                      Vector3D &relAng, Vector3D &rbRelAng)
{

}


Matrix4x4 RigidBody::GetInverseWorldTensor()
{
   return Matrix4x4();
}


END_ENGINE_NAMESPACE