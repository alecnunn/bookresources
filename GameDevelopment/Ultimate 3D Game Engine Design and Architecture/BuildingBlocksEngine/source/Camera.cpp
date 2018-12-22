/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<Camera.h>
#include<Quaternion.h>


DECLARE_ENGINE_NAMESPACE


Camera::Camera()
{
   m_lookAt.z = -1;
   m_up.y = 1;
   m_right.x = 1;
}


Camera::Camera(Vector3D &pos, Vector3D &lookAt,
               Vector3D &up, Vector3D &right)
{
   m_pos = pos;
   m_lookAt = lookAt;
   m_up = up;
   m_right = right;
}


void Camera::MoveCamera(Vector3D &direction, float speed)
{
	m_pos += direction * speed;
	m_lookAt += direction * speed;
}


void Camera::StrafeCam(float speed)
{
   MoveCamera(m_right, speed);
}


void Camera::RotateCamera(float angle, Vector3D &axis)
{
	Quaternion qRotation, qView, qNewView;

   qRotation.RotationAxisToQuaternion(angle, axis);

   qView.x = m_lookAt.x - m_pos.x;
   qView.y = m_lookAt.y - m_pos.y;
   qView.z = m_lookAt.z - m_pos.z;
   qView.w = 0;

   qNewView = ((qRotation * qView) * qRotation.Conjugate());

   m_right.x = m_pos.x + qNewView.x;
   m_right.y = m_pos.y + qNewView.y;
   m_right.z = m_pos.z + qNewView.z;

   m_lookAt.x = m_pos.x + qNewView.x;
   m_lookAt.y = m_pos.y + qNewView.y;
   m_lookAt.z = m_pos.z + qNewView.z;
}


void Camera::RotateCamera(float deltaX, float deltaY)
{
   Vector3D axis, dir, yAxis(0, 1, 0);

   dir = m_lookAt - m_pos;
   axis = dir.CrossProduct(m_up);
   axis.Normalize();

   RotateCamera(deltaY, axis);
   RotateCamera(deltaX, yAxis);
}


END_ENGINE_NAMESPACE