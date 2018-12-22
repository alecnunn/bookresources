/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_QUATERNION_H_
#define _BB_QUATERNION_H_

#include<NameSpace.h>
#include<Vector3D.h>


DECLARE_ENGINE_NAMESPACE


class Quaternion 
{
   public:
      Quaternion();
      Quaternion(const Quaternion &q);
	   Quaternion(float X, float Y, float Z, float W);

      Quaternion operator*(const Quaternion &q);
      void operator=(const Quaternion &q);

      float Magnitude();
      void Normalize();
      Quaternion Conjugate();

      void RotationAxisToQuaternion(float angle, Vector3D &axis);
      void EulerToQuaternion(Vector3D &euler);

      Quaternion CrossProduct(const Quaternion &q);

	   void CreateMatrix(float *matrix);
      void MatrixToQuaternion(float *matrix);

      void Slerp(const Quaternion &q1, const Quaternion &q2, float t);

      float w, y, z, x;
};


END_ENGINE_NAMESPACE


#endif