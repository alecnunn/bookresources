/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_VECTOR_H_
#define _BB_VECTOR_H_

#include<Defines.h>
#include<NameSpace.h>


DECLARE_ENGINE_NAMESPACE


class Vector3D
{
   public:
      Vector3D();
      Vector3D(scalar X, scalar Y, scalar Z);
      Vector3D(const Vector3D &v);

      void Add(const Vector3D &v1, const Vector3D &v2);
      void Subtract(const Vector3D &v1, const Vector3D &v2);
      void Multiply(const Vector3D &v1, const Vector3D &v2);
      void Divide(const Vector3D &v1, const Vector3D &v2);

      void Add(const Vector3D &v1, float f);
      void Subtract(const Vector3D &v1, float f);
      void Multiply(const Vector3D &v1, float f);
      void Divide(const Vector3D &v1, float f);

      void operator=(const Vector3D &v);
      void operator+=(const Vector3D &v);
      void operator-=(const Vector3D &v);
      void operator/=(const Vector3D &v);
      void operator*=(const Vector3D &v);

      Vector3D operator+(const Vector3D &v2);
      Vector3D operator-(const Vector3D &v2);      
      Vector3D operator/(const Vector3D &v2);
      Vector3D operator*(const Vector3D &v2);

      Vector3D operator+(float f);
      Vector3D operator-(float f);      
      Vector3D operator/(float f);
      Vector3D operator*(float f);

      void Negate();
      scalar Dot3(const Vector3D &v);
      scalar Magnitude();

      void Normalize();
      void Normalize(Vector3D p1, Vector3D p2,
                     Vector3D p3);

      Vector3D CrossProduct(const Vector3D &v);

      scalar x, y, z;
};


END_ENGINE_NAMESPACE

#endif