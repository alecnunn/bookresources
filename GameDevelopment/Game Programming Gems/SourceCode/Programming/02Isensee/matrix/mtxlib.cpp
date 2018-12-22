/* Copyright (C) Dante Treglia II and Mark A. DeLoura, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dante Treglia II and Mark A. DeLoura, 2000"
 */
//==========================================================
// C++ Matrix Library
// Version: 2.6
// Date: May 19, 2000
// Authors: Dante Treglia II and Mark A. DeLoura
// Thanks to: Miguel Gomez, Stan Melax, Pete Isensee, 
//   Gabor Nagy, Scott Bilas, James Boer, Eric Lengyel
//==========================================================
#include "mtxlib.h"
#include <cmath>
#include <cassert>

////////////////////////////////////////////////////////////
// Miscellaneous vector functions
//

// Return Normal of vector2's
vector2 Normalized(const vector2 &a)
{
  vector2 ret(a);
  return ret.normalize();
}
// Return Normal of vector3's
vector3 Normalized(const vector3 &a)
{
  vector3 ret(a);
  return ret.normalize();
}
// Return Normal of vector4's
vector4 Normalized(const vector4 &a)
{
  vector4 ret(a);
  return ret.normalize();
}

// Dot product of two vector2's
float DotProduct(const vector2 &a, const vector2 &b) 
{
  return a.x*b.x + a.y*b.y;
}

// Dot product of two vector3's
float DotProduct(const vector3 &a, const vector3 &b) 
{
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

// Dot product of two vector4's
float DotProduct(const vector4 &a, const vector4 &b) 
{
  return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

// Swap two vector2's
void SwapVec(vector2 &a, vector2 &b) 
{
  vector2 tmp(a);

  a = b;
  b = tmp;
}

// Swap two vector3's
void SwapVec(vector3 &a, vector3 &b) 
{
  vector3 tmp(a);

  a = b;
  b = tmp;
}

// Swap two vector4's
void SwapVec(vector4 &a, vector4 &b) 
{
  vector4 tmp(a);

  a = b;
  b = tmp;
}

// Cross product of two vector3's
vector3 CrossProduct(const vector3 &a, const vector3 &b) 
{
  return vector3(a.y*b.z - a.z*b.y,
      a.z*b.x - a.x*b.z,
      a.x*b.y - a.y*b.x);
}

// Are these two vector2's nearly equal?
bool NearlyEquals( const vector2& a, const vector2& b, float r ) 
{
  vector2 diff = a - b;  // difference

  return(DotProduct(diff, diff) < r*r);  // radius
}

// Are these two vector3's nearly equal?
bool NearlyEquals( const vector3& a, const vector3& b, float r ) 
{
  vector3 diff = a - b;  // difference

  return(DotProduct(diff, diff) < r*r);  // radius
}

// Are these two vector4's nearly equal?
bool NearlyEquals( const vector4& a, const vector4& b, float r ) 
{
  vector4 diff = a - b;  // difference

  return(DotProduct(diff, diff) < r*r);  // radius
}


////////////////////////////////////////////////////////////
// matrix33 class
//

// Multiply the matrix33 by another matrix33
matrix33 &matrix33::operator *= (const matrix33 &m) 
{
  matrix33 t;

  for (unsigned int r = 0; r < 3; r++)
  {
  for (unsigned int c = 0; c < 3; c++)
  {
    float f = 0;

    f += col[0][r] * m[c][0];
    f += col[1][r] * m[c][1];
    f += col[2][r] * m[c][2];

    t[c][r] = f;
  }
  }

  *this = t;

  return *this;
}

// Transpose the matrix33
matrix33 &matrix33::transpose() 
{
  float t;

  for (unsigned int c = 0; c < 3; c++)
  {
  for (unsigned int r = c + 1; r < 3; r++)
  {
    t = col[c][r];
    col[c][r] = col[r][c];
    col[r][c] = t;
  } 
  }

  return *this;
}

// Invert the matrix33
matrix33 &matrix33::invert() 
{
  matrix33 a(*this);
  matrix33 b(IdentityMatrix33());

  unsigned int c, r;
  unsigned int cc;
  unsigned int rowMax; // Points to max abs value row in this column
  unsigned int row;
  float tmp;

  // Go through columns
  for (c=0; c<3; c++)
  {
  // Find the row with max value in this column
  rowMax = c;
  for (r=c+1; r<3; r++)
  {
    if (fabs(a[c][r]) > fabs(a[c][rowMax]))
    {
    rowMax = r;
    }
  }

  // If the max value here is 0, we can't invert.  Return identity.
  if (a[rowMax][c] == 0.0F)
    return(identity());

  // Swap row "rowMax" with row "c"
  for (cc=0; cc<3; cc++)
  {
    tmp = a[cc][c];
    a[cc][c] = a[cc][rowMax];
    a[cc][rowMax] = tmp;
    tmp = b[cc][c];
    b[cc][c] = b[cc][rowMax];
    b[cc][rowMax] = tmp;
  }

  // Now everything we do is on row "c".
  // Set the max cell to 1 by dividing the entire row by that value
  tmp = a[c][c];
  for (cc=0; cc<3; cc++)
  {
    a[cc][c] /= tmp;
    b[cc][c] /= tmp;
  }

  // Now do the other rows, so that this column only has a 1 and 0's
  for (row = 0; row < 3; row++)
  {
    if (row != c)
    {
    tmp = a[c][row];
    for (cc=0; cc<3; cc++)
    {
      a[cc][row] -= a[cc][c] * tmp;
      b[cc][row] -= b[cc][c] * tmp;
    }
    }
  }

  }

  *this = b;

  return *this;
}

// Return a matrix33 set to the identity matrix
matrix33 IdentityMatrix33() 
{
  matrix33 ret;

  return ret.identity();
}

// Return the transpose of the matrix33
matrix33 TransposeMatrix33(const matrix33 &m) 
{
  matrix33 ret(m);

  return ret.transpose();
}

// Return the inverted matrix33
matrix33 InvertMatrix33(const matrix33 &m) 
{
  matrix33 ret(m);

  return ret.invert();
}

// Return a 2D rotation matrix33
matrix33 RotateRadMatrix33(float rad) 
{
  matrix33 ret;
  float sinA, cosA;

  sinA = (float)sin(rad);
  cosA = (float)cos(rad);

  ret[0][0] = cosA; ret[1][0] = -sinA; ret[2][0] = 0.0F;
  ret[0][1] = sinA; ret[1][1] =  cosA; ret[2][1] = 0.0F;
  ret[0][2] = 0.0F; ret[1][2] =  0.0F; ret[2][2] = 1.0F;

  return ret;
}

// Return a 2D translation matrix33
matrix33 TranslateMatrix33(float x, float y) 
{
  matrix33 ret;

  ret.identity();
  ret[2][0] = x;
  ret[2][1] = y;

  return ret;
}

// Return a 2D/3D scale matrix33
matrix33 ScaleMatrix33(float x, float y, float z) 
{
  matrix33 ret;

  ret.identity();
  ret[0][0] = x;
  ret[1][1] = y;
  ret[2][2] = z;

  return ret;
}


////////////////////////////////////////////////////////////
// matrix44 class
//

// Multiply the matrix44 by another matrix44
matrix44 &matrix44::operator *= (const matrix44 &m) 
{
  matrix44 t;
  for (unsigned int r = 0; r < 4; r++)
  {
  for (unsigned int c = 0; c < 4; c++)
  {
    float f = 0;

    f += (col[0][r] * m[c][0]);
    f += (col[1][r] * m[c][1]);
    f += (col[2][r] * m[c][2]);
    f += (col[3][r] * m[c][3]);

    t[c][r] = f;
  }
  }
  *this = t;
  return *this;
}


// Transpose the matrix44
matrix44 &matrix44::transpose() 
{
  float t;

  for (unsigned int c = 0; c < 4; c++)
  {
  for (unsigned int r = c + 1; r < 4; r++)
  {
    t = col[c][r];
    col[c][r] = col[r][c];
    col[r][c] = t;
  } 
  } 

  return *this;
}

// Invert the matrix44
matrix44 &matrix44::invert() 
{
  matrix44 a(*this);
  matrix44 b(IdentityMatrix44());

  unsigned int r, c;
  unsigned int cc;
  unsigned int rowMax; // Points to max abs value row in this column
  unsigned int row;
  float tmp;

  // Go through columns
  for (c=0; c<4; c++)
  {

  // Find the row with max value in this column
  rowMax = c;
  for (r=c+1; r<4; r++)
  {
    if (fabs(a[c][r]) > fabs(a[c][rowMax]))
    {
    rowMax = r;
    }
  }

  // If the max value here is 0, we can't invert.  Return identity.
  if (a[rowMax][c] == 0.0F)
    return(identity());

  // Swap row "rowMax" with row "c"
  for (cc=0; cc<4; cc++)
  {
    tmp = a[cc][c];
    a[cc][c] = a[cc][rowMax];
    a[cc][rowMax] = tmp;
    tmp = b[cc][c];
    b[cc][c] = b[cc][rowMax];
    b[cc][rowMax] = tmp;
  }

  // Now everything we do is on row "c".
  // Set the max cell to 1 by dividing the entire row by that value
  tmp = a[c][c];
  for (cc=0; cc<4; cc++)
  {
    a[cc][c] /= tmp;
    b[cc][c] /= tmp;
  }

  // Now do the other rows, so that this column only has a 1 and 0's
  for (row = 0; row < 4; row++)
  {
    if (row != c)
    {
    tmp = a[c][row];
    for (cc=0; cc<4; cc++)
    {
      a[cc][row] -= a[cc][c] * tmp;
      b[cc][row] -= b[cc][c] * tmp;
    }
    }
  }

  }

  *this = b;

  return *this;
}

// Return a matrix44 set to the identity matrix
matrix44 IdentityMatrix44() 
{
  matrix44 ret;

  return ret.identity();
}

// Return the transpose of the matrix44
matrix44 TransposeMatrix44(const matrix44 &m) 
{
  matrix44 ret(m);

  return ret.transpose();
}

// Return the inverted matrix44
matrix44 InvertMatrix44(const matrix44 &m) 
{
  matrix44 ret(m);

  return ret.invert();
}

// Return a 3D axis-rotation matrix44
// Pass in 'x', 'y', or 'z' for the axis.
matrix44 RotateRadMatrix44(char axis, float rad) 
{
  matrix44 ret;
  float sinA, cosA;

  sinA = (float)sin(rad);
  cosA = (float)cos(rad);

  switch (axis)
  {
  case 'x':
  case 'X':
    ret[0][0] =  1.0F; ret[1][0] =  0.0F; ret[2][0] =  0.0F;
    ret[0][1] =  0.0F; ret[1][1] =  cosA; ret[2][1] = -sinA;
    ret[0][2] =  0.0F; ret[1][2] =  sinA; ret[2][2] =  cosA;
    break;

  case 'y':
  case 'Y':
    ret[0][0] =  cosA; ret[1][0] =  0.0F; ret[2][0] =  sinA;
    ret[0][1] =  0.0F; ret[1][1] =  1.0F; ret[2][1] =  0.0F;
    ret[0][2] = -sinA; ret[1][2] =  0.0F; ret[2][2] =  cosA;
    break;

  case 'z':
  case 'Z':
    ret[0][0] =  cosA; ret[1][0] = -sinA; ret[2][0] =  0.0F;
    ret[0][1] =  sinA; ret[1][1] =  cosA; ret[2][1] =  0.0F;
    ret[0][2] =  0.0F; ret[1][2] =  0.0F; ret[2][2] =  1.0F;
    break;
  }

  ret[0][3] = 0.0F; ret[1][3] = 0.0F; ret[2][3] = 0.0F;
  ret[3][0] = 0.0F;
  ret[3][1] = 0.0F;
  ret[3][2] = 0.0F;
  ret[3][3] = 1.0F;

  return ret;
}

// Return a 3D axis-rotation matrix44
// Pass in an arbitrary vector3 axis.
matrix44 RotateRadMatrix44(const vector3 &axis, float rad) 
{
  matrix44 ret;
  float sinA, cosA;
  float invCosA;
  vector3 nrm = axis;
  float x, y, z;
  float xSq, ySq, zSq;

  nrm.normalize();
  sinA = (float)sin(rad);
  cosA = (float)cos(rad);
  invCosA = 1.0F - cosA;

  x = nrm.x;
  y = nrm.y;
  z = nrm.z;

  xSq = x * x;
  ySq = y * y;
  zSq = z * z;

  ret[0][0] = (invCosA * xSq) + (cosA);
  ret[1][0] = (invCosA * x * y) - (sinA * z );
  ret[2][0] = (invCosA * x * z) + (sinA * y );
  ret[3][0] = 0.0F;

  ret[0][1] = (invCosA * x * y) + (sinA * z);
  ret[1][1] = (invCosA * ySq) + (cosA);
  ret[2][1] = (invCosA * y * z) - (sinA * x);
  ret[3][1] = 0.0F;

  ret[0][2] = (invCosA * x * z) - (sinA * y);
  ret[1][2] = (invCosA * y * z) + (sinA * x);
  ret[2][2] = (invCosA * zSq) + (cosA);
  ret[3][2] = 0.0F;

  ret[0][3] = 0.0F;
  ret[1][3] = 0.0F;
  ret[2][3] = 0.0F;
  ret[3][3] = 1.0F;

  return ret;
}

// Return a 3D translation matrix44
matrix44 TranslateMatrix44(float x, float y, float z) 
{
  matrix44 ret;

  ret.identity();
  ret[3][0] = x;
  ret[3][1] = y;
  ret[3][2] = z;

  return ret;
}

// Return a 3D/4D scale matrix44
matrix44 ScaleMatrix44(float x, float y, float z, float w) 
{
  matrix44 ret;

  ret.identity();
  ret[0][0] = x;
  ret[1][1] = y;
  ret[2][2] = z;
  ret[3][3] = w;

  return ret;
}

// Return a "lookat" matrix44 given the current camera position (vector3),
//   camera-up vector3, and camera-target vector3.
matrix44 LookAtMatrix44(const vector3 &camPos, const vector3 &target, 
    const vector3 &camUp ) 
{
  matrix44 ret;

  vector3 F = target - camPos;
  F.normalize();

  vector3 S = CrossProduct(F, Normalized(camUp));
  S.normalize();

  vector3 U = CrossProduct(S, F);
  U.normalize();

  ret[0][0] = S.x;
  ret[1][0] = S.y;
  ret[2][0] = S.z;
  ret[3][0] = 0.0;

  ret[0][1] = U.x;
  ret[1][1] = U.y;
  ret[2][1] = U.z;
  ret[3][1] = 0.0;

  ret[0][2] = -F.x;
  ret[1][2] = -F.y;
  ret[2][2] = -F.z;
  ret[3][2] = 0.0;

  ret[0][3] = 0.0F;
  ret[1][3] = 0.0F;
  ret[2][3] = 0.0F;
  ret[3][3] = 1.0F;

  ret *= TranslateMatrix44(-camPos.x, -camPos.y, -camPos.z);

  return ret;
}

// Return a frustum matrix44 given the left, right, bottom, top,
//   near, and far values for the frustum boundaries.
matrix44 FrustumMatrix44(float l, float r, 
    float b, float t, float n, float f) 
{
  matrix44 ret;
  float width = r-l;
  float height = t-b;
  float depth = f-n;

  ret[0][0] = (2*n) / width;
  ret[0][1] = 0.0F;
  ret[0][2] = 0.0F;
  ret[0][3] = 0.0F;

  ret[1][0] = 0.0F;
  ret[1][1] = (2*n) / height;
  ret[1][2] = 0.0F;
  ret[1][3] = 0.0F;

  ret[2][0] = (r + l) / width;
  ret[2][1] = (t + b) / height;
  ret[2][2] = -(f + n) / depth;
  ret[2][3] = -1.0F;

  ret[3][0] = 0.0F;
  ret[3][1] = 0.0F;
  ret[3][2] = -(2*f*n) / depth;
  ret[3][3] = 0.0F;

  return ret;
}

// Return a perspective matrix44 given the field-of-view in the Y
//   direction in degrees, the aspect ratio of Y/X, and near and
//   far plane distances.
matrix44 PerspectiveMatrix44(float fovY, float aspect, float n, float f) 
{
  matrix44 ret;
  float angle;
  float cot;

  angle = fovY / 2.0F;
  angle = DegToRad( angle );

  cot = (float) cos(angle) / (float) sin(angle);

  ret[0][0] = cot / aspect;
  ret[0][1] = 0.0F;
  ret[0][2] = 0.0F;
  ret[0][3] = 0.0F;

  ret[1][0] = 0.0F;
  ret[1][1] = cot;
  ret[1][2] = 0.0F;
  ret[1][3] = 0.0F;

  ret[2][0] = 0.0F;
  ret[2][1] = 0.0F;
  ret[2][2] = -(f + n) / (f - n);
  ret[2][3] = -1.0F;


  ret[3][0] = 0.0F;
  ret[3][1] = 0.0F;
  ret[3][2] = -(2*f*n) / (f - n);
  ret[3][3] = 0.0F;

  return ret;
}

// Return an orthographic matrix44 given the left, right, bottom, top,
//   near, and far values for the frustum boundaries.
matrix44 OrthoMatrix44(float l, float r, 
    float b, float t, float n, float f) 
{
  matrix44 ret;
  float width = r-l;
  float height = t-b;
  float depth = f-n;

  ret[0][0] = 2.0F / width;
  ret[0][1] = 0.0F;
  ret[0][2] = 0.0F;
  ret[0][3] = 0.0F;

  ret[1][0] = 0.0F;
  ret[1][1] = 2.0F / height;
  ret[1][2] = 0.0F;
  ret[1][3] = 0.0F;

  ret[2][0] = 0.0F;
  ret[2][1] = 0.0F;
  ret[2][2] = -(2.0F) / depth;
  ret[2][3] = 0.0F;

  ret[3][0] = -(r + l) / width;
  ret[1][3] = -(t + b) / height;
  ret[3][2] = -(f + n) / depth;
  ret[3][3] = 1.0F;

  return ret;
}

// Return an orientation matrix using 3 basis normalized vectors
matrix44    OrthoNormalMatrix44(const vector3 &xdir, 
    const vector3 &ydir, const vector3 &zdir)
{
  matrix44 ret;

  ret[0] = (vector4)xdir;
  ret[1] = (vector4)ydir;
  ret[2] = (vector4)zdir;
  ret[3][3] = 1.0F;

  return ret;
}


////////////////////////////////////////////////////////////
// Debug functions
//

// Print a vector2 to a file
void vector2::fprint(FILE* file, char* str) const 
{
  fprintf(file, "%svector2: <%f, %f>\n", str, x, y);
}

// Print a vector3 to a file
void vector3::fprint(FILE* file, char* str) const 
{
  fprintf(file, "%svector3: <%f, %f, %f>\n", str, x, y, z);
}

// Print a vector4 to a file
void vector4::fprint(FILE* file, char* str) const 
{
  fprintf(file, "%svector4: <%f, %f, %f, %f>\n", str, x, y, z, w);
}

// Print a matrix33 to a file
void matrix33::fprint(FILE* file, char * str) const 
{
  fprintf(file, "%smatrix33:\n", str);
  vector3 row0(col[0][0], col[1][0], col[2][0]);
  row0.fprint(file, "\t");
  vector3 row1(col[0][1], col[1][1], col[2][1]);
  row1.fprint(file, "\t");
  vector3 row2(col[0][2], col[1][2], col[2][2]);
  row2.fprint(file, "\t");
}

// Print a matrix44 to a file
void matrix44::fprint(FILE* file, char* str) const 
{
  fprintf(file, "%smatrix44:\n", str);
  vector4 row0(col[0][0], col[1][0], col[2][0], col[3][0]);
  row0.fprint(file, "\t");
  vector4 row1(col[0][1], col[1][1], col[2][1], col[3][1]);
  row1.fprint(file, "\t");
  vector4 row2(col[0][2], col[1][2], col[2][2], col[3][2]);
  row2.fprint(file, "\t");
  vector4 row3(col[0][3], col[1][3], col[2][3], col[3][3]);
  row3.fprint(file, "\t");
}
