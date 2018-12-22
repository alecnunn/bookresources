/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<Matrix.h>
#include<math.h>


DECLARE_ENGINE_NAMESPACE


Matrix4x4::Matrix4x4()
{
   Identity();
}


Matrix4x4::Matrix4x4(float r11, float r12, float r13, float r14,
                     float r21, float r22, float r23, float r24,
                     float r31, float r32, float r33, float r34,
                     float r41, float r42, float r43, float r44)
{
   matrix[0]  = r11; matrix[1]  = r12; matrix[2]  = r13; matrix[3]  = r14;
	matrix[4]  = r21; matrix[5]  = r22; matrix[6]  = r23; matrix[7]  = r24;
	matrix[8]  = r31; matrix[9]  = r32; matrix[10] = r33; matrix[11] = r34;
	matrix[12] = r41; matrix[13] = r42; matrix[14] = r43; matrix[15] = r44;
}


void Matrix4x4::Add(const Matrix4x4 &m1, const Matrix4x4 &m2)
{
   matrix[0] = m1.matrix[0] + m2.matrix[0];
   matrix[1] = m1.matrix[1] + m2.matrix[1];
   matrix[2] = m1.matrix[2] + m2.matrix[2];
   matrix[3] = m1.matrix[3] + m2.matrix[3];
   // Row 2 start.
   matrix[4] = m1.matrix[4] + m2.matrix[4];
   matrix[5] = m1.matrix[5] + m2.matrix[5];
   matrix[6] = m1.matrix[6] + m2.matrix[6];
   matrix[7] = m1.matrix[7] + m2.matrix[7];
   // Row 3 start.
   matrix[8] = m1.matrix[8] + m2.matrix[8];
   matrix[9] = m1.matrix[9] + m2.matrix[9];
   matrix[10] = m1.matrix[10] + m2.matrix[10];
   matrix[11] = m1.matrix[11] + m2.matrix[11];
   // Row 4 start.
   matrix[12] = m1.matrix[12] + m2.matrix[12];
   matrix[13] = m1.matrix[13] + m2.matrix[13];
   matrix[14] = m1.matrix[14] + m2.matrix[14];
   matrix[15] = m1.matrix[15] + m2.matrix[15];
}


void Matrix4x4::Subtract(const Matrix4x4 &m1, const Matrix4x4 &m2)
{
   matrix[0] = m1.matrix[0] - m2.matrix[0];
   matrix[1] = m1.matrix[1] - m2.matrix[1];
   matrix[2] = m1.matrix[2] - m2.matrix[2];
   matrix[3] = m1.matrix[3] - m2.matrix[3];
   // Row 2 start.
   matrix[4] = m1.matrix[4] - m2.matrix[4];
   matrix[5] = m1.matrix[5] - m2.matrix[5];
   matrix[6] = m1.matrix[6] - m2.matrix[6];
   matrix[7] = m1.matrix[7] - m2.matrix[7];
   // Row 3 start.
   matrix[8] = m1.matrix[8] - m2.matrix[8];
   matrix[9] = m1.matrix[9] - m2.matrix[9];
   matrix[10] = m1.matrix[10] - m2.matrix[10];
   matrix[11] = m1.matrix[11] - m2.matrix[11];
   // Row 4 start.
   matrix[12] = m1.matrix[12] - m2.matrix[12];
   matrix[13] = m1.matrix[13] - m2.matrix[13];
   matrix[14] = m1.matrix[14] - m2.matrix[14];
   matrix[15] = m1.matrix[15] - m2.matrix[15];
}


void Matrix4x4::Multiple(const Matrix4x4 &mat1, const Matrix4x4 &mat2)
{
	const float *m1 = mat1.matrix, *m2 = mat2.matrix;

	matrix[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2];
	matrix[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2];
	matrix[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2];
	matrix[3] = 0;

	matrix[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6];
	matrix[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6];
	matrix[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6];
	matrix[7] = 0;

	matrix[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10];
	matrix[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10];
	matrix[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10];
	matrix[11] = 0;

	matrix[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12];
	matrix[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13];
	matrix[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14];
	matrix[15] = 1;
}


void Matrix4x4::operator=(const Matrix4x4 &m)
{
   matrix[0]  = m.matrix[0];
   matrix[4]  = m.matrix[4];
   matrix[8]  = m.matrix[8];
   matrix[12] = m.matrix[12];

   matrix[1]  = m.matrix[1];
   matrix[5]  = m.matrix[5];
   matrix[9]  = m.matrix[9];
   matrix[13] = m.matrix[13];

   matrix[2]  = m.matrix[2];
   matrix[6]  = m.matrix[6];
   matrix[10] = m.matrix[10];
   matrix[14] = m.matrix[14];

   matrix[3]  = m.matrix[3];
   matrix[7]  = m.matrix[7];
   matrix[11] = m.matrix[11];
   matrix[15] = m.matrix[15];
}


Matrix4x4 Matrix4x4::operator-(const Matrix4x4 &m)
{
   return Matrix4x4(matrix[0] - m.matrix[0],
                    matrix[1] - m.matrix[1],
                    matrix[2] - m.matrix[2],
                    matrix[3] - m.matrix[3],
                    // Row 2 start.
                    matrix[4] - m.matrix[4],
                    matrix[5] - m.matrix[5],
                    matrix[6] - m.matrix[6],
                    matrix[7] - m.matrix[7],
                    // Row 3 start.
                    matrix[8] - m.matrix[8],
                    matrix[9] - m.matrix[9],
                    matrix[10] - m.matrix[10],
                    matrix[11] - m.matrix[11],
                    // Row 4 start.
                    matrix[12] - m.matrix[12],
                    matrix[13] - m.matrix[13],
                    matrix[14] - m.matrix[14],
                    matrix[15] - m.matrix[15]);
}


Matrix4x4 Matrix4x4::operator+(const Matrix4x4 &m)
{
   return Matrix4x4(matrix[0] + m.matrix[0],
                    matrix[1] + m.matrix[1],
                    matrix[2] + m.matrix[2],
                    matrix[3] + m.matrix[3],
                    // Row 2 start.
                    matrix[4] + m.matrix[4],
                    matrix[5] + m.matrix[5],
                    matrix[6] + m.matrix[6],
                    matrix[7] + m.matrix[7],
                    // Row 3 start.
                    matrix[8] + m.matrix[8],
                    matrix[9] + m.matrix[9],
                    matrix[10] + m.matrix[10],
                    matrix[11] + m.matrix[11],
                    // Row 4 start.
                    matrix[12] + m.matrix[12],
                    matrix[13] + m.matrix[13],
                    matrix[14] + m.matrix[14],
                    matrix[15] + m.matrix[15]);
}


Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &m)
{
   // Return the value of this vector * m.
   float newMatrix[16];
	const float *m1 = matrix, *m2 = m.matrix;

	newMatrix[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2];
	newMatrix[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2];
	newMatrix[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2];
	newMatrix[3] = 0;

	newMatrix[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6];
	newMatrix[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6];
	newMatrix[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6];
	newMatrix[7] = 0;

	newMatrix[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10];
	newMatrix[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10];
	newMatrix[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10];
	newMatrix[11] = 0;

	newMatrix[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12];
	newMatrix[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13];
	newMatrix[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14];
	newMatrix[15] = 1;

   return Matrix4x4(newMatrix[0], newMatrix[1], newMatrix[2], newMatrix[3], newMatrix[4],
                    newMatrix[5], newMatrix[6], newMatrix[7], newMatrix[8], newMatrix[9],
                    newMatrix[10], newMatrix[11], newMatrix[12], newMatrix[13], newMatrix[14],
                    newMatrix[15]);
}


void Matrix4x4::Identity()
{
   matrix[0]  = 1.0f; matrix[1]  = 0.0f; matrix[2]  = 0.0f; matrix[3]  = 0.0f;
	matrix[4]  = 0.0f; matrix[5]  = 1.0f; matrix[6]  = 0.0f; matrix[7]  = 0.0f;
	matrix[8]  = 0.0f; matrix[9]  = 0.0f; matrix[10] = 1.0f; matrix[11] = 0.0f;
	matrix[12] = 0.0f; matrix[13] = 0.0f; matrix[14] = 0.0f; matrix[15] = 1.0f;
}


void Matrix4x4::Zero()
{
   matrix[0]  = 0.0f; matrix[1]  = 0.0f; matrix[2]  = 0.0f; matrix[3]  = 0.0f;
	matrix[4]  = 0.0f; matrix[5]  = 0.0f; matrix[6]  = 0.0f; matrix[7]  = 0.0f;
	matrix[8]  = 0.0f; matrix[9]  = 0.0f; matrix[10] = 0.0f; matrix[11] = 0.0f;
	matrix[12] = 0.0f; matrix[13] = 0.0f; matrix[14] = 0.0f; matrix[15] = 0.0f;
}

void Matrix4x4::Translate(const Vector3D &v)
{
   matrix[12] = v.x;
   matrix[13] = v.y;
   matrix[14] = v.z;
   matrix[15] = 1.0f;
}


void Matrix4x4::Translate(float x, float y, float z)
{
   matrix[12] = x;
   matrix[13] = y;
   matrix[14] = z;
   matrix[15] = 1.0f;
}


Vector3D Matrix4x4::inverseTranslateVector(const Vector3D &v)
{
   return Vector3D(v.x - matrix[12],
                   v.y - matrix[13],
                   v.z - matrix[14]);
}


bool Matrix4x4::inverseMatrix(const Matrix4x4 &m)
{
   float tempMatrix[16] = {0};
   float d12, d13, d23, d24, d34, d41;

	d12 = matrix[2]  * matrix[7]  - matrix[3]  * matrix[6];
	d13 = matrix[2]  * matrix[11] - matrix[3]  * matrix[10];
	d23 = matrix[6]  * matrix[11] - matrix[7]  * matrix[10];
	d24 = matrix[6]  * matrix[15] - matrix[7]  * matrix[14];
	d34 = matrix[10] * matrix[15] - matrix[11] * matrix[14];
	d41 = matrix[14] * matrix[3]  - matrix[15] * matrix[2];

	tempMatrix[0] =   matrix[5] * d34 - matrix[9] * d24 + matrix[13] * d23;
	tempMatrix[1] = -(matrix[1] * d34 + matrix[9] * d41 + matrix[13] * d13);
	tempMatrix[2] =   matrix[1] * d24 + matrix[5] * d41 + matrix[13] * d12;
	tempMatrix[3] = -(matrix[1] * d23 - matrix[5] * d13 + matrix[9]  * d12);

	float determinant = matrix[0] * tempMatrix[0] + matrix[4] * tempMatrix[1] +
                       matrix[8] * tempMatrix[2] + matrix[12] * tempMatrix[3];

	if(determinant == 0.0)
	   {
		   Identity();
         return false;
	   }

	float invDeterminant = 1.0f / determinant;
	
	tempMatrix[0] *= invDeterminant;
	tempMatrix[1] *= invDeterminant;
	tempMatrix[2] *= invDeterminant;
	tempMatrix[3] *= invDeterminant;

	tempMatrix[4] = -(matrix[4] * d34 - matrix[8] * d24 + matrix[12] * d23) * invDeterminant;
	tempMatrix[5] =   matrix[0] * d34 + matrix[8] * d41 + matrix[12] * d13  * invDeterminant;
	tempMatrix[6] = -(matrix[0] * d24 + matrix[4] * d41 + matrix[12] * d12) * invDeterminant;
	tempMatrix[7] =   matrix[0] * d23 - matrix[4] * d13 + matrix[8]  * d12  * invDeterminant;

	d12 = matrix[0]  * matrix[5]  - matrix[1]  * matrix[12];
	d13 = matrix[0]  * matrix[9]  - matrix[1]  * matrix[8];
	d23 = matrix[4]  * matrix[9]  - matrix[5]  * matrix[8];
	d24 = matrix[4]  * matrix[13] - matrix[5]  * matrix[12];
	d34 = matrix[8]  * matrix[13] - matrix[9]  * matrix[12];
	d41 = matrix[12] * matrix[1]  - matrix[13] * matrix[0];

	tempMatrix[8]  =   matrix[7] * d34 - matrix[11] * d24 + matrix[15] * d23 * invDeterminant;
	tempMatrix[9]  = -(matrix[3] * d34 + matrix[11] * d41 + matrix[15] * d13) * invDeterminant;
	tempMatrix[10] =   matrix[3] * d24 + matrix[7]  * d41 + matrix[15] * d12 * invDeterminant;
	tempMatrix[11] = -(matrix[3] * d23 - matrix[7]  * d13 + matrix[11] * d12) * invDeterminant;
	tempMatrix[12] = -(matrix[6] * d34 - matrix[10] * d24 + matrix[14] * d23) * invDeterminant;
	tempMatrix[13] =   matrix[2] * d34 + matrix[10] * d41 + matrix[14] * d13 * invDeterminant;
	tempMatrix[14] = -(matrix[2] * d24 + matrix[6]  * d41 + matrix[14] * d12) * invDeterminant;
	tempMatrix[15] =   matrix[2] * d23 - matrix[6]  * d13 + matrix[10] * d12 * invDeterminant;

   matrix[0]  = tempMatrix[0];
   matrix[1]  = tempMatrix[1];
   matrix[2]  = tempMatrix[2];
   matrix[3]  = tempMatrix[3];

	matrix[4]  = tempMatrix[4];
	matrix[5]  = tempMatrix[5];
   matrix[6]  = tempMatrix[6];
   matrix[7]  = tempMatrix[7];

	matrix[8]  = tempMatrix[8];
	matrix[9]  = tempMatrix[9];
   matrix[10] = tempMatrix[10];
   matrix[11] = tempMatrix[11];

	matrix[12] = tempMatrix[12];
	matrix[13] = tempMatrix[13];
   matrix[14] = tempMatrix[14];
   matrix[15] = tempMatrix[15];

   return true;
}


void Matrix4x4::invertMatrix(const Matrix4x4 &m)
{
   Transpose(m);

   // Identity shearing terms
   matrix[3] = 0.0f;
   matrix[7] = 0.0f;
   matrix[11] = 0.0f;
   matrix[15] = 1.0f;

   matrix[12] = -(m.matrix[12] * m.matrix[0]) - (m.matrix[13] * m.matrix[1]) - (m.matrix[14] * m.matrix[2]);
   matrix[13] = -(m.matrix[12] * m.matrix[4]) - (m.matrix[13] * m.matrix[5]) - (m.matrix[14] * m.matrix[6]);
   matrix[14] = -(m.matrix[12] * m.matrix[8]) - (m.matrix[13] * m.matrix[9]) - (m.matrix[14] * m.matrix[10]);
}


Vector3D Matrix4x4::VectorMatrixMultiply(const Vector3D &v)
{
   Vector3D out;

   out.x = (v.x * matrix[0]) + (v.y * matrix[4]) + (v.z * matrix[8]) + matrix[12];
   out.y = (v.x * matrix[1]) + (v.y * matrix[5]) + (v.z * matrix[9]) + matrix[13];
   out.z = (v.x * matrix[2]) + (v.y * matrix[6]) + (v.z * matrix[10]) + matrix[14];

   return out;
}


Vector3D Matrix4x4::VectorMatrixMultiply3x3(const Vector3D &v)
{
   Vector3D out;

   out.x = (v.x * matrix[0]) + (v.y * matrix[4]) + (v.z * matrix[8]);
   out.y = (v.x * matrix[1]) + (v.y * matrix[5]) + (v.z * matrix[9]);
   out.z = (v.x * matrix[2]) + (v.y * matrix[6]) + (v.z * matrix[10]);

   return out;
}


Vector3D Matrix4x4::VectorMatrixMultiply3x3Inv(const Vector3D &v)
{
   Vector3D out;

   out.x = (v.x * matrix[0]) + (v.y * matrix[1]) + (v.z * matrix[2]);
   out.y = (v.x * matrix[4]) + (v.y * matrix[5]) + (v.z * matrix[6]);
   out.z = (v.x * matrix[8]) + (v.y * matrix[9]) + (v.z * matrix[10]);

   return out;
}


void Matrix4x4::Transpose(const Matrix4x4 &m)
{
   matrix[0] = m.matrix[0];
   matrix[1] = m.matrix[4];
   matrix[2] = m.matrix[8]; 

   matrix[4] = m.matrix[1];
   matrix[5] = m.matrix[5];
   matrix[6] = m.matrix[9];

   matrix[8] = m.matrix[2];
   matrix[9] = m.matrix[6];
   matrix[10] = m.matrix[10];

   matrix[3] = m.matrix[12];
   matrix[7] = m.matrix[13];
   matrix[11] = m.matrix[14];

   matrix[12] = m.matrix[3];
   matrix[13] = m.matrix[7];
   matrix[14] = m.matrix[11];
}


void Matrix4x4::Scale(const Vector3D &scale)
{
   matrix[0] = scale.x;
	matrix[5] = scale.y;
	matrix[10] = scale.z;
}


void Matrix4x4::SetRotationRadians(double x, double y, double z)
{
	double cosX = cos(x), cosY = cos(y), cosZ = cos(z);
	double sinX = sin(x), sinY = sin(y), sinZ = sin(z);

	matrix[0] = (float)(cosY * cosZ);
	matrix[1] = (float)(cosY * sinZ);
	matrix[2] = (float)-sinY;

	matrix[4] = (float)((sinX * sinY) * cosZ - cosX * sinZ);
	matrix[5] = (float)((sinX * sinY) * sinZ + cosX * cosZ);
	matrix[6] = (float)(sinX * cosY);

	matrix[8] = (float)((cosX * sinY) * cosZ + sinX * sinZ);
	matrix[9] = (float)((cosX * sinY) * sinZ - sinX * cosZ);
	matrix[10] = (float)(cosX * cosY);
}


void Matrix4x4::Rotate(float angle, int x, int y, int z)
{
   angle = (float)DEG_TO_RAD(angle);
   float cosAngle = (float)cos(angle);
   float sineAngle = (float)sin(angle);

   if(z)
      {
         matrix[0] = cosAngle;
	      matrix[1] = sineAngle;
	      matrix[4] = -sineAngle;
	      matrix[5] = cosAngle;
      }

   if(y)
      {
         matrix[0]  = cosAngle;
	      matrix[2]  = -sineAngle;
	      matrix[8]  = sineAngle;
	      matrix[10] = cosAngle;
      }

   if(x)
      {
	      matrix[5]  = cosAngle;
	      matrix[6]  = sineAngle;
	      matrix[9]  = -sineAngle;
	      matrix[10] = cosAngle;
      }
}


void Matrix4x4::RotateAxis(double angle, Vector3D axis)
{
	axis.Normalize();

	float sinAngle = (float)sin(PI_CONST * angle / 180);
	float cosAngle = (float)cos(PI_CONST * angle / 180);
	float oneSubCos = 1.0f - cosAngle;

	matrix[0] = (axis.x * axis.x) * oneSubCos + cosAngle;
	matrix[4] = (axis.x * axis.y) * oneSubCos - (axis.z * sinAngle);
	matrix[8] = (axis.x * axis.z) * oneSubCos + (axis.y * sinAngle);

	matrix[1] = (axis.y * axis.x) * oneSubCos + (sinAngle * axis.z);
	matrix[5] = (axis.y * axis.y) * oneSubCos + cosAngle;
	matrix[9] = (axis.y * axis.z) * oneSubCos - (axis.x * sinAngle);
	
	matrix[2] = (axis.z * axis.x) * oneSubCos - (axis.y * sinAngle);
	matrix[6] = (axis.z * axis.y) * oneSubCos + (axis.x * sinAngle);
	matrix[10] = (axis.z * axis.z) * oneSubCos + cosAngle;
}


void Matrix4x4::RotateX(double angle)
{
	matrix[5] = (float)cos(PI_CONST * angle / 180);
	matrix[6] = (float)sin(PI_CONST * angle / 180);

	matrix[9] = -matrix[6];
	matrix[10] = matrix[5];
}

void Matrix4x4::RotateY(double angle)
{
	matrix[0] = (float)cos(PI_CONST * angle / 180);
	matrix[2] = -(float)sin(PI_CONST * angle / 180);

	matrix[8] = -matrix[2];
	matrix[10] = matrix[0];
}


void Matrix4x4::RotateZ(double angle)
{
	matrix[0] = (float)cos(PI_CONST * angle / 180);
	matrix[1] = (float)sin(PI_CONST * angle / 180);

	matrix[4] = -matrix[1];
	matrix[5] = matrix[0];
}


void Matrix4x4::CreateViewMatrix(Vector3D pos, Vector3D dir,
                                 Vector3D up, Vector3D right)
{
   matrix[3] = matrix[7] = matrix[11] = 0.0f;
   matrix[15] = 1.0f;
   
   matrix[0] = right.x;
   matrix[4] = right.y;
   matrix[8] = right.z;
   matrix[12] = right.Dot3(pos);
   
   matrix[1] = up.x;
   matrix[5] = up.y;
   matrix[9] = up.z;
   matrix[13] = up.Dot3(pos);
   
   matrix[2] = dir.x;
   matrix[6] = dir.y;
   matrix[10] = dir.z;
   matrix[14] = dir.Dot3(pos);
}


END_ENGINE_NAMESPACE