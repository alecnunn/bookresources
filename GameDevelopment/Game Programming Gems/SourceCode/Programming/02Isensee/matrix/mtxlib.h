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

#ifndef _MTXLIB_H
#define _MTXLIB_H
#include <cstdio>
#include <cmath>
#include <cassert>

static inline float DegToRad(float a) { return a*0.01745329252f;};
static inline float RadToDeg(float a) { return a*57.29577951f;};

class vector2;
class vector3;
class vector4;
class matrix33;
class matrix44;

////////////////////////////////////////////////////////////
// vector2 class
//

class vector2 
{
public:
  // Members
  float x, y;

public:
  // Constructors
  vector2() {};
  // Constructor with initializing float values
  vector2(float inX, float inY): x(inX), y(inY) {}
  // Constructor with initializing vector2
  vector2(const vector2 &v): x(v.x), y(v.y) {}

public:
  // Array indexing
  float                 &operator [] (unsigned int i) {
    assert (i<2);
    return *(&x+i);
  }
  // Array indexing
  const float           &operator [] (unsigned int i) const {
    assert (i<2);
    return *(&x+i);
  }
  // Add a vector2 to this one
  vector2               &operator += (const vector2 &v) {
    x += v.x;
    y += v.y;
    return *this;
  }
  // Subtract a vector2 from this one
  vector2               &operator -= (const vector2 &v) {
    x -= v.x;
    y -= v.y;
    return *this;
  }
  // Multiply the vector2 by a float
  vector2               &operator *= (float f) {
    x *= f;
    y *= f;
    return *this;
  }
  // Divide the vector2 by a float
  vector2               &operator /= (float f) {
    x /= f;
    y /= f;
    return *this;
  }
  // Are these two vector2's equal?
  friend bool           operator == (const vector2 &a, const vector2 &b) {
    return((a.x == b.x) && (a.y == b.y));
  }
  // Are these two vector2's not equal?
  friend bool           operator != (const vector2 &a, const vector2 &b) {
    return((a.x != b.x) || (a.y != b.y));
  }
  // Negate this vector
  friend vector2        operator - (const vector2 &a) {
    return vector2(-a.x, -a.y);
  }
  // Add two vector2's
  friend vector2        operator + (const vector2 &a, const vector2 &b) {
    vector2 ret(a);
    ret += b;
    return ret;
  }
  // Subtract one vector2 from another
  friend vector2        operator - (const vector2 &a, const vector2 &b) {
    vector2 ret(a);
    ret -= b;
    return ret;
  }
  // Multiply vector2 by a float
  friend vector2        operator * (const vector2 &v, float f) {
    return vector2(f * v.x, f * v.y);
  }
  // Multiply vector2 by a float
  friend vector2        operator * (float f, const vector2 &v) {
    return vector2(f * v.x, f * v.y);
  }
  // Divide vector2 by a float
  friend vector2        operator / (const vector2 &v, float f) {
    return vector2(v.x / f, v.y / f);
  }

public:
  // Methods
  // Set Values
  void                  set(float xIn, float yIn) {
    x = xIn;
    y = yIn;
  }
  // Get length of a vector2
  float                 length() const {
    return(float) sqrt(x*x + y*y);
  }
  // Get squared length of a vector2
  float                 lengthSqr() const {
    return(x*x + y*y);
  }
  // Does vector2 equal (0, 0)?
  bool                  isZero() const {
    return((x == 0.0F) && (y == 0.0F));
  }
  // Normalize a vector2
  vector2               &normalize() {
    float m = length();

    if (m > 0.0F)
      m = 1.0F / m;
    else
      m = 0.0F;
    x *= m;
    y *= m;

    return *this;
  }

  // Debug
  void                  fprint(FILE* file, char* str) const;
};


////////////////////////////////////////////////////////////
// vector3 class
//

class vector3 
{
public:
  // Members
  float x, y, z;

public:
  // Constructors
  vector3() {};
  // Constructor with initializing float values
  vector3(float inX, float inY, float inZ): x(inX), y(inY), z(inZ) {}
  // Constructor with initializing vector3
  vector3(const vector3 &v): x(v.x), y(v.y), z(v.z) {}
  // Constructor with initializing vector2
  explicit vector3(const vector2 &v): x(v.x), y(v.y), z(0.0F) {}
  // Constructor with initializing vector4
  // TODO
  explicit vector3(const vector4 &v);

public:
  // Operators
  // Array indexing
  float                 &operator [] (unsigned int i) {
    assert (i<3);
    return *(&x+i);
  }
  // Array indexing
  const float           &operator [] (unsigned int i) const {
    assert (i<3);
    return *(&x+i);
  }
  // Assign from a vector2
  vector3               &operator =  (const vector2 &v) {
    x = v.x;
    y = v.y;
    z = 0.0F;
    return *this;
  }
  // Add a vector3 to this one
  vector3               &operator += (const vector3 &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }
  // Subtract a vector3 from this one
  vector3               &operator -= (const vector3 &v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }
  // Multiply the vector3 by a float
  vector3               &operator *= (float f) {
    x *= f;
    y *= f;
    z *= f;
    return *this;
  }
  // Divide the vector3 by a float
  vector3               &operator /= (float f) {
    x /= f;
    y /= f;
    z /= f;
    return *this;
  }
  // Are these two vector3's equal?
  friend bool           operator == (const vector3 &a, const vector3 &b) {
    return((a.x == b.x) && (a.y == b.y) && (a.z == b.z));
  }
  // Are these two vector3's not equal?
  friend bool           operator != (const vector3 &a, const vector3 &b) {
    return((a.x != b.x) || (a.y != b.y) || (a.z != b.z));
  }
  // Negate a vector3
  friend vector3        operator - (const vector3 &a) {
    return vector3(-a.x, -a.y, -a.z);
  }
  // Add two vector3's
  friend vector3        operator + (const vector3 &a, const vector3 &b) {
    vector3 ret(a);
    ret += b;
    return ret;
  }
  // Subtract one vector3 from another
  friend vector3        operator - (const vector3 &a, const vector3 &b) {
    vector3 ret(a);
    ret -= b;
    return ret;
  }
  // Multiply vector3 by a float
  friend vector3        operator * (const vector3 &v, float f) {
    return vector3(f * v.x, f * v.y, f * v.z);
  }
  // Multiply vector3 by a float
  friend vector3        operator * (float f, const vector3 &v) {
    return vector3(f * v.x, f * v.y, f * v.z);
  }
  // Divide vector3 by a float
  friend vector3        operator / (const vector3 &v, float f) {
    return vector3(v.x / f, v.y / f, v.z / f);
  }

public:
  // Methods
  // Set Values
  void                  set(float xIn, float yIn, float zIn) {
    x = xIn;
    y = yIn;
    z = zIn;
  }
  // Get length of a vector3
  float                 length() const {
    return(float) sqrt(x*x + y*y + z*z);
  }
  // Get squared length of a vector3
  float                 lengthSqr() const {
    return(x*x + y*y + z*z);
  }
  // Does vector3 equal (0, 0, 0)?
  bool                  isZero() const {
    return((x == 0.0F) && (y == 0.0F) && (z == 0.0F));
  }
  // Normalize a vector3
  vector3               &normalize() {
    float m = length();
    if (m > 0.0F)
      m = 1.0F / m;
    else
      m = 0.0F;
    x *= m;
    y *= m;
    z *= m;
    return *this;
  }

  // Debug
  void                  fprint(FILE* file, char* str) const;
};


////////////////////////////////////////////////////////////
// vector4 class
//

class vector4 
{
public:
  // Members
  float x, y, z, w;

public:
  // Constructors
  // vector4(): x(0), y(0), z(0), w(0) {};
  vector4() {};
  // Constructor with initializing float values
  vector4(float inX, float inY, float inZ, float inW): x(inX), y(inY), z(inZ), w(inW) {};
  // Constructor with initializing vector4
  vector4(const vector4 &v): x(v.x), y(v.y), z(v.z), w(v.w) {};
  // Constructor with initializing vector3
  explicit vector4(const vector3 &v): x(v.x), y(v.y), z(v.z), w(0.0F) {};
  // Constructor with initializing vector2
  explicit vector4(const vector2 &v): x(v.x), y(v.y), z(0.0F), w(0.0F) {};

public:
  // Operators
  // Array indexing
  float                 &operator [] (unsigned int i) {
    assert (i<4);
    //return *(&x+i);
    return(i == 0) ? x : (i == 1) ? y : (i == 2) ? z : w;
  }
  // Array indexing
  const float           &operator [] (unsigned int i) const {
    assert (i<4);
    //return *(&x+i);
    return(i == 0) ? x : (i == 1) ? y : (i == 2) ? z : w;
  }
  // Assign from a vector3
  vector4               &operator =  (const vector3 &v) { 
    x = v.x;
    y = v.y;
    z = v.z;
    w = 0.0F;
    return *this;
  }
  // Assign from a vector2
  vector4               &operator =  (const vector2 &v) {
    x = v.x;
    y = v.y;
    z = 0.0F;
    w = 0.0F;
    return *this;
  }
  // Add a vector4 to this one
  vector4               &operator += (const vector4 &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
  }
  // Subtract a vector4 from this one
  vector4               &operator -= (const vector4 &v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
  }
  // Multiply the vector4 by a float
  vector4               &operator *= (float f) {
    x *= f;
    y *= f;
    z *= f;
    w *= f;
    return *this;
  }
  // Divide the vector4 by a float
  vector4               &operator /= (float f) {
    x /= f;
    y /= f;
    z /= f;
    w /= f;
    return *this;
  }
  // Are these two vector4's equal?
  friend bool           operator == (const vector4 &a, const vector4 &b) {
    return((a.x == b.x) && (a.y == b.y) &&
        (a.z == b.z) && (a.w == b.w));
  }
  // Are these two vector4's not equal?
  friend bool           operator != (const vector4 &a, const vector4 &b) {
    return((a.x != b.x) || (a.y != b.y) ||
        (a.z != b.z) || (a.w != b.w));
  }
  // Negate a vector4
  friend vector4        operator - (const vector4 &a) {
    return vector4(-a.x, -a.y, -a.z, -a.w);
  }
  // Add two vector4's
  friend vector4        operator + (const vector4 &a, const vector4 &b) {
    vector4 ret(a);
    ret += b;
    return ret;
  }
  // Subtract one vector4 from another
  friend vector4        operator - (const vector4 &a, const vector4 &b) {
    vector4 ret(a);
    ret -= b;
    return ret;
  }
  // Multiply vector4 by a float
  friend vector4        operator * (const vector4 &v, float f) {
    return vector4(f * v.x, f * v.y, f * v.z, f * v.w);
  }
  // Multiply vector4 by a float
  friend vector4        operator * (float f, const vector4 &v) {
    return vector4(f * v.x, f * v.y, f * v.z, f * v.w);
  }
  // Divide vector4 by a float
  friend vector4        operator / (const vector4 &v, float f) {
    return vector4(v.x / f, v.y / f, v.z / f, v.w / f);
  }

public:
  // Methods
  // Set Values
  void                  set(float xIn, float yIn, float zIn, float wIn) {
    x = xIn;
    y = yIn;
    z = zIn;
    w = wIn;
  }
  // Get length of a vector4
  float                 length() const {
    return(float) sqrt(x*x + y*y + z*z + w*w);
  }
  // Get squared length of a vector4
  float                 lengthSqr() const {
    return(x*x + y*y + z*z + w*w);
  }
  // Does vector4 equal (0, 0, 0, 0)?
  bool                  isZero() const {
    return((x == 0.0F) && (y == 0.0F) && (z == 0.0F) && (w == 0.0F));
  }
  // Normalize a vector4
  vector4               &normalize() {
    float m = length();
    if (m > 0.0F)
      m = 1.0F / m;
    else
      m = 0.0F;
    x *= m;
    y *= m;
    z *= m;
    w *= m;
    return *this;
  }

  // Debug
  void                  fprint(FILE* file, char* str) const;
};


////////////////////////////////////////////////////////////
// Miscellaneous vector functions
//

vector2 Normalized(const vector2 &a);
vector3 Normalized(const vector3 &a);
vector4 Normalized(const vector4 &a);
float DotProduct(const vector2 &a, const vector2 &b);
float DotProduct(const vector3 &a, const vector3 &b);
float DotProduct(const vector4 &a, const vector4 &b);
void SwapVec(vector2 &a, vector2 &b);
void SwapVec(vector3 &a, vector3 &b);
void SwapVec(vector4 &a, vector4 &b);
vector3 CrossProduct(const vector3 &a, const vector3 &b);
bool NearlyEquals(const vector2 &a, const vector2 &b, float r);
bool NearlyEquals(const vector3 &a, const vector3 &b, float r);
bool NearlyEquals(const vector4 &a, const vector4 &b, float r);


////////////////////////////////////////////////////////////
// matrix33 class
//

class matrix33 
{
public:
  // Members
  vector3       col[3];

public:
  // Constructors
  matrix33() {};
  // Constructor with initializing value
  matrix33(float v) {
    col[0].set(v, v, v);
    col[1].set(v, v, v);
    col[2].set(v, v, v);
  }
  // Constructor with initializing matrix33
  matrix33(const matrix33 &m) {
    col[0] = m[0];
    col[1] = m[1];
    col[2] = m[2];
  }
  // Constructor with initializing vector3's
  matrix33(const vector3 &v0, const vector3 &v1, const vector3 &v2) {
    col[0] = v0;
    col[1] = v1;
    col[2] = v2;
  }

public:
  // Operators
  // Array indexing
  vector3       &operator [] (unsigned int i) {
    assert (i<3);
    return(vector3&)col[i];
  }
  // Array indexing
  const vector3     &operator [] (unsigned int i) const {
    assert (i<3);
    return(vector3&)col[i];
  }
  // Assign
  matrix33      &operator =  (const matrix33 &m) {
    col[0] = m[0];
    col[1] = m[1];
    col[2] = m[2];
    return *this;
  }
  // Add a matrix33 to this one
  matrix33      &operator += (const matrix33 &m) {
    col[0] += m[0];
    col[1] += m[1];
    col[2] += m[2];
    return *this;
  }
  // Subtract a matrix33 from this one
  matrix33      &operator -= (const matrix33 &m) {
    col[0] -= m[0];
    col[1] -= m[1];
    col[2] -= m[2];
    return *this;
  }
  // Multiply the matrix33 by another matrix33
  matrix33      &operator *= (const matrix33 &m);
  // Multiply the matrix33 by a float
  matrix33      &operator *= (float f) {
  }
  // Multiply the matrix33 by a float
  matrix33      &operator /= (float f) {
    col[0] *= f;
    col[1] *= f;
    col[2] *= f;
    return *this;
  }
  // Are these two matrix33's equal?
  friend bool       operator == (const matrix33 &a, const matrix33 &b) {
    return((a[0] == b[0]) && (a[1] == b[1]) && (a[2] == b[2]));
  }
  // Are these two matrix33's not equal?
  friend bool       operator != (const matrix33 &a, const matrix33 &b) {
    return((a[0] != b[0]) || (a[1] != b[1]) || (a[2] != b[2]));
  }
  // Add two matrix33's
  friend matrix33   operator + (const matrix33 &a, const matrix33 &b) {
    matrix33 ret(a);
    ret += b;
    return ret;
  }
  // Subtract one matrix33 from another
  friend matrix33   operator - (const matrix33 &a, const matrix33 &b) {
    matrix33 ret(a);
    ret -= b;
    return ret;
  }
  // Multiply matrix33 by another matrix33
  friend matrix33   operator * (const matrix33 &a, const matrix33 &b) {
    matrix33 ret(a);
    ret *= b;
    return ret;
  }
  // Multiply a vector3 by this matrix33
  friend vector3    operator * (const matrix33 &m, const vector3 &v) {
    vector3 ret;
    ret.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0];
    ret.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1];
    ret.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2];
    return ret;
  }
  // Multiply a vector3 by this matrix33
  friend vector3    operator * (const vector3 &v, const matrix33 &m) {
    vector3 ret;
    ret.x = DotProduct(m[0], v);
    ret.y = DotProduct(m[1], v);
    ret.z = DotProduct(m[2], v);
    return ret;
  }
  // Multiply matrix33 by a float
  friend matrix33   operator * (const matrix33 &m, float f) {
    matrix33 ret(m);
    ret *= f;
    return ret;
  }
  // Multiply matrix33 by a float
  friend matrix33   operator * (float f, const matrix33 &m) {
    matrix33 ret(m);
    ret *= f;
    return ret;
  }

public:
  // Methods
  // Set matrix33 to the identity matrix
  matrix33      &identity() {
    col[0].set(1.0, 0.0, 0.0);
    col[1].set(0.0, 1.0, 0.0);
    col[2].set(0.0, 0.0, 1.0);
    return *this;
  }
  // Transpose the matrix33
  matrix33      &transpose();
  // Invert the matrix33
  matrix33      &invert();

  // Debug
  void          fprint(FILE* file, char* str) const;
};

matrix33    IdentityMatrix33();
matrix33    TransposeMatrix33(const matrix33 &m);
matrix33    InvertMatrix33(const matrix33 &m);
matrix33    RotateRadMatrix33(float rad);
matrix33    TranslateMatrix33(float x, float y);
matrix33    ScaleMatrix33(float x, float y, float z = 1.0);


////////////////////////////////////////////////////////////
// matrix44 class
//

class matrix44 
{
public:
  // Members
  vector4   col[4];

public:
  // Constructors
  matrix44() {};
  // Constructor with initializing value
  matrix44(float v) {
    col[0].set(v, v, v, v);
    col[1].set(v, v, v, v);
    col[2].set(v, v, v, v);
    col[3].set(v, v, v, v);
  }
  // Constructor with initializing matrix44
  matrix44(const matrix44 &m) {
    col[0] = m[0];
    col[1] = m[1];
    col[2] = m[2];
    col[3] = m[3];
  }
  // Constructor with initializing vector4's
  matrix44(const vector4 &v0, const vector4 &v1, 
      const vector4 &v2, const vector4 &v3) {
    col[0] = v0;
    col[1] = v1;
    col[2] = v2;
    col[3] = v3;
  }
  // Constructor with initializing matrix33
  explicit matrix44(const matrix33 &m) {
    col[0] = m[0];
    col[1] = m[1];
    col[2] = m[2];
    col[3].set(0.0, 0.0, 0.0, 1.0);
  }

public:
  // Operators
  // Array indexing
  vector4       &operator [] (unsigned int i) {
    assert (i<4);
    return col[i];
  }
  // Array indexing
  const vector4     &operator [] (unsigned int i) const {
    assert (i<4);
    return col[i];
  }
  // Assign
  matrix44      &operator =  (const matrix44 &m) {
    col[0] = m[0];
    col[1] = m[1];
    col[2] = m[2];
    col[3] = m[3];
    return *this;
  }
  // Assign a matrix33 to the matrix44
  matrix44      &operator =  (const matrix33 &m) {
    col[0] = m[0];
    col[1] = m[1];
    col[2] = m[2];
    col[3].set(0.0, 0.0, 0.0, 1.0);
    return *this;
  }
  // Add a matrix44 to this one
  matrix44      &operator += (const matrix44 &m) {
    col[0] += m[0];
    col[1] += m[1];
    col[2] += m[2];
    col[3] += m[3];
    return *this;
  }
  // Subtract a matrix44 from this one
  matrix44      &operator -= (const matrix44 &m) {
    col[0] -= m[0];
    col[1] -= m[1];
    col[2] -= m[2];
    col[3] -= m[3];
    return *this;
  }
  // Multiply the matrix44 by another matrix44
  matrix44      &operator *= (const matrix44 &m);
  // Multiply the matrix44 by a float
  matrix44      &operator *= (float f) {
    col[0] *= f;
    col[1] *= f;
    col[2] *= f;
    col[3] *= f;
    return *this;
  }
  //matrix44      &operator /= (float f) {}
  // Are these two matrix44's equal?
  friend bool       operator == (const matrix44 &a, const matrix44 &b) {
    return((a[0] == b[0]) && (a[1] == b[1]) &&
        (a[2] == b[2]) && (a[3] == b[3]));
  }
  // Are these two matrix44's not equal?
  friend bool       operator != (const matrix44 &a, const matrix44 &b) {
    return((a[0] != b[0]) || (a[1] != b[1]) ||
        (a[2] != b[2]) || (a[3] != b[3]));
  }
  // Add two matrix44's
  friend matrix44   operator + (const matrix44 &a, const matrix44 &b) {
    matrix44 ret(a);
    ret += b;
    return ret;
  }
  // Subtract one matrix44 from another
  friend matrix44   operator - (const matrix44 &a, const matrix44 &b) {
    matrix44 ret(a);
    ret -= b;
    return ret;
  }
  // Multiply matrix44 by another matrix44
  friend matrix44   operator * (const matrix44 &a, const matrix44 &b) {
    matrix44 ret(a);
    ret *= b;
    return ret;
  }
  // Multiply a vector3 by this matrix44
  friend vector3    operator * (const matrix44 &m, const vector3 &v) {
    vector4 ret(v);
    ret = m * ret;
    return vector3(ret.x, ret.y, ret.z);
  }
  // Multiply a vector3 by this matrix44
  friend vector3    operator * (const vector3 &v, const matrix44 &m) {
    vector4 ret(v);
    ret = ret * m;
    return vector3(ret.x, ret.y, ret.z);
  }
  // Multiply a vector4 by this matrix44
  friend vector4    operator * (const matrix44 &m, const vector4 &v) {
    vector4 ret;
    ret.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + v.w * m[3][0];
    ret.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + v.w * m[3][1];
    ret.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + v.w * m[3][2];
    ret.w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + v.w * m[3][3];
    return ret;
  }
  // Multiply a vector4 by this matrix44
  friend vector4    operator * (const vector4 &v, const matrix44 &m) {
    vector4 ret;
    ret.x = DotProduct(m[0], v);
    ret.y = DotProduct(m[1], v);
    ret.z = DotProduct(m[2], v);
    ret.w = DotProduct(m[3], v);
    return ret;
  }
  // Multiply matrix44 by a float
  friend matrix44   operator * (const matrix44 &m, float f) {
    matrix44 ret(m);
    ret *= f;
    return ret;
  }
  // Set matrix44 to the identity matrix
  friend matrix44   operator * (float f, const matrix44 &m) {
    matrix44 ret(m);
    ret *= f;
    return ret;
  }

public:
  // Methods
  // Set matrix44 to the identity matrix
  matrix44      &identity() {
    col[0].set(1.0, 0.0, 0.0, 0.0);
    col[1].set(0.0, 1.0, 0.0, 0.0);
    col[2].set(0.0, 0.0, 1.0, 0.0);
    col[3].set(0.0, 0.0, 0.0, 1.0);
    return *this;
  }
  // Transpose the matrix44
  matrix44      &transpose();
  // Invert the matrix44
  matrix44      &invert();

  // Debug
  void          fprint(FILE* file, char* str) const;
};

matrix44    IdentityMatrix44();
matrix44    TransposeMatrix44(const matrix44 &m);
matrix44    InvertMatrix44(const matrix44 &m);
matrix44    RotateRadMatrix44(char axis, float rad);
matrix44    RotateRadMatrix44(const vector3 &axis, float rad);
matrix44    TranslateMatrix44(float x, float y, float z);
matrix44    ScaleMatrix44(float x, float y, float z, float w = 1.0);
matrix44    LookAtMatrix44(const vector3 &camPos, const vector3 &camUp, 
    const vector3 &target );
matrix44    FrustumMatrix44(float l, float r, float b, float t, 
    float n, float f);
matrix44    PerspectiveMatrix44(float fovY, float aspect, 
    float n, float f);
matrix44    OrthoMatrix44(float l, float r, float b, float t, 
    float n, float f);
matrix44    OrthoNormalMatrix44(const vector3 &xdir, 
    const vector3 &ydir, const vector3 &zdir);

#endif
