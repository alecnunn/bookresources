/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_CAMERA_H_
#define _BB_CAMERA_H_

#include<NameSpace.h>
#include<Vector3D.h>


DECLARE_ENGINE_NAMESPACE


class Camera
{
   public:
      Camera();

      Camera(Vector3D &pos, Vector3D &lookAt,
             Vector3D &up, Vector3D &right);


      void MoveCamera(Vector3D &direction, float speed);
      void StrafeCam(float speed);

      void RotateCamera(float angle, Vector3D &axis);
      void RotateCamera(float deltaX, float deltaY);


      void SetPosition(Vector3D &pos) { m_pos = pos; }
      void SetLookDirection(Vector3D &at) { m_lookAt = at; }
      void SetUpDirection(Vector3D &up) { m_up = up; }
      void SetRightDirection(Vector3D &right) { m_right = right; }

      Vector3D GetPosition() { return m_pos; }
      Vector3D GetLookDirection() { return m_lookAt; }
      Vector3D GetUpDirection() { return m_up; }
      Vector3D GetRightDirection() { return m_right; }


   private:
      Vector3D m_pos, m_lookAt, m_up, m_right;
};


END_ENGINE_NAMESPACE

#endif