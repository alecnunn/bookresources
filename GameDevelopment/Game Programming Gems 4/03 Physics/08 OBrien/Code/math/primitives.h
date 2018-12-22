/*********************************************************************
 * primitives.h
 * Authored by Kris Hauser 2002-2003
 *
 * Mostly-inlined math primitives.
 *
 * Copyright (c) 2002,2003 SquireSoft, used with permission.
 *
 *********************************************************************/

/**************************************************************************
 * Copyright (c) 2003, University of California at Berkeley
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 * 
 *     * Neither the name of the University of California nor the names
 *       of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written
 *       permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/



#ifndef MATH_PRIMITIVES_H
#define MATH_PRIMITIVES_H

#include <math.h>
#include <float.h>
#include <stdio.h>

/////////////////// Common math typedefs, constants, functions ////////////////////////////

#ifdef MATH_DOUBLE
typedef double Real;

const static Real Zero = 0.0;
const static Real One = 1.0;
const static Real Two = 2.0;
const static Real Half = 0.5;
const static Real Pi = 3.1415926;
const static Real Epsilon = 1e-6;
const static Real Inf = DBL_MAX;
const static Real DtoRConst = 0.0174532925;
const static Real RtoDConst = 57.29577951;

//math.h aliases
inline Real Abs(Real x) { return fabs(x); }
inline Real Sqr(Real x) { return x*x; }
inline Real Sqrt(Real x) { return sqrt(x); }
inline Real Exp(Real x) { return exp(x); }
inline Real Log(Real x) { return log(x); }
inline Real Pow(Real x, Real y) { return pow(x,y); }
inline Real Sin(Real x) { return sin(x); }
inline Real Cos(Real x) { return cos(x); }
inline Real Tan(Real x) { return tan(x); }
inline Real Asin(Real x) { return asin(x); }
inline Real Acos(Real x) { return acos(x); }
inline Real Atan(Real x) { return atan(x); }
inline Real Atan2(Real y, Real x) { return atan2(y,x); }
inline Real Floor(Real x) { return floor(x); }
inline Real Ceil(Real x) { return ceil(x); }

#else
typedef float Real;

const static Real Zero = 0.0f;
const static Real One = 1.0f;
const static Real Two = 2.0f;
const static Real Half = 0.5f;
const static Real Pi = 3.1415926f;
const static Real Epsilon = 1e-5f;
const static Real Inf = FLT_MAX;
const static Real DtoRConst = 0.0174532925f;
const static Real RtoDConst = 57.29577951f;

//math.h aliases
inline Real Abs(Real x) { return fabsf(x); }
inline Real Sqr(Real x) { return x*x; }
inline Real Sqrt(Real x) { return sqrtf(x); }
inline Real Exp(Real x) { return expf(x); }
inline Real Log(Real x) { return logf(x); }
inline Real Pow(Real x, Real y) { return powf(x,y); }
inline Real Sin(Real x) { return sinf(x); }
inline Real Cos(Real x) { return cosf(x); }
inline Real Tan(Real x) { return tanf(x); }
inline Real Asin(Real x) { return asinf(x); }
inline Real Acos(Real x) { return acosf(x); }
inline Real Atan(Real x) { return atanf(x); }
inline Real Atan2(Real y, Real x) { return atan2f(y,x); }
inline Real Floor(Real x) { return floorf(x); }
inline Real Ceil(Real x) { return ceilf(x); }

#endif  // MATH_DOUBLE

inline Real Inv(Real x) { return One/x; }
inline Real Sign(Real x) { return (x>Zero? One : (x<Zero? -One : Zero)); }
inline Real Min(Real a, Real b) { return (a<b?a:b); }
inline Real Max(Real a, Real b) { return (a>b?a:b); }
inline Real Clamp(Real x, Real a, Real b) { return (x<a? a : (x>b? b : x)); }

inline bool FuzzyEquals(Real a, Real b, Real eps = Epsilon) { return Abs(a-b) <= eps; }

//Kronecker delta
inline Real Delta(int i, int j) { return (i==j? One : Zero); }

//degree/radian conversion
inline float DtoR(Real f) { return f*DtoRConst; }
inline float RtoD(Real f) {	return f*RtoDConst; }



/////////////////// Class declarations for useful math types ////////////////////////////

class Vector3;
class Vector4;
class Matrix3;
class Matrix4;
class RigidTransform;
class Complex;
class Quaternion;

/**********************************************************************
 * Format:
 * class ClassName
 * {
 *   ClassName()		constructors (WARNING default should not initialize anything -- not zero!)
 *   operator*=()		inplace operators
 *   operator Other()	cast operators
 *   op()				efficient operator methods in the form this=arg1 op arg2 ...
 *   setX()				initializers
 *   getX()				extractors
 *   inplaceX()			inplace modifiers
 *   load/save()		binary file io
 *   otherMethods()		other methods
 *
 *   attributes
 * }
 * external functions
 **********************************************************************/

class Vector3
{
 public:
  Vector3();
  Vector3(const Vector3&);
  Vector3(Real x);
  Vector3(Real x, Real y, Real z);
  Vector3(const Real data[3]);

  inline bool operator == (const Vector3&) const;
  inline bool operator != (const Vector3&) const;
  inline const Vector3& operator = (const Vector3&);
  inline const Vector3& operator = (const Real [3]);
  inline void operator += (const Vector3&);
  inline void operator -= (const Vector3&);
  inline void operator *= (Real);
  inline void operator /= (Real);
  inline Real& operator [] (int);
  inline const Real& operator [] (int) const;
  inline operator Real* ();
  inline operator const Real* () const;

  inline void add(const Vector3& a, const Vector3& b);
  inline void sub(const Vector3& a, const Vector3& b);
  inline void mul(const Vector3& a, Real);
  inline void div(const Vector3& a, Real);
  inline void madd(const Vector3& b, Real);
  inline Real dot(const Vector3& a) const;

  inline void set(const Vector3&);
  inline void set(Real x);
  inline void set(Real x, Real y, Real z);
  inline void set(const Real data[3]);
  inline void setZero();
  inline void setNegative(const Vector3&);
  inline void setCross(const Vector3&, const Vector3&);
  inline void setNormalized(const Vector3&);
  inline void setProjection(const Vector3&, const Vector3&);

  //For compatibility with james' code
  inline void minSet(const Vector3& a) { if(a.x < x) x = a.x;  if(a.y < y) y = a.y;  if(a.z < z) z = a.z; }
  inline void maxSet(const Vector3& a) { if(a.x > x) x = a.x;  if(a.y > y) y = a.y;  if(a.z > z) z = a.z; }

  inline void get(Vector3&) const;
  inline void get(Real& x, Real& y, Real& z) const;
  inline void get(Real data[3]) const;
  inline void getNegative(Vector3&) const;
  inline void getNormalized(Vector3&) const;

  inline void inplaceNegative();
  inline void inplaceScale(Real);
  inline void inplaceNormalize();

  inline Real norm() const;
  inline Real normSquared() const;
  inline Real length() const;		// = norm

  union {
    Real data[3];
    struct { Real x,y,z; };
  };
};

inline Real dot(const Vector3& a, const Vector3& b);
inline Vector3 cross(const Vector3& a, const Vector3& b);
inline void normalize(Vector3& a);
inline Vector3 project(const Vector3& a, const Vector3& b);
inline Vector3 operator - (const Vector3& a);
inline Vector3 operator + (const Vector3& a, const Vector3& b);
inline Vector3 operator - (const Vector3& a, const Vector3& b);
inline Vector3 operator * (const Vector3& a, Real b);
inline Vector3 operator * (Real a, const Vector3& b);
inline Vector3 operator / (const Vector3& a, Real b);

//Added for compatibility with james' SM stuff 2/10/03
inline Vector3 operator * (const Vector3& a, const Vector3& b);


class Vector4
{
 public:
  Vector4();
  Vector4(const Vector4&);
  Vector4(Real x);
  Vector4(Real x, Real y, Real z, Real w = One);
  Vector4(const Real data[4]);
  Vector4(const Vector3&);		//point in homogeneous coords

  inline bool operator == (const Vector4&) const;
  inline bool operator != (const Vector4&) const;
  inline const Vector4& operator = (const Vector4&);
  inline const Vector4& operator = (const Real [4]);
  inline void operator += (const Vector4&);
  inline void operator -= (const Vector4&);
  inline void operator *= (Real);
  inline void operator /= (Real);
  inline void operator += (const Vector3&);
  inline void operator -= (const Vector3&);
  inline Real& operator [] (int);
  inline const Real& operator [] (int) const;
  inline operator Vector3() const;
  inline operator Real* ();
  inline operator const Real* () const;

  inline void add(const Vector4& a, const Vector4& b);
  inline void sub(const Vector4& a, const Vector4& b);
  inline void mul(const Vector4& a, Real);
  inline void div(const Vector4& a, Real);
  inline void madd(const Vector4& a, Real);
  inline void add(const Vector4& a, const Vector3& b);
  inline void sub(const Vector4& a, const Vector3& b);
  inline Real dot(const Vector4& a) const;
  inline Real dot3(const Vector4& a) const;
  inline Real dot3(const Vector3& a) const;

  inline void set(const Vector4&);
  inline void set(Real x);
  inline void set(Real x, Real y, Real z, Real w = One);
  inline void set(const Real data[4]);
  inline void setZero();
  inline void setVector(const Vector3&);			//w = 0
  inline void setHomogeneous(const Vector3&);		//w = 1
  inline void setNegative(const Vector4&);
  inline void setNormalized(const Vector4&);
  inline void setProjection(const Vector4&, const Vector4&);

  inline void get(Vector4&) const;
  inline void get(Real& x, Real& y, Real& z, Real& w) const;
  inline void get(Real data[4]) const;
  inline void get(Vector3&) const;
  inline void getNegative(Vector4&) const;
  inline void getNormalized(Vector4&) const;

  inline void inplaceNegative();
  inline void inplaceScale(Real);
  inline void inplaceNormalize();

  inline Real norm() const;
  inline Real normSquared() const;
  inline Real length() const;		// = norm

  union {
    Real data[4];
    struct { Real x,y,z,w; };
  };
};

inline Real dot(const Vector4& a, const Vector4& b);
inline Real dot3(const Vector4& a, const Vector4& b);
inline Real dot3(const Vector4& a, const Vector3& b);
inline Real dot3(const Vector3& a, const Vector4& b);
inline void normalize(Vector4& a);
inline Vector4 project(const Vector4& a, const Vector4& b);
inline Vector4 planeProject(const Vector4& a, const Vector4& b);
inline Vector4 operator - (const Vector4& a);
inline Vector4 operator + (const Vector4& a, const Vector4& b);
inline Vector4 operator - (const Vector4& a, const Vector4& b);
inline Vector4 operator * (const Vector4& a, Real b);
inline Vector4 operator * (Real a, const Vector4& b);
inline Vector4 operator / (const Vector4& a, Real b);


//WARNING: data is in column major format, but (i,j) operator acts as normal!!!
class Matrix3
{
 public:
  Matrix3();
  Matrix3(const Matrix3&);
  Matrix3(Real);
  Matrix3(const Real [3][3]);
  Matrix3(const Real [9]);
  Matrix3(const Vector3& xb, const Vector3& yb, const Vector3& zb);

  bool operator == (const Matrix3&) const;
  bool operator != (const Matrix3&) const;
  inline const Matrix3& operator  = (const Matrix3&);
  inline void operator += (const Matrix3&);
  inline void operator -= (const Matrix3&);
  inline void operator *= (const Matrix3&);
  operator const Real*() const { return &data[0][0]; }
  inline Real& operator () (int i, int j) { return data[j][i]; }
  inline const Real& operator () (int i, int j) const { return data[j][i]; }

  //matrix ops (this = a op b)
  inline void add(const Matrix3& a, const Matrix3& b);
  inline void sub(const Matrix3& a, const Matrix3& b);
  void mul(const Matrix3& a, const Matrix3& b);

  //scalar ops (this = a op b)
  inline void mul(const Matrix3& a, Real b);
  inline void div(const Matrix3& a, Real b);

  //vector ops (out = this op a
  inline void mul(const Vector3& a, Vector3& out) const;			//result is incorrect if out = a
  inline void mulTranspose(const Vector3& a, Vector3& out) const;	//result is incorrect if out = a

  inline void set(const Matrix3&);
  inline void set(Real);
  inline void set(const Real [3][3]);
  inline void set(const Real [9]);
  inline void set(const Vector3& xb, const Vector3& yb, const Vector3& zb);
  inline void setZero();
  inline void setIdentity();
  inline void setTranspose(const Matrix3&);
  inline void setNegative(const Matrix3&);
  bool setInverse(const Matrix3&);
  inline void setScale(Real s);
  inline void setScale(Real sx, Real sy, Real sz);
  inline void setCrossProduct(const Vector3&);
  inline void setRotateX(Real rads);
  inline void setRotateY(Real rads);
  inline void setRotateZ(Real rads);

  inline void get(Matrix3&) const;
  inline void get(Real [3][3]) const;
  inline void get(Real [9]) const;
  inline void get(Vector3& xb, Vector3& yb, Vector3& zb) const;
  inline void getTranspose(Matrix3&) const;
  inline void getNegative(Matrix3&) const;
  inline bool getInverse(Matrix3&) const;
  inline void getCrossProduct(Vector3&) const;

  inline void inplaceTranspose();
  inline void inplaceNegative();
  inline void inplaceInverse();
  inline void inplaceScale(Real s);
  inline void inplaceScale(Real sx, Real sy, Real sz);

  inline Real* col(int j) { return data[j]; }
  inline Real* col1() { return data[0]; }
  inline Real* col2() { return data[1]; }
  inline Real* col3() { return data[2]; }
  inline const Real* col(int j) const { return data[j]; }
  inline const Real* col1() const { return data[0]; }
  inline const Real* col2() const { return data[1]; }
  inline const Real* col3() const { return data[2]; }

  inline void setCol(int j, const Vector3& v) { v.get(data[j]); }
  inline void setCol1(const Vector3& v) { v.get(data[0]); }
  inline void setCol2(const Vector3& v) { v.get(data[1]); }
  inline void setCol3(const Vector3& v) { v.get(data[2]); }

  inline void setRow(int i, const Vector3& v) { v.get(data[0][i], data[1][i], data[2][i]); }
  inline void setRow1(const Vector3& v) { v.get(data[0][0], data[1][0], data[2][0]); }
  inline void setRow2(const Vector3& v) { v.get(data[0][1], data[1][1], data[2][1]); }
  inline void setRow3(const Vector3& v) { v.get(data[0][2], data[1][2], data[2][2]); }

  inline void getCol(int j, Vector3& v) const { v.set(data[j]); }
  inline void getCol1(Vector3& v) const { v.set(data[0]); }
  inline void getCol2(Vector3& v) const { v.set(data[1]); }
  inline void getCol3(Vector3& v) const { v.set(data[2]); }

  inline void getRow(int i, Vector3& v) const { v.set(data[0][i], data[1][i], data[2][i]); }
  inline void getRow1(Vector3& v) const { v.set(data[0][0], data[1][0], data[2][0]); }
  inline void getRow2(Vector3& v) const { v.set(data[0][1], data[1][1], data[2][1]); }
  inline void getRow3(Vector3& v) const { v.set(data[0][2], data[1][2], data[2][2]); }

  inline Vector3 getXBasis() const { return Vector3(col1()); }
  inline Vector3 getYBasis() const { return Vector3(col2()); }
  inline Vector3 getZBasis() const { return Vector3(col3()); }

  Real cofactor(int i, int j) const;
  inline bool isInvertible() const;
  Real determinant() const;
  inline Real trace() const;

  //column major format
  Real data[3][3];
};

inline Matrix3 transpose(const Matrix3&);
inline Real determinant(const Matrix3&);
inline Real trace(const Matrix3&);
inline Matrix3 operator + (const Matrix3& a, const Matrix3& b);
inline Matrix3 operator - (const Matrix3& a, const Matrix3& b);
inline Matrix3 operator * (const Matrix3& a, const Matrix3& b);
inline Vector3 operator * (const Matrix3& a, const Vector3& b);
inline Vector3 operator * (const Vector3& a, const Matrix3& b);

class Matrix4
{
 public:
  Matrix4();
  Matrix4(const Matrix4&);
  Matrix4(Real);
  Matrix4(const Real [4][4]);
  Matrix4(const Real [16]);
  Matrix4(const Vector3& xb, const Vector3& yb, const Vector3& zb, const Vector3& trans);
  Matrix4(const Vector4& x, const Vector4& y, const Vector4& z, const Vector4& w);
  Matrix4(const Matrix3&);
  Matrix4(const Matrix3&, const Vector3& trans);
  Matrix4(const Vector3& trans);

  bool operator == (const Matrix4&) const;
  bool operator != (const Matrix4&) const;
  inline const Matrix4& operator  = (const Matrix4&);
  inline void operator += (const Matrix4&);
  inline void operator -= (const Matrix4&);
  inline void operator *= (const Matrix4&);
  operator Matrix3() const;
  inline operator const Real*() const { return &data[0][0]; }
  inline Real& operator () (int i, int j) { return data[j][i]; }
  inline const Real& operator () (int i, int j) const { return data[j][i]; }

  //matrix ops (this = a op b)
  inline void add(const Matrix4& a, const Matrix4& b);
  inline void sub(const Matrix4& a, const Matrix4& b);
  void mul(const Matrix4& a, const Matrix4& b);

  //scalar ops (this = a op b)
  inline void mul(const Matrix4& a, Real b);
  inline void div(const Matrix4& a, Real b);

  //vector ops (out = this op a
  inline void mul(const Vector4& a, Vector4& out) const;
  inline void mulTranspose(const Vector4& a, Vector4& out) const;
  inline void mulPoint(const Vector3& a, Vector3& out) const;	//assumes w = 1
  inline void mulVector(const Vector3& a, Vector3& out) const;	//assumes w = 0
  inline void mulVectorTranspose(const Vector3& a, Vector3& out) const;	//assumes w = 0

  inline void set(const Matrix4&);
  inline void set(Real);
  inline void set(const Real [4][4]);
  inline void set(const Real [16]);
  inline void set(const Vector3& xb, const Vector3& yb, const Vector3& zb, const Vector3& trans);
  inline void set(const Vector4& x, const Vector4& y, const Vector4& z, const Vector4& w);
  inline void set(const Matrix3&);
  inline void set(const Matrix3&, const Vector3& trans);
  inline void setZero();
  inline void setIdentity();
  inline void setTranslate(const Vector3& trans);
  inline void setTranspose(const Matrix4&);
  inline void setNegative(const Matrix4&);
  bool setInverse(const Matrix4&);

  inline void get(Matrix4&) const;
  inline void get(Real [4][4]) const;
  inline void get(Real [16]) const;
  inline void get(Vector3& xb, Vector3& yb, Vector3& zb, Vector3& trans) const;
  inline void get(Vector4& x, Vector4& y, Vector4& z, Vector4& w) const;
  inline void get(Matrix3&) const;
  inline void get(Matrix3&, Vector3&) const;
  inline void getTranspose(Matrix4&) const;
  inline void getNegative(Matrix4&) const;
  inline bool getInverse(Matrix4&) const;

  inline void inplaceTranspose();
  inline void inplaceNegative();
  inline void inplaceInverse();

  inline Real* col(int j) { return data[j]; }
  inline Real* col1() { return data[0]; }
  inline Real* col2() { return data[1]; }
  inline Real* col3() { return data[2]; }
  inline Real* col4() { return data[3]; }
  inline const Real* col(int j) const { return data[j]; }
  inline const Real* col1() const { return data[0]; }
  inline const Real* col2() const { return data[1]; }
  inline const Real* col3() const { return data[2]; }
  inline const Real* col4() const { return data[3]; }

  inline void setCol(int j, const Vector4& v) { v.get(data[j]); }
  inline void setCol1(const Vector4& v) { v.get(data[0]); }
  inline void setCol2(const Vector4& v) { v.get(data[1]); }
  inline void setCol3(const Vector4& v) { v.get(data[2]); }
  inline void setCol4(const Vector4& v) { v.get(data[3]); }
  inline void setCol(int j, const Vector3& v) { v.get(data[j]); }
  inline void setCol1(const Vector3& v) { v.get(data[0]); }
  inline void setCol2(const Vector3& v) { v.get(data[1]); }
  inline void setCol3(const Vector3& v) { v.get(data[2]); }
  inline void setCol4(const Vector3& v) { v.get(data[3]); }

  inline void setRow(int i, const Vector4& v) { v.get(data[0][i], data[1][i], data[2][i], data[3][i]); }
  inline void setRow1(const Vector4& v) { v.get(data[0][0], data[1][0], data[2][0], data[3][0]); }
  inline void setRow2(const Vector4& v) { v.get(data[0][1], data[1][1], data[2][1], data[3][1]); }
  inline void setRow3(const Vector4& v) { v.get(data[0][2], data[1][2], data[2][2], data[3][2]); }
  inline void setRow4(const Vector4& v) { v.get(data[0][3], data[1][3], data[2][3], data[3][3]); }
  inline void setRow(int i, const Vector3& v) { v.get(data[0][i], data[1][i], data[2][i]); }
  inline void setRow1(const Vector3& v) { v.get(data[0][0], data[1][0], data[2][0]); }
  inline void setRow2(const Vector3& v) { v.get(data[0][1], data[1][1], data[2][1]); }
  inline void setRow3(const Vector3& v) { v.get(data[0][2], data[1][2], data[2][2]); }
  inline void setRow4(const Vector3& v) { v.get(data[0][3], data[1][3], data[2][3]); }

  inline void getCol(int j, Vector4& v) const { v.set(data[j]); }
  inline void getCol1(Vector4& v) const { v.set(data[0]); }
  inline void getCol2(Vector4& v) const { v.set(data[1]); }
  inline void getCol3(Vector4& v) const { v.set(data[2]); }
  inline void getCol4(Vector4& v) const { v.set(data[3]); }
  inline void getCol(int j, Vector3& v) const { v.set(data[j]); }
  inline void getCol1(Vector3& v) const { v.set(data[0]); }
  inline void getCol2(Vector3& v) const { v.set(data[1]); }
  inline void getCol3(Vector3& v) const { v.set(data[2]); }
  inline void getCol4(Vector3& v) const { v.set(data[3]); }

  inline void getRow(int i, Vector4& v) const { v.set(data[0][i], data[1][i], data[2][i], data[3][i]); }
  inline void getRow1(Vector4& v) const { v.set(data[0][0], data[1][0], data[2][0], data[3][0]); }
  inline void getRow2(Vector4& v) const { v.set(data[0][1], data[1][1], data[2][1], data[3][1]); }
  inline void getRow3(Vector4& v) const { v.set(data[0][2], data[1][2], data[2][2], data[3][2]); }
  inline void getRow4(Vector4& v) const { v.set(data[0][3], data[1][3], data[2][3], data[3][3]); }
  inline void getRow(int i, Vector3& v) const { v.set(data[0][i], data[1][i], data[2][i]); }
  inline void getRow1(Vector3& v) const { v.set(data[0][0], data[1][0], data[2][0]); }
  inline void getRow2(Vector3& v) const { v.set(data[0][1], data[1][1], data[2][1]); }
  inline void getRow3(Vector3& v) const { v.set(data[0][2], data[1][2], data[2][2]); }
  inline void getRow4(Vector3& v) const { v.set(data[0][3], data[1][3], data[2][3]); }

  inline Vector3 getXBasis() const { return Vector3(col1()); }
  inline Vector3 getYBasis() const { return Vector3(col2()); }
  inline Vector3 getZBasis() const { return Vector3(col3()); }
  inline Vector3 getTranslation() const { return Vector3(col4()); }

  Real cofactor(int i, int j) const;
  inline bool isInvertible() const;
  Real determinant() const;
  inline Real trace() const;

  //column major format
  Real data[4][4];
};

inline Real determinant(const Matrix4&);
inline Real trace(const Matrix4&);
inline Matrix4 operator + (const Matrix4& a, const Matrix4& b);
inline Matrix4 operator - (const Matrix4& a, const Matrix4& b);
inline Matrix4 operator * (const Matrix4& a, const Matrix4& b);
inline Vector4 operator * (const Matrix4& a, const Vector4& b);
inline Vector4 operator * (const Vector4& a, const Matrix4& b);
inline Vector3 operator * (const Matrix4& a, const Vector3& b);		//(WARNING) vector multiply
inline Vector3 operator * (const Vector3& a, const Matrix4& b);		//(WARNING) vector multiply (transpose)


//A linear transformation of the form T(t)*R where R is an orthogonal matrix, and T is a translation by a vector t.
//The efficiency gains here come from noting that:
//1) inv(R) = transpose(R)
//2) R*T(t) = T(Rt)*R allowing for fast compositions
class RigidTransform
{
 public:
  RigidTransform();
  RigidTransform(const RigidTransform&);
  RigidTransform(const Matrix3&, const Vector3&);
  RigidTransform(const Vector3&);
  RigidTransform(const Matrix3&);
  RigidTransform(const Matrix4&);

  inline bool operator == (const RigidTransform&) const;
  inline bool operator != (const RigidTransform&) const;
  inline const RigidTransform& operator = (const RigidTransform&);									//this(v) = this(a(v))
  inline void operator *= (const RigidTransform&);									//this(v) = this(a(v))
  inline void operator *= (const Matrix3&);
  inline void operator += (const Vector3&);
  inline void operator -= (const Vector3&);

  inline void compose(const RigidTransform& a, const RigidTransform& b);		//this(v) = a(b(v))
  inline void mul(const RigidTransform& a, const RigidTransform& b);			// = compose

  //vector operators
  inline void mul(const Vector3& a, Vector3& out) const;						// = mulPoint
  inline void mul(const Vector4& a, Vector3& out) const;
  inline void mulPoint(const Vector3& a, Vector3& out) const;
  inline void mulVector(const Vector3& a, Vector3& out) const;

  inline void setIdentity();
  inline void set(const RigidTransform&);
  inline void set(const Matrix3&, const Vector3&);
  inline void set(const Vector3&);
  inline void set(const Matrix3&);
  inline void set(const Matrix4&);
  inline void setRotation(const Matrix3&);
  inline void setTranslation(const Vector3&);
  inline void setInverse(const RigidTransform&);
  inline void setRotated(const RigidTransform& a, const Matrix3& r);
  inline void setShifted(const RigidTransform& a, const Vector3& v);

  inline void get(RigidTransform&) const;
  inline void get(Matrix3&, Vector3&) const;
  inline void get(Matrix4&) const;
  inline void getRotation(Matrix3&) const;
  inline void getTranslation(Vector3&) const;
  inline void getInverse(RigidTransform&);

  inline void inplaceInverse();
  inline void inplaceRotate(const Matrix3&);
  inline void inplaceShift(const Vector3&);

  bool isValid() const;		//true if the matrix is orthogonal

  Matrix3 R;
  Vector3 t;
};

inline RigidTransform operator * (const RigidTransform&, const RigidTransform&);
inline RigidTransform operator * (const RigidTransform&, const Matrix3&);
inline RigidTransform operator * (const Matrix3& a, const RigidTransform& b);
inline RigidTransform operator + (const RigidTransform&, const Vector3&);
inline RigidTransform operator - (const RigidTransform&, const Vector3&);
inline Vector3 operator * (const RigidTransform&, const Vector3&);

class Complex 
{
 public:
  Complex();
  Complex(const Complex&);
  Complex(Real x);
  Complex(Real x, Real y);

  inline bool operator == (const Complex&) const;
  inline bool operator != (const Complex&) const;
  inline const Complex& operator =  (const Complex&);
  inline void operator += (const Complex&);
  inline void operator -= (const Complex&);
  inline void operator *= (const Complex&);
  inline void operator /= (const Complex&);
  inline const Complex& operator =  (Real);
  inline void operator += (Real);
  inline void operator -= (Real);
  inline void operator *= (Real);
  inline void operator /= (Real);

  inline void add(const Complex& a, const Complex& b);
  inline void sub(const Complex& a, const Complex& b);
  inline void mul(const Complex& a, const Complex& b);
  inline void div(const Complex& a, const Complex& b);
  inline void madd(const Complex& a, const Complex& b);
  inline void add(const Complex& a, Real);
  inline void sub(const Complex& a, Real);
  inline void mul(const Complex& a, Real);
  inline void div(const Complex& a, Real);
  inline void madd(const Complex& a, Real);

  inline void set(const Complex&);
  inline void set(Real x);
  inline void set(Real x, Real y);
  inline void setZero();
  inline void setNegative(const Complex&);
  inline void setNormalized(const Complex&);
  inline void setConjugate(const Complex&);
  inline bool setInverse(const Complex&);
  void setExp (const Complex&);
  bool setLog (const Complex&, int n = 0);
  void setPow (const Complex&, Real);

  inline void get(Complex&) const;
  inline void get(Real& x, Real& y) const;
  inline void getNegative(Complex&) const;
  inline void getNormalized(Complex&) const;
  inline void getConjugate(Complex&) const;
  inline bool getInverse(Complex&) const;

  inline void inplaceNegative();
  inline void inplaceScale(Real);
  inline void inplaceNormalize();
  inline void inplaceConjugate();
  inline bool inplaceInverse();

  inline bool isInvertible() const;
  inline Real norm() const;
  inline Real normSquared() const;
  inline Real arg() const;

  inline const Real& Re() const;
  inline const Real& Im() const;
  inline Real& Re();
  inline Real& Im();

  Real x, y;
};

inline Real dot(const Complex& a, const Complex& b);
inline Complex operator - (const Complex& a);
inline Complex operator + (const Complex& a, const Complex& b);
inline Complex operator - (const Complex& a, const Complex& b);
inline Complex operator * (const Complex& a, const Complex& b);
inline Complex operator / (const Complex& a, const Complex& b);
inline Complex operator + (const Complex& a, Real b);
inline Complex operator - (const Complex& a, Real b);
inline Complex operator * (const Complex& a, Real b);
inline Complex operator / (const Complex& a, Real b);


class Quaternion
{
 public:
  Quaternion();
  Quaternion(const Quaternion&);
  Quaternion(Real w);
  Quaternion(Real w, const Vector3& im);
  Quaternion(Real w, Real x, Real y, Real z);

  inline bool operator == (const Quaternion&) const;
  inline bool operator != (const Quaternion&) const;
  inline const Quaternion& operator =  (const Quaternion&);
  inline void operator += (const Quaternion&);
  inline void operator -= (const Quaternion&);
  inline void operator *= (const Quaternion&);
  inline void operator /= (const Quaternion&);
  inline const Quaternion& operator =  (Real);
  inline void operator += (Real);
  inline void operator -= (Real);
  inline void operator *= (Real);
  inline void operator /= (Real);

  inline void add(const Quaternion& a, const Quaternion& b);
  inline void sub(const Quaternion& a, const Quaternion& b);
  void mul(const Quaternion& a, const Quaternion& b);
  void div(const Quaternion& a, const Quaternion& b);
  void madd(const Quaternion& a, const Quaternion& b);
  inline void mul(const Quaternion& a, Real b);
  inline void div(const Quaternion& a, Real b);
  inline void madd(const Quaternion& a, Real b);

  inline void set(const Quaternion&);
  inline void set(Real w);
  inline void set(Real w, const Vector3& im);
  inline void set(Real w, Real x, Real y, Real z);
  inline void setZero();
  inline void setNegative(const Quaternion&);
  inline void setNormalized(const Quaternion&);
  inline void setConjugate (const Quaternion&);
  inline bool setInverse (const Quaternion&);
  void setExp (const Quaternion&);
  bool setLog (const Quaternion&, int n = 0);
  void setPow (const Quaternion&, Real);

  inline void get(Quaternion&) const;
  inline void get(Real& w, Vector3& im) const;
  inline void get(Real& w, Real& x, Real& y, Real& z);
  inline void getNegative(Quaternion&) const;
  inline void getNormalized(Quaternion&) const;
  inline void getConjugate (Quaternion&) const;
  inline bool getInverse (Quaternion&) const;

  inline void inplaceNegative();
  inline void inplaceScale(Real);
  inline void inplaceNormalize();
  inline void inplaceConjugate();
  inline bool inplaceInverse();

  inline bool isInvertible() const;
  inline Real norm() const;
  inline Real normSquared() const;
  inline Real imNorm() const;		//norm of imaginary part

  union {
    Real data [4];
    struct { Real w, x, y, z; };
  };
};

inline Real dot(const Quaternion& a, const Quaternion& b);
inline Quaternion operator - (const Quaternion& a);
inline Quaternion operator + (const Quaternion& a, const Quaternion& b);
inline Quaternion operator - (const Quaternion& a, const Quaternion& b);
inline Quaternion operator * (const Quaternion& a, const Quaternion& b);
inline Quaternion operator / (const Quaternion& a, const Quaternion& b);
inline Quaternion operator + (const Quaternion& a, Real b);
inline Quaternion operator - (const Quaternion& a, Real b);
inline Quaternion operator * (const Quaternion& a, Real b);
inline Quaternion operator / (const Quaternion& a, Real b);


//inlined member functions
inline bool Vector3::operator == (const Vector3& a) const { return a.x == x && a.y == y && a.z == z; }
inline bool Vector3::operator != (const Vector3& a) const { return a.x != x || a.y != y || a.z != z; }
inline const Vector3& Vector3::operator = (const Vector3& v) { set(v); return *this; }
inline const Vector3& Vector3::operator = (const Real dat[3]) { set(dat); return *this; }
inline void Vector3::operator += (const Vector3& v) { x += v.x; y += v.y; z += v.z; }
inline void Vector3::operator -= (const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; }
inline void Vector3::operator *= (Real c) { inplaceScale(c); }
inline void Vector3::operator /= (Real c) { inplaceScale(Inv(c)); }
inline Real& Vector3::operator [] (int i) { return data[i]; }
inline const Real& Vector3::operator [] (int i) const  { return data[i]; }
inline Vector3::operator Real* () { return data; }
inline Vector3::operator const Real* () const { return data; }
inline void Vector3::add(const Vector3& a, const Vector3& b) { x=a.x+b.x; y=a.y+b.y; z=a.z+b.z; }
inline void Vector3::sub(const Vector3& a, const Vector3& b) { x=a.x-b.x; y=a.y-b.y; z=a.z-b.z; }
inline void Vector3::mul(const Vector3& a, Real b) { x=a.x*b; y=a.y*b; z=a.z*b; }
inline void Vector3::div(const Vector3& a, Real b) { Real binv = Inv(b); mul(a,binv); }
inline void Vector3::madd(const Vector3& a, Real b) { x+=a.x*b; y+=a.y*b; z+=a.z*b; }
inline Real Vector3::dot(const Vector3& a) const { return x*a.x + y*a.y + z*a.z; }
inline void Vector3::set(const Vector3& a) { set(a.x,a.y,a.z); }
inline void Vector3::set(Real _x) { x = y = z = _x; }
inline void Vector3::set(Real _x, Real _y, Real _z) { x=_x; y=_y; z=_z; }
inline void Vector3::set(const Real _data[3]) { set(_data[0],_data[1],_data[2]); }
inline void Vector3::setZero() { x = y = z = Zero; }
inline void Vector3::setNegative(const Vector3& a) { x=-a.x; y=-a.y; z=-a.z; }
inline void Vector3::setCross(const Vector3& a, const Vector3& b) { x=a.y*b.z-a.z*b.y; y=a.z*b.x-a.x*b.z; z=a.x*b.y-a.y*b.x; }
inline void Vector3::setNormalized(const Vector3& a) { div(a,a.norm()); }
inline void Vector3::setProjection(const Vector3& a, const Vector3& b) { mul(b, a.dot(b)/b.dot(b)); }
inline void Vector3::get(Vector3& v) const { get(v.x,v.y,v.z); }
inline void Vector3::get(Real& _x, Real& _y, Real& _z) const { _x=x; _y=y; _z=z; }
inline void Vector3::get(Real _data[3]) const { get(_data[0],_data[1],_data[2]); }
inline void Vector3::getNegative(Vector3& v) const { v.setNegative(*this); }
inline void Vector3::getNormalized(Vector3& v) const  { v.setNormalized(*this); }
inline void Vector3::inplaceNegative() { x=-x; y=-y; z=-z; }
inline void Vector3::inplaceScale(Real c) { x*=c; y*=c; z*=c; }
inline void Vector3::inplaceNormalize() { inplaceScale(Inv(norm())); }
inline Real Vector3::norm() const { return Sqrt(normSquared()); }
inline Real Vector3::normSquared() const { return Sqr(x)+Sqr(y)+Sqr(z); }
inline Real Vector3::length() const { return norm(); }

inline bool Vector4::operator == (const Vector4& a) const { return a.x == x && a.y == y && a.z == z && a.w == w; }
inline bool Vector4::operator != (const Vector4& a) const { return a.x != x || a.y != y || a.z != z || a.w != w; }
inline const Vector4& Vector4::operator = (const Vector4& v) { set(v); return *this; }
inline const Vector4& Vector4::operator = (const Real dat[4]) { set(dat); }
inline void Vector4::operator += (const Vector4& v) { x += v.x; y += v.y; z += v.z; w += v.w; }
inline void Vector4::operator -= (const Vector4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; }
inline void Vector4::operator *= (Real c) { inplaceScale(c); }
inline void Vector4::operator /= (Real c) { inplaceScale(Inv(c)); }
inline void Vector4::operator += (const Vector3& v) { x += v.x; y += v.y; z += v.z; }
inline void Vector4::operator -= (const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; }
inline Real& Vector4::operator [] (int i) { return data[i]; }
inline const Real& Vector4::operator [] (int i) const  { return data[i]; }
inline Vector4::operator Vector3() const { return Vector3(x,y,z); }
inline Vector4::operator Real* () { return data; }
inline Vector4::operator const Real* () const { return data; }
inline void Vector4::add(const Vector4& a, const Vector4& b) { x=a.x+b.x; y=a.y+b.y; z=a.z+b.z; w=a.w+b.w; }
inline void Vector4::sub(const Vector4& a, const Vector4& b) { x=a.x-b.x; y=a.y-b.y; z=a.z-b.z; w=a.w-b.w; }
inline void Vector4::mul(const Vector4& a, Real b) { x=a.x*b; y=a.y*b; z=a.z*b; w=a.w*b; }
inline void Vector4::div(const Vector4& a, Real b) { Real binv = Inv(b); mul(a,binv); }
inline void Vector4::madd(const Vector4& a, Real b) { x+=a.x*b; y+=a.y*b; z+=a.z*b; w+=a.w*b; }
inline Real Vector4::dot(const Vector4& a) const { return x*a.x + y*a.y + z*a.z + w*a.w; }
inline Real Vector4::dot3(const Vector4& a) const { return x*a.x + y*a.y + z*a.z; }
inline Real Vector4::dot3(const Vector3& a) const { return x*a.x + y*a.y + z*a.z; }
inline void Vector4::set(const Vector4& a) { x = a.x; y = a.y; z = a.z; w = a.w; }
inline void Vector4::set(Real _x) { x = y = z = w = _x; }
inline void Vector4::set(Real _x, Real _y, Real _z, Real _w) { x=_x; y=_y; z=_z; w=_w; }
inline void Vector4::set(const Real _data[4])  { set(_data[0],_data[1],_data[2],_data[3]); }
inline void Vector4::setZero() { x = y = z = w = Zero; }
inline void Vector4::setVector(const Vector3& v) { set(v.x,v.y,v.z,Zero); }
inline void Vector4::setHomogeneous(const Vector3& v) { set(v.x,v.y,v.z,One); }
inline void Vector4::setNegative(const Vector4& v) { set(-v.x, -v.y, -v.z, -v.w); }
inline void Vector4::setNormalized(const Vector4& v) { div(v, v.norm()); }
inline void Vector4::setProjection(const Vector4& a, const Vector4& b) { mul(b, a.dot(b)/b.dot(b)); }
inline void Vector4::get(Vector4& v) const { get(v.x,v.y,v.z,v.w); }
inline void Vector4::get(Real& _x, Real& _y, Real& _z, Real& _w) const { _x=x; _y=y; _z=z; _w=w; }
inline void Vector4::get(Real _data[4]) const { get(_data[0],_data[1],_data[2],_data[3]); }
inline void Vector4::get(Vector3& v) const { v.x=x; v.y=y; v.z=z; }
inline void Vector4::getNegative(Vector4& v) const { v.setNegative(*this); }
inline void Vector4::getNormalized(Vector4& v) const { v.setNormalized(*this); }
inline void Vector4::inplaceNegative() { x=-x; y=-y; z=-z; w=-w; }
inline void Vector4::inplaceScale(Real c) { x*=c; y*=c; z*=c; w*=c; }
inline void Vector4::inplaceNormalize() { inplaceScale(Inv(norm())); }
inline Real Vector4::norm() const { return Sqrt(normSquared()); }
inline Real Vector4::normSquared() const { return Sqr(x)+Sqr(y)+Sqr(z)+Sqr(w); }
inline Real Vector4::length() const { return norm(); }




inline const Matrix3& Matrix3::operator  = (const Matrix3& m)
{
  set(m);
  return *this;
}

inline void Matrix3::operator += (const Matrix3& m)
{
  int i,j;
  for(i=0; i<3; i++)
    {
      for(j=0; j<3; j++)
	data[i][j] += m.data[i][j];
    }
}

inline void Matrix3::operator -= (const Matrix3& m)
{
  int i,j;
  for(i=0; i<3; i++)
    {
      for(j=0; j<3; j++)
	data[i][j] -= m.data[i][j];
    }
}

inline void Matrix3::operator *= (const Matrix3& m)
{
  mul(*this, m);
}

inline void Matrix3::add(const Matrix3& a, const Matrix3& b)
{
  int i,j;
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      data[i][j] = a.data[i][j] + b.data[i][j];
}

inline void Matrix3::sub(const Matrix3& a, const Matrix3& b)
{
  int i,j;
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      data[i][j] = a.data[i][j] - b.data[i][j];
}

inline void Matrix3::mul(const Matrix3& a, Real b)
{
  int i,j;
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      data[i][j] = a.data[i][j]*b;
}

inline void Matrix3::div(const Matrix3& a, Real b)
{
  Real binv = Inv(b);
  mul(a,b);
}

inline void Matrix3::mul(const Vector3& a, Vector3& out) const
{
  out.x = data[0][0]*a.x + data[1][0]*a.y + data[2][0]*a.z;
  out.y = data[0][1]*a.x + data[1][1]*a.y + data[2][1]*a.z;
  out.z = data[0][2]*a.x + data[1][2]*a.y + data[2][2]*a.z;
}

inline void Matrix3::mulTranspose(const Vector3& a, Vector3& out) const
{
  out.x = data[0][0]*a.x + data[0][1]*a.y + data[0][2]*a.z;
  out.y = data[1][0]*a.x + data[1][1]*a.y + data[1][2]*a.z;
  out.z = data[2][0]*a.x + data[2][1]*a.y + data[2][2]*a.z;
}

inline void Matrix3::set(const Matrix3& m)
{
  int i,j;
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      data[i][j] = m.data[i][j];
}

inline void Matrix3::set(Real x)
{
  int i,j;
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      data[i][j] = x;
}

inline void Matrix3::set(const Real m[3][3])
{
  int i,j;
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      data[i][j] = m[i][j];
}

inline void Matrix3::set(const Real m[9])
{
  int i,j,k=0;
  for(i=0; i<3; i++)
    for(j=0; j<3; j++, k++)
      data[i][j] = m[k];
}

inline void Matrix3::set(const Vector3& xb, const Vector3& yb, const Vector3& zb)
{
  setCol1(xb);
  setCol2(yb);
  setCol3(zb);
}

inline void Matrix3::setZero()
{
  set(Zero);
}

inline void Matrix3::setIdentity()
{
  int i,j;
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      data[i][j] = Delta(i,j);
}

inline void Matrix3::setTranspose(const Matrix3& m)
{
  int i,j;
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      data[i][j] = m.data[j][i];
}

inline void Matrix3::setNegative(const Matrix3& m)
{
  int i,j;
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      data[i][j] = -m.data[i][j];
}

inline void Matrix3::setScale(Real s)
{
  setZero();
  data[0][0] = data[1][1] = data[2][2] = s;
}

inline void Matrix3::setScale(Real sx, Real sy, Real sz)
{
  setZero();
  data[0][0] = sx;
  data[1][1] = sy;
  data[2][2] = sz;
}

inline void Matrix3::setRotateX(Real rads)
{
  Real cr = Cos(rads);
  Real sr = Sin(rads);
  setIdentity();
  data[1][1] = cr;  data[2][1] = -sr;
  data[1][2] = sr;  data[2][2] = cr;
}

inline void Matrix3::setRotateY(Real rads)
{
  Real cr = Cos(rads);
  Real sr = Sin(rads);
  setIdentity();
  data[0][0] = cr;  data[2][0] = sr;
  data[0][2] = -sr;  data[2][2] = cr;
}

inline void Matrix3::setRotateZ(Real rads)
{
  Real cr = Cos(rads);
  Real sr = Sin(rads);
  setIdentity();
  data[0][0] = cr;  data[1][0] = -sr;
  data[0][1] = sr;  data[1][1] = cr;
}

inline void Matrix3::setCrossProduct(const Vector3& v)
{
  data[1][0] = -v[2];
  data[2][1] = -v[0];
  data[2][0] = v[1];

  data[0][1] = v[2];
  data[1][2] = v[0];
  data[0][2] = -v[1];

  data[0][0] = data[1][1] = data[2][2] = Zero;
}

inline void Matrix3::get(Matrix3& m) const
{
  m.set(*this);
}

inline void Matrix3::get(Real m [3][3]) const
{
  int i,j;
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      m[i][j] = data[i][j];
}

inline void Matrix3::get(Real m [9]) const
{
  int i,j,k=0;
  for(i=0; i<3; i++)
    for(j=0; j<3; j++, k++)
      m[k] = data[i][j];
}

inline void Matrix3::get(Vector3& xb, Vector3& yb, Vector3& zb) const
{
  getCol1(xb);
  getCol2(yb);
  getCol3(zb);
}

inline void Matrix3::getTranspose(Matrix3& m) const
{
  m.setTranspose(*this);
}

inline void Matrix3::getNegative(Matrix3& m) const
{
  m.setNegative(*this);
}

inline bool Matrix3::getInverse(Matrix3& m) const
{
  return m.setInverse(*this);
}

inline void Matrix3::getCrossProduct(Vector3& v) const
{
  v[0] = data[1][2];
  v[1] = data[2][0];
  v[2] = data[0][1];
}

inline void Matrix3::inplaceTranspose()
{
  Real temp;
  int i,j;
  for(i=0; i<3; i++) {
    for(j=0; j<i; j++) {
      temp = data[i][j];
      data[i][j] = data[j][i];
      data[j][i] = temp;
    }
  }
}

inline void Matrix3::inplaceNegative()
{
  int i,j;
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      data[i][j] = -data[i][j];
}

inline void Matrix3::inplaceScale(Real s)
{
  int i,j;
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      data[i][j] *= s;	
}

inline void Matrix3::inplaceScale(Real sx, Real sy, Real sz)
{
  int j;
  for(j=0; j<3; j++) {
    data[0][j] *= sx;
    data[1][j] *= sy;
    data[2][j] *= sz;
  }
}

inline void Matrix3::inplaceInverse()
{
  Matrix3 tmp = *this;
  setInverse(tmp);
}

inline bool Matrix3::isInvertible() const
{
  return determinant() != Zero;
}

inline Real Matrix3::trace() const { return data[0][0] + data[1][1] + data[2][2]; }





inline const Matrix4& Matrix4::operator  = (const Matrix4& m)
{
  set(m);
  return *this;
}

inline void Matrix4::operator += (const Matrix4& m)
{
  int i,j;
  for(i=0; i<4; i++) {
    for(j=0; j<4; j++)
      data[i][j] += m.data[i][j];
  }
}

inline void Matrix4::operator -= (const Matrix4& m)
{
  int i,j;
  for(i=0; i<4; i++) {
    for(j=0; j<4; j++)
      data[i][j] -= m.data[i][j];
  }
}

inline void Matrix4::operator *= (const Matrix4& m)
{
  Matrix4 tmp(*this);
  mul(tmp, m);
}

inline void Matrix4::add(const Matrix4& a, const Matrix4& b)
{
  int i,j;
  for(i=0; i<4; i++) {
    for(j=0; j<4; j++)
      data[i][j] = a.data[i][j] + b.data[i][j];
  }
}

inline void Matrix4::sub(const Matrix4& a, const Matrix4& b)
{
  int i,j;
  for(i=0; i<4; i++) {
    for(j=0; j<4; j++)
      data[i][j] = a.data[i][j] - b.data[i][j];
  }
}

inline void Matrix4::mul(const Matrix4& a, Real b)
{
  int i,j;
  for(i=0; i<4; i++) {
    for(j=0; j<4; j++)
      data[i][j] = a.data[i][j]*b;
  }
}

inline void Matrix4::div(const Matrix4& a, Real b)
{
  mul(a,Inv(b));
}

inline void Matrix4::mul(const Vector4& a, Vector4& out) const
{
  out.x = data[0][0]*a.x + data[1][0]*a.y + data[2][0]*a.z + data[3][0]*a.w;
  out.y = data[0][1]*a.x + data[1][1]*a.y + data[2][1]*a.z + data[3][1]*a.w;
  out.z = data[0][2]*a.x + data[1][2]*a.y + data[2][2]*a.z + data[3][2]*a.w;
  out.w = data[0][3]*a.x + data[1][3]*a.y + data[2][3]*a.z + data[3][3]*a.w;
}

inline void Matrix4::mulTranspose(const Vector4& a, Vector4& out) const
{
  out.x = data[0][0]*a.x + data[0][1]*a.y + data[0][2]*a.z + data[0][3]*a.w;
  out.y = data[1][0]*a.x + data[1][1]*a.y + data[1][2]*a.z + data[1][3]*a.w;
  out.z = data[2][0]*a.x + data[2][1]*a.y + data[2][2]*a.z + data[2][3]*a.w;
  out.w = data[3][0]*a.x + data[3][1]*a.y + data[3][2]*a.z + data[3][3]*a.w;
}

inline void Matrix4::mulPoint(const Vector3& a, Vector3& out) const
{
  out.x = data[0][0]*a.x + data[1][0]*a.y + data[2][0]*a.z + data[3][0];
  out.y = data[0][1]*a.x + data[1][1]*a.y + data[2][1]*a.z + data[3][1];
  out.z = data[0][2]*a.x + data[1][2]*a.y + data[2][2]*a.z + data[3][2];
}

inline void Matrix4::mulVector(const Vector3& a, Vector3& out) const
{
  out.x = data[0][0]*a.x + data[1][0]*a.y + data[2][0]*a.z;
  out.y = data[0][1]*a.x + data[1][1]*a.y + data[2][1]*a.z;
  out.z = data[0][2]*a.x + data[1][2]*a.y + data[2][2]*a.z;
}

inline void Matrix4::mulVectorTranspose(const Vector3& a, Vector3& out) const
{
  out.x = data[0][0]*a.x + data[0][1]*a.y + data[0][2]*a.z;
  out.y = data[1][0]*a.x + data[1][1]*a.y + data[1][2]*a.z;
  out.z = data[2][0]*a.x + data[2][1]*a.y + data[2][2]*a.z;
}

inline void Matrix4::set(const Matrix4& m)
{
  int i,j;
  for(i=0; i<4; i++)
    for(j=0; j<4; j++)
      data[i][j] = m.data[i][j];
}

inline void Matrix4::set(Real x)
{
  int i,j;
  for(i=0; i<4; i++)
    for(j=0; j<4; j++)
      data[i][j] = x;
}

inline void Matrix4::set(const Real m [4][4])
{
  int i,j;
  for(i=0; i<4; i++)
    for(j=0; j<4; j++)
      data[i][j] = m[i][j];
}

inline void Matrix4::set(const Real m [16])
{
  int i,j,k=0;
  for(i=0; i<4; i++)
    for(j=0; j<4; j++, k++)
      data[i][j] = m[k];
}

inline void Matrix4::set(const Vector3& xb, const Vector3& yb, const Vector3& zb, const Vector3& trans)
{
  data[0][3] = data[1][3] = data[2][3] = Zero;
  data[3][3] = One;
  setCol1(xb);
  setCol2(yb);
  setCol3(zb);
  setCol4(trans);
}

inline void Matrix4::set(const Vector4& x, const Vector4& y, const Vector4& z, const Vector4& w)
{
  setCol1(x);
  setCol2(y);
  setCol3(z);
  setCol4(w);
}

inline void Matrix4::set(const Matrix3& m)
{
  data[0][3] = data[1][3] = data[2][3] = Zero;
  data[3][3] = One;
  int i,j;
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      data[i][j] = m.data[i][j];
  data[3][0] = data[3][1] = data[3][2] = Zero;
}

inline void Matrix4::set(const Matrix3& m, const Vector3& trans)
{
  data[0][3] = data[1][3] = data[2][3] = Zero;
  data[3][3] = One;
  int i,j;
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      data[i][j] = m.data[i][j];
  setCol4(trans);
}

inline void Matrix4::setZero()
{
  set(Zero);
}

inline void Matrix4::setIdentity()
{
  int i,j;
  for(i=0; i<4; i++)
    for(j=0; j<4; j++)
      data[i][j] = Delta(i,j);
}

inline void Matrix4::setTranslate(const Vector3& trans)
{
  int i,j;
  for(i=0; i<3; i++)
    for(j=0; j<4; j++)
      data[i][j] = Delta(i,j);

  trans.get(col4());
  data[3][3] = One;
}

inline void Matrix4::setTranspose(const Matrix4& m)
{
  int i,j;
  for(i=0; i<4; i++)
    for(j=0; j<4; j++)
      data[i][j] = m.data[j][i];
}

inline void Matrix4::setNegative(const Matrix4& m)
{
  int i,j;
  for(i=0; i<4; i++)
    for(j=0; j<4; j++)
      data[i][j] = -m.data[i][j];

}

inline void Matrix4::get(Matrix4& m) const { m.set(*this); }

inline void Matrix4::get(Real m[4][4]) const
{
  int i,j;
  for(i=0; i<4; i++)
    for(j=0; j<4; j++)
      m[i][j] = data[i][j];
}

inline void Matrix4::get(Real m[16]) const
{
  int i,j,k=0;
  for(i=0; i<4; i++)
    for(j=0; j<4; j++, k++)
      m[k] = data[i][j];
}

inline void Matrix4::get(Vector3& xb, Vector3& yb, Vector3& zb, Vector3& trans) const
{
  getCol1(xb);
  getCol2(yb);
  getCol3(zb);
  getCol4(trans);
}

inline void Matrix4::get(Vector4& x, Vector4& y, Vector4& z, Vector4& w) const
{
  getCol1(x);
  getCol2(y);
  getCol3(z);
  getCol4(w);
}

inline void Matrix4::get(Matrix3& m) const
{
  int i,j;
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      m.data[i][j] = data[i][j];
}

inline void Matrix4::get(Matrix3& m, Vector3& trans) const
{
  int i,j;
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      m.data[i][j] = data[i][j];
  getCol4(trans);
}

inline void Matrix4::getTranspose(Matrix4& m) const { m.setTranspose(*this); }
inline void Matrix4::getNegative(Matrix4& m) const { m.setNegative(*this); }
inline bool Matrix4::getInverse(Matrix4& m) const { m.setInverse(*this); }

inline void Matrix4::inplaceTranspose()
{
  Real temp;
  int i,j;
  for(i=0; i<4; i++) {
    for(j=0; j<i; j++) {
      temp = data[i][j];
      data[i][j] = data[j][i];
      data[j][i] = temp;
    }
  }
}

inline void Matrix4::inplaceNegative()
{
  int i,j;
  for(i=0; i<4; i++) {
    for(j=0; j<4; j++) {
      data[i][j] = -data[i][j];
    }
  }
}

inline void Matrix4::inplaceInverse()
{
  Matrix4 tmp = *this;
  setInverse(tmp);
}

inline bool Matrix4::isInvertible() const
{
  return determinant() != Zero;
}

inline Real Matrix4::trace() const { return data[0][0] + data[1][1] + data[2][2] + data[3][3]; }



inline bool RigidTransform::operator == (const RigidTransform& a) const { return a.R == R && a.t == t; }
inline bool RigidTransform::operator != (const RigidTransform& a) const { return a.R != R || a.t != t; }
inline const RigidTransform& RigidTransform::operator = (const RigidTransform& x) { set(x); return *this; }
inline void RigidTransform::operator *= (const RigidTransform& x) { t += R*x.t; R *= x.R; }
inline void RigidTransform::operator *= (const Matrix3& r) { inplaceRotate(r); }
inline void RigidTransform::operator += (const Vector3& v) { t += v; }
inline void RigidTransform::operator -= (const Vector3& v) { t -= v; }
inline void RigidTransform::compose(const RigidTransform& a, const RigidTransform& b)
{
  //t = ta + Ra*tb
  //R = Ra*Rb
  a.R.mul(b.t, t);
  t += a.t;
  R.mul(a.R,b.R);
}
inline void RigidTransform::mul(const RigidTransform& a, const RigidTransform& b) { compose(a,b); }
//vector operators
inline void RigidTransform::mul(const Vector3& a, Vector3& out) const { mulPoint(a,out); }
inline void RigidTransform::mul(const Vector4& a, Vector3& out) const { R.mul(Vector3(a.x,a.y,a.z), out); out.madd(t, a.w); }
inline void RigidTransform::mulPoint(const Vector3& a, Vector3& out) const { R.mul(a,out); out += t; }
inline void RigidTransform::mulVector(const Vector3& a, Vector3& out) const { R.mul(a,out); }
inline void RigidTransform::setIdentity() { R.setIdentity(); t.setZero(); }
inline void RigidTransform::set(const RigidTransform& x) { set(x.R, x.t); }
inline void RigidTransform::set(const Matrix3& m, const Vector3& v) { R.set(m); t.set(v); }
inline void RigidTransform::set(const Vector3& v) { R.setIdentity(); t.set(v); }
inline void RigidTransform::set(const Matrix3& m) { R.set(m); t.setZero(); }
inline void RigidTransform::set(const Matrix4& m) { m.get(R,t); }
inline void RigidTransform::setRotation(const Matrix3& m) { R.set(m); }
inline void RigidTransform::setTranslation(const Vector3& v) { t.set(v); }
inline void RigidTransform::setInverse(const RigidTransform& x) { R.setTranspose(x.R); R.mul(x.t,t); t.inplaceNegative(); }
inline void RigidTransform::setRotated(const RigidTransform& a, const Matrix3& r) { R.mul(a.R,r); t.set(a.t); }
inline void RigidTransform::setShifted(const RigidTransform& a, const Vector3& v) { R.set(a.R); t.add(a.t,v); }
inline void RigidTransform::get(RigidTransform& x) const { x.set(*this); }
inline void RigidTransform::get(Matrix3& m, Vector3& v) const { m.set(R); v.set(t); }
inline void RigidTransform::get(Matrix4& m) const { m.set(R,t); }
inline void RigidTransform::getRotation(Matrix3& m) const { m.set(R); }
inline void RigidTransform::getTranslation(Vector3& v) const { v.set(t); }
inline void RigidTransform::getInverse(RigidTransform& x) { x.setInverse(*this); }
inline void RigidTransform::inplaceInverse() { R.inplaceTranspose(); Vector3 tmp; tmp.setNegative(t); R.mul(tmp,t); }
inline void RigidTransform::inplaceRotate(const Matrix3& m) { R *= m; }
inline void RigidTransform::inplaceShift(const Vector3& v) { t += v; }


inline bool Complex::operator == (const Complex& a) const { return a.x == x && a.y == y; }
inline bool Complex::operator != (const Complex& a) const { return a.x != x || a.y != y; }
inline const Complex& Complex::operator =  (const Complex& z) { set(z); return *this; }
inline void Complex::operator += (const Complex& z) { x += z.x; y += z.y; }
inline void Complex::operator -= (const Complex& z) { x -= z.x; y -= z.y; }
inline void Complex::operator *= (const Complex& z) { Real tmp = x*z.x - y*z.y; y = x*z.y + y*z.x; x = tmp; }
inline void Complex::operator /= (const Complex& z) { div(*this, z); }
inline const Complex& Complex::operator =  (Real a) { x = a; y = Zero;	return *this; }
inline void Complex::operator += (Real a) { x += a; }
inline void Complex::operator -= (Real a) { x -= a; }
inline void Complex::operator *= (Real a) { x *= a; y *= a; }
inline void Complex::operator /= (Real a) { Real ainv = Inv(a); operator *= (a); }
inline void Complex::add(const Complex& a, const Complex& b) { x = a.x + b.x; y = a.y + b.y; }
inline void Complex::sub(const Complex& a, const Complex& b) { x = a.x - b.x; y = a.y - b.y; }
inline void Complex::mul(const Complex& a, const Complex& b) { Real tmp = a.x*b.x - a.y*b.y; y = a.x*b.y + a.y*b.x; x = tmp; }
inline void Complex::madd(const Complex& a, const Complex& b) { Real tmp = a.x*b.x - a.y*b.y; y += a.x*b.y + a.y*b.x; x += tmp; }
inline void Complex::div(const Complex& a, const Complex& b) { Real l = Inv(b.normSquared()); Real tmp = a.x*b.x + a.y*b.y; y = a.x*b.y - a.y*b.x; x = tmp; x*=l; y*=l; }
inline void Complex::add(const Complex& a, Real b) { x = a.x + b; }
inline void Complex::sub(const Complex& a, Real b) { x = a.x - b; }
inline void Complex::mul(const Complex& a, Real b) { x = a.x*b; y = a.y*b; }
inline void Complex::div(const Complex& a, Real b) { Real binv = Inv(b); mul(a,binv); }
inline void Complex::madd(const Complex& a, Real b) { x += a.x*b; y += a.y*b; }
inline void Complex::set(const Complex& z) { x = z.x; y = z.y; }
inline void Complex::set(Real _x) { x = _x; y = Zero; }
inline void Complex::set(Real _x, Real _y) { x = _x; y = _y; }
inline void Complex::setZero() { x = Zero; y = Zero; }
inline void Complex::setNegative(const Complex& z) { x = -z.x; y = -z.y; }
inline void Complex::setNormalized(const Complex& z) { div(z,z.norm()); }
inline void Complex::setConjugate(const Complex& z) { x = z.x; y = -z.y; }
inline bool Complex::setInverse(const Complex& z) { Real n2 = z.normSquared(); if(n2==Zero) return false; div(z,n2); inplaceConjugate(); return true;}
inline void Complex::get(Complex& z) const { z.x = x; z.y = y; }
inline void Complex::get(Real& _x, Real& _y) const  { _x = x; _y = y; }
inline void Complex::getNegative(Complex& z) const { z.setNegative(*this); }
inline void Complex::getNormalized(Complex& z) const { z.setNormalized(*this); }
inline void Complex::getConjugate(Complex& z) const { z.setConjugate(*this); }
inline bool Complex::getInverse(Complex& z) const { return z.setInverse(*this); }
inline void Complex::inplaceNegative() { x = -x; y = -y; }
inline void Complex::inplaceScale(Real c) { x*=c; y*=c; }
inline void Complex::inplaceNormalize() { inplaceScale(Inv(norm())); }
inline void Complex::inplaceConjugate() { y = -y; }
inline bool Complex::inplaceInverse() { Real n2 = normSquared(); if(n2==Zero) return false; inplaceScale(Inv(n2)); inplaceConjugate(); return true; }
inline bool Complex::isInvertible() const { return !(x == Zero && y == Zero); }
inline Real Complex::normSquared() const { return Sqr(x)+Sqr(y); }
inline Real Complex::norm() const { return Sqrt(normSquared()); }
inline Real Complex::arg() const { return Atan2(y,x); }
inline const Real& Complex::Re() const { return x; }
inline const Real& Complex::Im() const { return y; }
inline Real& Complex::Re() { return x; }
inline Real& Complex::Im() { return y; }


inline bool Quaternion::operator == (const Quaternion& a) const { return a.x == x && a.y == y && a.z == z && a.w == w; }
inline bool Quaternion::operator != (const Quaternion& a) const { return a.x != x || a.y != y || a.z != z || a.w != w; }
inline const Quaternion& Quaternion::operator = (const Quaternion& q) { set(q); return *this; }
inline void Quaternion::operator += (const Quaternion& q) { w+=q.w; x+=q.x; y+=q.y; z+=q.z; }
inline void Quaternion::operator -= (const Quaternion& q) { w-=q.w; x-=q.x; y-=q.y; z-=q.z; }
inline void Quaternion::operator *= (const Quaternion& q) { mul(*this,q); }
inline void Quaternion::operator /= (const Quaternion& q) { div(*this,q); }
inline const Quaternion& Quaternion::operator = (Real s) { set(s); return *this; }
inline void Quaternion::operator += (Real s) { w += s; }
inline void Quaternion::operator -= (Real s) { w -= s; }
inline void Quaternion::operator *= (Real s) { w*=s; x*=s; y*=s; z*=s; }
inline void Quaternion::operator /= (Real s) { Real s_inv = Inv(s); operator *= (s_inv); }
inline void Quaternion::add(const Quaternion& a, const Quaternion& b) { w=a.w+b.w; x=a.x+b.x; y=a.y+b.y; z=a.z+b.z; }
inline void Quaternion::sub(const Quaternion& a, const Quaternion& b) { w=a.w-b.w; x=a.x-b.x; y=a.y-b.y; z=a.z-b.z; }
inline void Quaternion::mul(const Quaternion& a, Real b) { w=a.w*b; x=a.x*b; y=a.y*b; z=a.z*b; }
inline void Quaternion::div(const Quaternion& a, Real b) { Real binv = Inv(b); mul(a,binv); }
inline void Quaternion::madd(const Quaternion& a, Real b) { w+=a.w*b; x+=a.x*b; y+=a.y*b; z+=a.z*b; }
inline void Quaternion::set(const Quaternion& q) { w=q.w; x=q.x; y=q.y; z=q.z; }
inline void Quaternion::set(Real _w) { w=_w; x = y = z = Zero; }
inline void Quaternion::set(Real _w, const Vector3& im)  { w=_w; im.get(x,y,z); }
inline void Quaternion::set(Real _w, Real _x, Real _y, Real _z) { w=_w; x=_x; y=_y; z=_z; }
inline void Quaternion::setZero() { w = x = y = z = Zero; }
inline void Quaternion::setNegative(const Quaternion& q) { w=-q.w; x=-q.x; y=-q.y; z=-q.z; }
inline void Quaternion::setNormalized(const Quaternion& q) { div(q,q.norm()); }
inline void Quaternion::setConjugate (const Quaternion& q) { w = q.w; x = -q.x; y = -q.y; z = -q.z; }
inline bool Quaternion::setInverse (const Quaternion& q) { Real n2=q.normSquared(); if(n2==Zero) return false; div(q,n2); inplaceConjugate(); return true; }
inline void Quaternion::get(Quaternion& q) const { q.set(*this); }
inline void Quaternion::get(Real& _w, Vector3& im) const { _w=w; im.set(x,y,z); }
inline void Quaternion::get(Real& _w, Real& _x, Real& _y, Real& _z) { _w=w; _x=x; _y=y; _z=z; }
inline void Quaternion::getNegative(Quaternion& q) const { q.setNegative(*this); }
inline void Quaternion::getNormalized(Quaternion& q) const { q.setNormalized(*this); }
inline void Quaternion::getConjugate (Quaternion& q) const { q.setConjugate(*this); }
inline bool Quaternion::getInverse (Quaternion& q) const { return q.setInverse(*this); }
inline void Quaternion::inplaceNegative() { w=-w; x=-x; y=-y; z=-z; }
inline void Quaternion::inplaceScale(Real c) { w*=c; x*=c; y*=c; z*=c; }
inline void Quaternion::inplaceNormalize() { inplaceScale(Inv(norm())); }
inline void Quaternion::inplaceConjugate() { x=-x; y=-y; z=-z; }
inline bool Quaternion::inplaceInverse() { Real n2 = normSquared(); if(n2==Zero) return false; inplaceScale(Inv(n2)); inplaceConjugate(); return true; }
inline bool Quaternion::isInvertible() const { return !(w == Zero && x == Zero && y == Zero && z == Zero); }
inline Real Quaternion::norm() const { return Sqrt(normSquared()); }
inline Real Quaternion::normSquared() const { return Sqr(w)+Sqr(x)+Sqr(y)+Sqr(z); }
inline Real Quaternion::imNorm() const { return Sqr(x)+Sqr(y)+Sqr(z); }

//inlined standalone functions/operators (less efficient than the member functions)

//Vector3


inline Real dot(const Vector3& a, const Vector3& b)
{
  return a.dot(b);
}

inline void normalize(Vector3& a)
{
  a.inplaceNormalize();
}

inline Vector3 cross(const Vector3& a, const Vector3& b)
{
  Vector3 temp;
  temp.setCross(a,b);
  return temp;
}

inline Vector3 project(const Vector3& a, const Vector3& b)
{
  Vector3 temp;
  temp.setProjection(a,b);
  return temp;
}

inline Vector3 operator - (const Vector3& a)
{
  Vector3 temp;
  temp.setNegative(a);
  return temp;
}

inline Vector3 operator + (const Vector3& a, const Vector3& b)
{
  Vector3 temp;
  temp.add(a,b);
  return temp;
}

inline Vector3 operator - (const Vector3& a, const Vector3& b)
{
  Vector3 temp;
  temp.sub(a,b);
  return temp;
}

inline Vector3 operator * (const Vector3& a, Real b)
{
  Vector3 temp;
  temp.mul(a,b);
  return temp;
}

inline Vector3 operator * (Real a, const Vector3& b)
{
  Vector3 temp;
  temp.mul(b,a);
  return temp;
}

inline Vector3 operator / (const Vector3& a, Real b)
{
  Vector3 temp;
  temp.div(a,b);
  return temp;
}

inline Vector3 operator * (const Vector3& a, const Vector3& b)
{
	return Vector3(a.x*b.x,a.y*b.y,a.z*b.z);
}

//Vector4

inline Real dot(const Vector4& a, const Vector4& b)
{
  return a.dot(b);
}

inline Real dot3(const Vector4& a, const Vector4& b)
{
  return a.dot3(b);
}

inline Real dot3(const Vector4& a, const Vector3& b)
{
  return a.dot3(b);
}

inline Real dot3(const Vector3& a, const Vector4& b)
{
  return b.dot3(a);
}

inline void normalize(Vector4& a)
{
  a.inplaceNormalize();
}

inline Vector4 operator - (const Vector4& a)
{
  Vector4 temp;
  temp.setNegative(a);
  return temp;
}

inline Vector4 operator + (const Vector4& a, const Vector4& b)
{
  Vector4 temp;
  temp.add(a,b);
  return temp;
}

inline Vector4 operator - (const Vector4& a, const Vector4& b)
{
  Vector4 temp;
  temp.sub(a,b);
  return temp;
}

inline Vector4 operator * (const Vector4& a, Real b)
{
  Vector4 temp;
  temp.mul(a,b);
  return temp;
}

inline Vector4 operator / (const Vector4& a, Real b)
{
  Vector4 temp;
  temp.div(a,b);
  return temp;
}

//Matrix3

inline Matrix3 transpose(const Matrix3& m) { Matrix3 x; x.setTranspose(m); return x; }
inline Real determinant(const Matrix3& m) { return m.determinant(); }
inline Real trace(const Matrix3& m) { return m.trace(); }

inline Matrix3 operator + (const Matrix3& a, const Matrix3& b)
{
	Matrix3 m;
	m.add(a,b);
	return m;
}

inline Matrix3 operator - (const Matrix3& a, const Matrix3& b)
{
	Matrix3 m;
	m.sub(a,b);
	return m;
}

inline Matrix3 operator * (const Matrix3& a, const Matrix3& b)
{
	Matrix3 m;
	m.mul(a,b);
	return m;
}

inline Vector3 operator * (const Matrix3& a, const Vector3& b)
{
	Vector3 v;
	a.mul(b,v);
	return v;
}

inline Vector3 operator * (const Vector3& a, const Matrix3& b)
{
	Vector3 v;
	b.mulTranspose(a,v);
	return v;
}


//Matrix4

inline Real determinant(const Matrix4& m) { return m.determinant(); }
inline Real trace(const Matrix4& m) { return m.determinant(); }

inline Matrix4 operator + (const Matrix4& a, const Matrix4& b)
{
	Matrix4 m;
	m.add(a,b);
	return m;
}

inline Matrix4 operator - (const Matrix4& a, const Matrix4& b)
{
	Matrix4 m;
	m.sub(a,b);
	return m;
}

inline Matrix4 operator * (const Matrix4& a, const Matrix4& b)
{
	Matrix4 m;
	m.mul(a,b);
	return m;
}

inline Vector4 operator * (const Matrix4& a, const Vector4& b)
{
	Vector4 v;
	a.mul(b,v);
	return v;
}

inline Vector4 operator * (const Vector4& a, const Matrix4& b)
{
	Vector4 v;
	b.mulTranspose(a,v);
	return v;
}

inline Vector3 operator * (const Matrix4& a, const Vector3& b)
{
	Vector3 v;
	a.mulVector(b,v);
	return v;
}

inline Vector3 operator * (const Vector3& a, const Matrix4& b)
{
	Vector3 v;
	b.mulVectorTranspose(a,v);
	return v;
}


//RigidTransform

inline RigidTransform operator * (const RigidTransform& a, const RigidTransform& b)
{
	RigidTransform x;
	x.compose(a,b);
	return x;
}

inline RigidTransform operator * (const RigidTransform& a, const Matrix3& b)
{
	RigidTransform x;
	x.R.mul(a.R,b);
	x.t = a.t;
	return x;
}

inline RigidTransform operator * (const Matrix3& a, const RigidTransform& b)
{
	RigidTransform x;
	x.R.mul(a,b.R);
	a.mul(b.t,x.t);
	return x;
}

inline RigidTransform operator + (const RigidTransform& a, const Vector3& b)
{
	return RigidTransform (a.R,a.t+b);
}

inline RigidTransform operator - (const RigidTransform& a, const Vector3& b)
{
	return RigidTransform (a.R,a.t-b);
}

inline Vector3 operator * (const RigidTransform& a, const Vector3& b)
{
	Vector3 x;
	a.mul(b,x);
	return x;
}

//Complex

inline Real dot(const Complex& a, const Complex& b)
{
  return a.x*b.x + a.y*b.y;
}

inline Complex operator - (const Complex& a)
{
  Complex temp;
  temp.setNegative(a);
  return temp;
}

inline Complex operator + (const Complex& a, const Complex& b)
{
  Complex temp;
  temp.add(a,b);
  return temp;
}

inline Complex operator - (const Complex& a, const Complex& b)
{
  Complex temp;
  temp.sub(a,b);
  return temp;
}

inline Complex operator * (const Complex& a, const Complex& b)
{
  Complex temp;
  temp.mul(a,b);
  return temp;
}

inline Complex operator / (const Complex& a, const Complex& b)
{
  Complex temp;
  temp.div(a,b);
  return temp;
}

inline Complex operator + (const Complex& a, Real b)
{
  Complex temp;
  temp.add(a,b);
  return temp;
}

inline Complex operator - (const Complex& a, Real b)
{
  Complex temp;
  temp.sub(a,b);
  return temp;
}

inline Complex operator * (const Complex& a, Real b)
{
  Complex temp;
  temp.mul(a,b);
  return temp;
}

inline Complex operator / (const Complex& a, Real b)
{
  Complex temp;
  temp.div(a,b);
  return temp;
}

//Quaternion

inline Real dot(const Quaternion& a, const Quaternion& b)
{
  return a.w*b.w + a.x*b.x + a.y*b.y + a.z*b.z;
}

inline Quaternion operator - (const Quaternion& a)
{
  Quaternion temp;
  temp.setNegative(a);
  return temp;
}

inline Quaternion operator + (const Quaternion& a, const Quaternion& b)
{
  Quaternion temp;
  temp.add(a,b);
  return temp;
}

inline Quaternion operator - (const Quaternion& a, const Quaternion& b)
{
  Quaternion temp;
  temp.sub(a,b);
  return temp;
}

inline Quaternion operator * (const Quaternion& a, const Quaternion& b)
{
  Quaternion temp;
  temp.mul(a,b);
  return temp;
}

inline Quaternion operator / (const Quaternion& a, const Quaternion& b)
{
  Quaternion temp;
  temp.div(a,b);
  return temp;
}

inline Quaternion operator + (const Quaternion& a, Real b)
{
  Quaternion temp;
  temp.add(a,b);
  return temp;
}

inline Quaternion operator - (const Quaternion& a, Real b)
{
  Quaternion temp;
  temp.sub(a,b);
  return temp;
}

inline Quaternion operator * (const Quaternion& a, Real b)
{
  Quaternion temp;
  temp.mul(a,b);
  return temp;
}

inline Quaternion operator / (const Quaternion& a, Real b)
{
  Quaternion temp;
  temp.div(a,b);
  return temp;
}


//IO functions
#include <iostream>

typedef std::ostream myostream;
typedef std::istream myistream;

myostream& operator << (myostream&, const Vector3&);
myistream& operator >> (myistream&, Vector3&);

myostream& operator << (myostream&, const Vector4&);
myistream& operator >> (myistream&, Vector4&);

myostream& operator << (myostream&, const Matrix3&);
myistream& operator >> (myistream&, Matrix3&);

myostream& operator << (myostream&, const Matrix4&);
myistream& operator >> (myistream&, Matrix4&);

myostream& operator << (myostream&, const RigidTransform&);
myistream& operator >> (myistream&, RigidTransform&);

myostream& operator << (myostream&, const Complex&);
myistream& operator >> (myistream&, Complex&);

myostream& operator << (myostream&, const Quaternion&);
myistream& operator >> (myistream&, Quaternion&);


#endif
