/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_MATRIX_H_
#define _BB_MATRIX_H_

#include<NameSpace.h>
#include<Defines.h>
#include<Vector3D.h>


DECLARE_ENGINE_NAMESPACE


class Matrix4x4
{
   public:
      Matrix4x4();
      Matrix4x4(float r11, float r12, float r13, float r14,
                float r21, float r22, float r23, float r24,
                float r31, float r32, float r33, float r34,
                float r41, float r42, float r43, float r44);

      void Add(const Matrix4x4 &m1, const Matrix4x4 &m2);
      void Subtract(const Matrix4x4 &m1, const Matrix4x4 &m2);
      void Multiple(const Matrix4x4 &mat1, const Matrix4x4 &mat2);

      void operator=(const Matrix4x4 &m);
      Matrix4x4 operator-(const Matrix4x4 &m);
      Matrix4x4 operator+(const Matrix4x4 &m);
      Matrix4x4 operator*(const Matrix4x4 &m);

      void Identity();
      void Zero();

      void Translate(const Vector3D &v);
      void Translate(float x, float y, float z);

      Vector3D inverseTranslateVector(const Vector3D &v);

      bool inverseMatrix(const Matrix4x4 &m);
      void invertMatrix(const Matrix4x4 &m);

      Vector3D VectorMatrixMultiply(const Vector3D &v);
      Vector3D VectorMatrixMultiply3x3(const Vector3D &v);
      Vector3D VectorMatrixMultiply3x3Inv(const Vector3D &v);

      void Transpose(const Matrix4x4 &m);

      void Scale(const Vector3D &scale);

      void SetRotationRadians(double x, double y, double z);
      void Rotate(float angle, int x, int y, int z);
      void RotateAxis(double angle, Vector3D axis);
      void RotateX(double angle);
      void RotateY(double angle);
      void RotateZ(double angle);

      void CreateViewMatrix(Vector3D pos, Vector3D dir,
                            Vector3D up, Vector3D right);

      float matrix[16];
};


END_ENGINE_NAMESPACE

#endif