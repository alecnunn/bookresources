////////////////////////////////////////////////////////////
//
//  VERTEX4.H - 4-D Vertex Class
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

#ifndef _VERTEX4_H
#define _VERTEX4_H

#include "patch3.h"
#include "vector4.h"

class Vertex4   // 4-D homogeneous co-ordinates vertex
{
  private:
    Spectra color;      // Color
    Vector4 coord;      // 4-D homogeneous co-ordinates

  public:
    Spectra &GetColor() { return color; }
    Vector4 &GetCoord() { return coord; }

    void Set( Point3 &p, Spectra &c, double (*ptm)[4] )
    {
      // Perform projective transformation
      coord.ProjTransform(p, ptm);
      
      color = c;
    }

    void Set( Vector4 &v, Spectra &c )
    { coord = v; color = c; }
};

#endif

