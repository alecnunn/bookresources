/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<Vector3D.h>
#include<math.h>


DECLARE_ENGINE_NAMESPACE


Vector3D::Vector3D()
{
   x = 0;
   y = 0;
   z = 0;
}


Vector3D::Vector3D(scalar X, scalar Y, scalar Z)
{
   x = X;
   y = Y;
   z = Z;
}


Vector3D::Vector3D(const Vector3D &v)
{
   x = v.x;
   y = v.y;
   z = v.z;
}


void Vector3D::Add(const Vector3D &v1, const Vector3D &v2)
{
   x = v1.x + v2.x;
   y = v1.y + v2.y;
   z = v1.z + v2.z;
}


void Vector3D::Subtract(const Vector3D &v1, const Vector3D &v2)
{
   x = v1.x - v2.x;
   y = v1.y - v2.y;
   z = v1.z - v2.z;
}


void Vector3D::Multiply(const Vector3D &v1, const Vector3D &v2)
{
   x = v1.x * v2.x;
   y = v1.y * v2.y;
   z = v1.z * v2.z;
}


void Vector3D::Divide(const Vector3D &v1, const Vector3D &v2)
{
   x = v1.x / v2.x;
   y = v1.y / v2.y;
   z = v1.z / v2.z;
}


void Vector3D::Add(const Vector3D &v1, float f)
{
   x = v1.x + f;
   y = v1.y + f;
   z = v1.z + f;
}


void Vector3D::Subtract(const Vector3D &v1, float f)
{
   x = v1.x - f;
   y = v1.y - f;
   z = v1.z - f;
}


void Vector3D::Multiply(const Vector3D &v1, float f)
{
   x = v1.x * f;
   y = v1.y * f;
   z = v1.z * f;
}


void Vector3D::Divide(const Vector3D &v1, float f)
{
   if(f != 0)
      f = 1 / f;

   x = v1.x * f;
   y = v1.y * f;
   z = v1.z * f;
}


void Vector3D::operator=(const Vector3D &v)
{
   x = v.x;
   y = v.y;
   z = v.z;
}


void Vector3D::operator+=(const Vector3D &v)
{
   x += v.x;
   y += v.y;
   z += v.z;
}


void Vector3D::operator-=(const Vector3D &v)
{
   x -= v.x;
   y -= v.y;
   z -= v.z;
}


void Vector3D::operator/=(const Vector3D &v)
{
   x /= v.x;
   y /= v.y;
   z /= v.z;
}


void Vector3D::operator*=(const Vector3D &v)
{
   x *= v.x;
   y *= v.y;
   z *= v.z;
}


Vector3D Vector3D::operator+(const Vector3D &v2)
{
   return Vector3D(x + v2.x, y + v2.y, z + v2.z);
}


Vector3D Vector3D::operator-(const Vector3D &v2)
{
   return Vector3D(x - v2.x, y - v2.y, z - v2.z);
}


Vector3D Vector3D::operator/(const Vector3D &v2)
{
   return Vector3D(x / v2.x, y / v2.y, z / v2.z);
}


Vector3D Vector3D::operator*(const Vector3D &v2)
{
   return Vector3D(x * v2.x, y * v2.y, z * v2.z);
}


Vector3D Vector3D::operator+(float f)
{
   return Vector3D(x + f, y + f, z + f);
}


Vector3D Vector3D::operator-(float f)
{
   return Vector3D(x - f, y - f, z - f);
}


Vector3D Vector3D::operator/(float f)
{
   return Vector3D(x / f, y / f, z / f);
}


Vector3D Vector3D::operator*(float f)
{
   return Vector3D(x * f, y * f, z * f);
}


void Vector3D::Negate()
{
   x = -x; y = -y; z = -z;
}


scalar Vector3D::Dot3(const Vector3D &v)
{
   return x * x + y * y + z * z;
}


scalar Vector3D::Magnitude()
{
   return (scalar)sqrt(x * x + y * y + z * z);
}


void Vector3D::Normalize()
{
   scalar len = Magnitude();

   if(len <= 0.00001)
      len = 1;

   len = 1 / len;

   x *= len; y *= len; z *= len;
}


void Vector3D::Normalize(Vector3D p1, Vector3D p2, Vector3D p3)
{
   Vector3D e1 = p1 - p2;
   Vector3D e2 = p2 - p3;

   e1.Normalize();
   e2.Normalize();

   *this = e1.CrossProduct(e2);
   Normalize();
}


Vector3D Vector3D::CrossProduct(const Vector3D &v)
{
   return Vector3D(y * v.z - z * v.y,
                   z * v.x - x * v.z,
                   x * v.y - y * v.x);
}


END_ENGINE_NAMESPACE