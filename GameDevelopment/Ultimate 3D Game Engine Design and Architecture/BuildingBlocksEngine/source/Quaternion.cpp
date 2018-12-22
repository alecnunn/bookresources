/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<Quaternion.h>
#include<math.h>


DECLARE_ENGINE_NAMESPACE


Quaternion::Quaternion()
{
   x = 0;
   y = 0;
   z = 0;
   w = 1;
}


Quaternion::Quaternion(const Quaternion &q)
{
   w = q.w;
   x = q.x;
   y = q.y;
   z = q.z;
}


Quaternion::Quaternion(float X, float Y, float Z, float W)
{
   w = W;
   x = X;
   y = Y;
   z = Z;
}


Quaternion Quaternion::operator*(const Quaternion &q)
{
   return Quaternion(w * q.x + x * q.w + y * q.z - z * q.y,
                      w * q.y - x * q.z + y * q.w + z * q.x,
                      w * q.z + x * q.y - y * q.x + z * q.w,
                      w * q.w - x * q.x - y * q.y - z * q.z);
}


void Quaternion::operator=(const Quaternion &q)
{
   w = q.w;
   x = q.x;
   y = q.y;
   z = q.z;
}


float Quaternion::Magnitude()
{
   return (float)sqrt(x * x + y * y + z * z + w * w);
}


void Quaternion::Normalize()
{
   float len = Magnitude();

   if(len != 0 && len != 1)
      len = 1 / len;

   x *= len; y *= len; z *= len; w *= len;
}


Quaternion Quaternion::Conjugate()
{
   return Quaternion(-x, -y, -z, w);
}


void Quaternion::RotationAxisToQuaternion(float angle, Vector3D &axis)
{
   float angle_1 = (float)DEG_TO_RAD(angle);
   float angle_2 = angle_1 * 0.5f;
	float sine = (float)sin(angle_2);

   w = (float)cos(angle_2);
	x = axis.x * sine;
	y = axis.y * sine;
	z = axis.z * sine;
}


void Quaternion::EulerToQuaternion(Vector3D &euler)
{
   float cosX = 0.5f * cosf(euler.x);
   float cosY = 0.5f * cosf(euler.y);
   float cosZ = 0.5f * cosf(euler.z);
   
   float sinX = 0.5f * sinf(euler.x);
   float sinY = 0.5f * sinf(euler.y);
   float sinZ = 0.5f * sinf(euler.z);
   
   w = cosZ * cosY * cosX + sinZ * sinY * sinX;
   x = cosZ * cosY * sinX - sinZ * sinY * cosX;
   y = cosZ * sinY * cosX + sinZ * cosY * sinX;
   z = sinZ * cosY * cosX - cosZ * sinY * sinX;
}


Quaternion Quaternion::CrossProduct(const Quaternion &q)
{
   Quaternion crossProduct;

   crossProduct.x = w * q.x + x * q.w + y * q.z - z * q.y;
   crossProduct.y = w * q.y + x * q.z + y * q.w - z * q.x;
   crossProduct.z = w * q.z + x * q.y + y * q.x - z * q.w;
   crossProduct.w = w * q.w - x * q.x - y * q.y - z * q.z;

   return crossProduct;
}


void Quaternion::CreateMatrix(float *matrix)
{
	if(!matrix)
	   return;

	matrix[0]  = 1.0f - 2.0f * (y * y + z * z); 
	matrix[1]  = 2.0f * (x * y + z * w);
	matrix[2]  = 2.0f * (x * z - y * w);
	matrix[3]  = 0.0f;  

	matrix[4]  = 2.0f * (x * y - z * w);  
	matrix[5]  = 1.0f - 2.0f * (x * x + z * z); 
	matrix[6]  = 2.0f * (z * y + x * w);  
	matrix[7]  = 0.0f;  

	matrix[8]  = 2.0f * (x * z + y * w);
	matrix[9]  = 2.0f * (y * z - x * w);
	matrix[10] = 1.0f - 2.0f * (x * x + y * y);  
	matrix[11] = 0.0f;  

	matrix[12] = 0;  
	matrix[13] = 0;  
	matrix[14] = 0;  
	matrix[15] = 1.0f;
}


void Quaternion::MatrixToQuaternion(float *matrix)
{
   float trace;
   float sqrtTrace;
   float sqrtTrace2;

   trace = matrix[0] + matrix[5] + matrix[10] + matrix[15];
   sqrtTrace = (float)sqrt(trace);

   sqrtTrace2 = 1.0f / (2.0f * sqrtTrace);

   w = sqrtTrace * 0.5f;
   x = (matrix[6] - matrix[9]) * sqrtTrace2;
   y = (matrix[8] - matrix[2]) * sqrtTrace2;
   z = (matrix[1] - matrix[4]) * sqrtTrace2;
}


void Quaternion::Slerp(const Quaternion &q1, const Quaternion &q2, float t)
{
   float cosTheta = 0.0f;
   float sinTheta = 0.0f;
   float beta = 0.0f;
   float q2Array[4];

   q2Array[0] = q2.x;
   q2Array[1] = q2.y;
   q2Array[2] = q2.z;
   q2Array[3] = q2.w;

   cosTheta = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;

   if(cosTheta < 0.0f)
      {
         q2Array[0] = -q2Array[0];
         q2Array[1] = -q2Array[1];
         q2Array[2] = -q2Array[2];
         q2Array[3] = -q2Array[3];
         cosTheta = -cosTheta;
      }

   beta = 1.0f - t;

   if(1.0f - cosTheta > 0.001f)
      {
         cosTheta = (float)acos(cosTheta);
         sinTheta = 1.0f / (float)sin(cosTheta);
         beta = (float)sin(cosTheta * beta) * sinTheta;
         t = (float)sin(cosTheta * t) * sinTheta;
      }

   x = beta * q1.x + t * q2Array[0];
   y = beta * q1.y + t * q2Array[1];
   z = beta * q1.z + t * q2Array[2];
   w = beta * q1.w + t * q2Array[3];
}


END_ENGINE_NAMESPACE