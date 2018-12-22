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
// Version: 2.2
// Date: March 15, 2000
// Authors: Dante Treglia II and Mark A. DeLoura
//==========================================================

#ifndef _MTXLIB_H
#define _MTXLIB_H

#include <cstdio>

#define DegToRad(a) ((a) * 0.01745329252f)
#define RadToDeg(a) ((a) * 57.29577951f)

// Useful constants
const int kX = 0;
const int kY = 1;
const int kZ = 2;
const int kW = 3;

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
  vector2(float inX, float inY);
  vector2(const vector2 &v);
  
 public:
  // Operators
  float                 &operator [] (unsigned int index);
  const float           &operator [] (unsigned int index) const;
  vector2               &operator =  (const vector2 &v);
  vector2               &operator += (const vector2 &v);
  vector2               &operator -= (const vector2 &v);
  vector2               &operator *= (float f);
  vector2               &operator /= (float f);
  friend bool           operator == (const vector2 &a, const vector2 &b);
  friend bool           operator != (const vector2 &a, const vector2 &b);
  friend vector2        operator - (const vector2 &a);
  friend vector2        operator + (const vector2 &a, const vector2 &b);
  friend vector2        operator - (const vector2 &a, const vector2 &b);
  friend vector2        operator * (const vector2 &v, float f);
  friend vector2        operator * (float f, const vector2 &v);
  friend vector2        operator / (const vector2 &v, float f);
  
  //operator float*() {return &x;};

 public:
  // Methods
  float                 length() const;
  float                 lengthSqr() const;
  bool                  isZero() const;
  vector2               &normalize();
  
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
  vector3(float inX, float inY, float inZ);
  vector3(const vector3 &v);
  explicit vector3(const vector2 &v);
  
 public:
  // Operators
  float                 &operator [] (unsigned int index);
  const float           &operator [] (unsigned int index) const;
  vector3               &operator =  (const vector3 &v);
  vector3               &operator =  (const vector2 &v);
  vector3               &operator += (const vector3 &v);
  vector3               &operator -= (const vector3 &v);
  vector3               &operator *= (float f);
  vector3               &operator /= (float f);
  friend bool           operator == (const vector3 &a, const vector3 &b);
  friend bool           operator != (const vector3 &a, const vector3 &b);
  friend vector3        operator - (const vector3 &a);
  friend vector3        operator + (const vector3 &a, const vector3 &b);
  friend vector3        operator - (const vector3 &a, const vector3 &b);
  friend vector3        operator * (const vector3 &v, float f);
  friend vector3        operator * (float f, const vector3 &v);
  friend vector3        operator / (const vector3 &v, float f);

  //operator float*() {return &x;};

 public:
  // Methods
  float                 length() const;
  float                 lengthSqr() const;
  bool                  isZero() const;
  vector3               &normalize();
  
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
  vector4() {};
  vector4(float inX, float inY, float inZ, float inW);
  vector4(const vector4 &v);
  explicit vector4(const vector3 &v);
  explicit vector4(const vector2 &v);
  
 public:
  // Operators
  float                 &operator [] (unsigned int index);
  const float           &operator [] (unsigned int index) const;
  vector4               &operator =  (const vector4 &v);
  vector4               &operator =  (const vector3 &v);
  vector4               &operator =  (const vector2 &v);
  vector4               &operator += (const vector4 &v);
  vector4               &operator -= (const vector4 &v);
  vector4               &operator *= (float f);
  vector4               &operator /= (float f);
  friend bool           operator == (const vector4 &a, const vector4 &b);
  friend bool           operator != (const vector4 &a, const vector4 &b);
  friend vector4        operator - (const vector4 &a);
  friend vector4        operator + (const vector4 &a, const vector4 &b);
  friend vector4        operator - (const vector4 &a, const vector4 &b);
  friend vector4        operator * (const vector4 &v, float f);
  friend vector4        operator * (float f, const vector4 &v);
  friend vector4        operator / (const vector4 &v, float f);

  //operator float*() {return &x;};

 public:
  // Methods
  float                 length() const;
  float                 lengthSqr() const;
  bool                  isZero() const;
  vector4               &normalize();
  
  // Debug
  void                  fprint(FILE* file, char* str) const;
};


////////////////////////////////////////////////////////////
// Miscellaneous vector functions
//

float DotProduct(const vector2 &a, const vector2 &b);
float DotProduct(const vector3 &a, const vector3 &b);
float DotProduct(const vector4 &a, const vector4 &b);
void SwapVec(const vector2 &a, const vector2 &b);
void SwapVec(const vector3 &a, const vector3 &b);
void SwapVec(const vector4 &a, const vector4 &b);
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
  matrix33(const matrix33 &m);
  matrix33(const vector3 &v0, const vector3 &v1, const vector3 &v2);
  
 public:
  // Operators
  vector3		&operator [] (unsigned int i);
  const vector3		&operator [] (unsigned int i) const;
  matrix33		&operator =  (const matrix33 &m);
  matrix33		&operator += (const matrix33 &m);
  matrix33		&operator -= (const matrix33 &m);
  matrix33		&operator *= (const matrix33 &m);
  matrix33		&operator *= (float f);
  matrix33		&operator /= (float f);
  friend bool		operator == (const matrix33 &a, const matrix33 &b);
  friend bool		operator != (const matrix33 &a, const matrix33 &b);
  friend matrix33	operator + (const matrix33 &a, const matrix33 &b);
  friend matrix33	operator - (const matrix33 &a, const matrix33 &b);
  friend matrix33	operator * (const matrix33 &a, const matrix33 &b);
  friend vector3 	operator * (const matrix33 &m, const vector3 &v);
  friend vector3 	operator * (const vector3 &v, const matrix33 &m);
  friend matrix33	operator * (const matrix33 &m, float f);
  friend matrix33	operator * (float f, const matrix33 &m);
  friend matrix33	operator / (const matrix33 &m, float f);
  
  //operator float*() {return (float*)col[0]; };

 public:
  // Methods
  matrix33		&identity();
  matrix33		&transpose();
  matrix33		&invert();

  // Debug
  void			fprint(FILE* file, char* str) const;
};

matrix33	IdentityMatrix33();
matrix33	TransposeMatrix33(const matrix33 &m);
matrix33	InvertMatrix33(const matrix33 &m);
matrix33	RotateRadMatrix33(float rad);
matrix33	TranslateMatrix33(float x, float y);
matrix33	ScaleMatrix33(float x, float y, float z = 1.0);


////////////////////////////////////////////////////////////
// matrix44 class
//

class matrix44 
{
 public:
  // Members
  vector4	col[4];

 public:
  // Constructors
  matrix44() {};
  matrix44(const matrix44 &m);
  explicit matrix44(const matrix33 &m);
  matrix44(const vector4 &v0, const vector4 &v1, 
	   const vector4 &v2, const vector4 &v3);

 public:
  // Operators
  vector4		&operator [] (unsigned int i);
  const vector4		&operator [] (unsigned int i) const;
  matrix44		&operator =  (const matrix44 &m);
  matrix44		&operator =  (const matrix33 &m);
  matrix44		&operator += (const matrix44 &m);
  matrix44		&operator -= (const matrix44 &m);
  matrix44		&operator *= (const matrix44 &m);
  matrix44		&operator *= (float f);
  matrix44		&operator /= (float f);
  friend bool		operator == (const matrix44 &a, const matrix44 &b);
  friend bool		operator != (const matrix44 &a, const matrix44 &b);
  friend matrix44	operator + (const matrix44 &a, const matrix44 &b);
  friend matrix44	operator - (const matrix44 &a, const matrix44 &b);
  friend matrix44	operator * (const matrix44 &a, const matrix44 &b);
  friend vector3 	operator * (const matrix44 &m, const vector3 &v);
  friend vector3 	operator * (const vector3 &v, const matrix44 &m);
  friend vector4 	operator * (const matrix44 &m, const vector4 &v);
  friend vector4 	operator * (const vector4 &v, const matrix44 &m);
  friend matrix44	operator * (const matrix44 &m, float f);
  friend matrix44	operator * (float f, const matrix44 &m);
  friend matrix44	operator / (const matrix44 &m, float f);

  //operator float*() {return (float*)col[0]; };

 public:
  // Methods
  matrix44		&identity();
  matrix44		&transpose();
  matrix44		&invert();

  // Debug
  void			fprint(FILE* file, char* str) const;
};

matrix44	IdentityMatrix44();
matrix44	TransposeMatrix44(const matrix44 &m);
matrix44	InvertMatrix44(const matrix44 &m);
matrix44	RotateRadMatrix44(char axis, float rad);
matrix44	RotateRadMatrix44(const vector3 &axis, float rad);
matrix44	TranslateMatrix44(float x, float y, float z);
matrix44	ScaleMatrix44(float x, float y, float z, float w = 1.0);
matrix44	LookAtMatrix44(const vector3 &camPos, const vector3 &camUp, 
			       const vector3 &target );
matrix44	FrustumMatrix44(float l, float r, float b, float t, 
				float n, float f);
matrix44	PerspectiveMatrix44(float fovY, float aspect, 
				    float n, float f);
matrix44	OrthoMatrix44(float l, float r, float b, float t, 
			      float n, float f);

#endif
