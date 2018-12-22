////////////////////////////////////////////////////////////
//
//  VECTOR4.H - 4-D Homogeneous Co-ordinates Vector Class
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

#ifndef _VECTOR4_H
#define _VECTOR4_H

#include <math.h>
#include "vector3.h"

class ViewSys;  // Forward reference

class Vector4 : public Space3   // 4-D vector
{
  private:
    float w;    // W-axis co-ordinate

  public:
    Vector4() : Space3() { };

    Vector4( double xval, double yval, double zval, double
        wval ) : Space3( xval, yval, zval )
    { w = (float) wval; }

    double GetW() { return w; }
    void SetW( double wval ) { w = (float) wval; }

    // Return vector length
    double Length()
    { return sqrt(x * x + y * y + z * z + w * w); }

    // Normalize vector
    Vector4 &Norm()
    {
      double len = Length();

      if (len < MIN_VALUE)
        len = 1.0;

      x /= (float) len;
      y /= (float) len;
      z /= (float) len;
      w /= (float) len;

      return *this;
    }

    // Multiply by scalar s
    Vector4 &operator*=( double s )
    {
      x *= (float) s;
      y *= (float) s;
      z *= (float) s;
      w *= (float) s;
      
      return *this;
    }

    // Add vector v2 to vector v1
    friend Vector4 operator+( Vector4 &v1, Vector4 &v2 )
    {
      Vector4 temp;     // Temporary 4-D vector

      temp.x = v1.x + v2.x;
      temp.y = v1.y + v2.y;
      temp.z = v1.z + v2.z;
      temp.w = v1.w + v2.w;

      return temp;
    }

    // Subtract vector v2 from vector v1
    friend Vector4 operator-( Vector4 &v1, Vector4 &v2 )
    {
      Vector4 temp;     // Temporary 4-D vector

      temp.x = v1.x - v2.x;
      temp.y = v1.y - v2.y;
      temp.z = v1.z - v2.z;
      temp.w = v1.w - v2.w;

      return temp;
    }

    // Return dot product of vectors v1 and v2
    friend double Dot( Vector4 &v1, Vector4 &v2 )
    { return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z +
        v1.w * v2.w); }

    // Premultiply point by projective matrix
    void ProjTransform( Point3 &p, double (*ptm)[4] )
    {
      x = (float) (ptm[0][0] * p.GetX() + ptm[0][1] *
          p.GetY() + ptm[0][2] * p.GetZ() + ptm[0][3]);
      y = (float) (ptm[1][0] * p.GetX() + ptm[1][1] *
          p.GetY() + ptm[1][2] * p.GetZ() + ptm[1][3]);
      z = (float) (ptm[2][0] * p.GetX() + ptm[2][1] *
          p.GetY() + ptm[2][2] * p.GetZ() + ptm[2][3]);
      w = (float) (ptm[3][0] * p.GetX() + ptm[3][1] *
          p.GetY() + ptm[3][2] * p.GetZ() + ptm[3][3]);
    }

    // Premultiply vector by projective matrix
    void ProjTransform( Vector3 &p, double (*ptm)[4] )
    {
      x = (float) (ptm[0][0] * p.GetX() + ptm[0][1] *
          p.GetY() + ptm[0][2] * p.GetZ() + ptm[0][3]);
      y = (float) (ptm[1][0] * p.GetX() + ptm[1][1] *
          p.GetY() + ptm[1][2] * p.GetZ() + ptm[1][3]);
      z = (float) (ptm[2][0] * p.GetX() + ptm[2][1] *
          p.GetY() + ptm[2][2] * p.GetZ() + ptm[2][3]);
      w = (float) (ptm[3][0] * p.GetX() + ptm[3][1] *
          p.GetY() + ptm[3][2] * p.GetZ() + ptm[3][3]);
    }

    // Perform perspective division on point
    void Perspective( Point3 *pp )
    {
      pp->SetX(x / w);
      pp->SetY(y / w);
      pp->SetZ(z / w);
    }

    // Perform perspective division on vector
    void Perspective( Vector3 *pp )
    {
      pp->SetX(x / w);
      pp->SetY(y / w);
      pp->SetZ(z / w);
    }
};

#endif

