////////////////////////////////////////////////////////////
//
//  VECTOR3.H - 3-D Vector and Point Classes
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/07/21 - Version 1.02B release.
//              96/02/14 - Version 1.02C release.
//              96/04/01 - Version 1.03A release.
//              97/04/01 - Version 1.03B release.
//
//  Compilers:  Microsoft Visual C/C++ Professional V5.0
//              Borland C++ Version 5.0
//
//  Author:     Ian Ashdown, P.Eng.
//              byHeart Consultants Limited
//              620 Ballantree Road
//              West Vancouver, B.C.
//              Canada V7S 1W3
//              Tel. (604) 922-6148
//              Fax. (604) 987-7621
//
//  Copyright 1994-2003 byHeart Consultants Limited
//
//  The following source code has been derived from:
//
//    Ashdown, I. 1994. Radiosity: A Programmer's
//    Perspective. New York, NY: John Wiley & Sons.
//
//    (Available on CD-ROM from www.helios32.com.)
//
//  It may be freely copied, redistributed, and/or modified
//  for personal use ONLY, as long as the copyright notice
//  is included with all source code files.
//
////////////////////////////////////////////////////////////

#ifndef _VECTOR3_H
#define _VECTOR3_H

#include <math.h>
#include "general.h"

class Vector3;  // Forward reference

class Space3    // 3-D co-ordinates
{
  protected:
    float x;    // X-axis co-ordinate
    float y;    // Y-axis co-ordinate
    float z;    // Z-axis co-ordinate

  public:
    Space3() { };
    Space3( double xval, double yval, double zval )
    {
      x = (float) xval;
      y = (float) yval;
      z = (float) zval;
    }

    double GetX() { return x; }
    double GetY() { return y; }
    double GetZ() { return z; }

    void SetX( double xval ) { x = (float) xval; }
    void SetY( double yval ) { y = (float) yval; }
    void SetZ( double zval ) { z = (float) zval; }
};

class Point3 : public Space3    // 3-D point
{
  public:
    Point3() : Space3() { };

    Point3( double xval, double yval, double zval ) :
        Space3 ( xval, yval, zval )
    { };

    // Add vector v to point p
    friend Point3 operator+( Point3 p, Vector3 v );

    // Add point p to vector v
    friend Point3 operator+( Vector3 v, Point3 p );

    friend class Vector3;
};

class Vector3 : public Space3   // 3D vector
{
  public:
    Vector3() : Space3() { };

    Vector3( double xval, double yval, double zval ) :
        Space3 ( xval, yval, zval )
    { };

    Vector3( Point3 &p ) : Space3()
    { x = p.x; y = p.y; z = p.z; }

    Vector3( Point3 &start, Point3 &end ) : Space3()
    {
      x = end.x - start.x;
      y = end.y - start.y;
      z = end.z - start.z;
    }

    // Return vector length
    double Length()
    { return sqrt(x * x + y * y + z * z); }

    // Assign scalar
    Vector3 &operator=( double s )
    {
      x = (float) s;
      y = (float) s;
      z = (float) s;
      
      return *this;
    }

    // Add/assign vector v
    Vector3 &operator+=( Vector3 &v )
    { x += v.x; y += v.y; z += v.z; return *this; }

    // Subtract/assign vector v
    Vector3 &operator-=( Vector3 &v )
    { x -= v.x; y -= v.y; z -= v.z; return *this; }

    // Multiply/assign by scalar s
    Vector3 &operator*=( double s )
    {
      x *= (float) s;
      y *= (float) s;
      z *= (float) s;
      
      return *this;
    }

    // Divide/assign by scalar s
    Vector3 &operator/=( double s )
    {
      x /= (float) s;
      y /= (float) s;
      z /= (float) s;
      
      return *this;
    }

    // Negation
    Vector3 operator-()
    {
      Vector3 temp;     // Temporary 3-D vector

      temp.x = -x;
      temp.y = -y;
      temp.z = -z;

      return temp;
    }

    // Add vector v2 to vector v1
    friend Vector3 operator+( Vector3 v1, Vector3 v2 )
    {
      Vector3 temp;     // Temporary 3-D vector

      temp.x = v1.x + v2.x;
      temp.y = v1.y + v2.y;
      temp.z = v1.z + v2.z;

      return temp;
    }

    // Subtract vector v2 from vector v1
    friend Vector3 operator-( Vector3 v1, Vector3 v2 )
    {
      Vector3 temp;     // Temporary 3-D vector

      temp.x = v1.x - v2.x;
      temp.y = v1.y - v2.y;
      temp.z = v1.z - v2.z;

      return temp;
    }

    // Multiply vector v by scalar s
    friend Vector3 operator*( Vector3 v, double s )
    {
      Vector3 temp;     // Temporary 3-D vector

      temp.x = v.x * (float) s;
      temp.y = v.y * (float) s;
      temp.z = v.z * (float) s;

      return temp;
    }

    // Multiply scalar s by vector v
    friend Vector3 operator*( double s, Vector3 v )
    { return v * s; }

    // Divide vector v by scalar s
    friend Vector3 operator/( Vector3 v, double s )
    {
      Vector3 temp;     // Temporary 3-D vector

      temp.x = v.x / (float) s;
      temp.y = v.y / (float) s;
      temp.z = v.z / (float) s;

      return temp;
    }

    // Divide scalar s by vector v
    friend Vector3 operator/( double s, Vector3 v )
    { return v / s; }

    // Normalize
    Vector3 &Norm()
    {
      double len = Length();

      if (len < MIN_VALUE)
        len = 1.0;
        
      x /= (float) len;
      y /= (float) len;
      z /= (float) len;

      return *this;
    }

    // Return dot product of vectors v1 and v2
    friend double Dot( Vector3 &v1, Vector3 &v2 )
    { return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z); }

    // Return cross product of vectors v1 and v2
    friend Vector3 Cross( Vector3 &v1, Vector3 &v2 )
    {
      Vector3 temp;     // Temporary 3-D vector

      temp.x = v1.y * v2.z - v1.z * v2.y;
      temp.y = v1.z * v2.x - v1.x * v2.z;
      temp.z = v1.x * v2.y - v1.y * v2.x;

      return temp;
    }
};

#endif

