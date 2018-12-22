////////////////////////////////////////////////////////////
//
/// OUT_POLY.H - Output Polygon Class
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

#ifndef _OUT_POLY_H
#define _OUT_POLY_H

#include "vertex4.h"

// Maximum  number of output vertices
static const int MaxOutVert = 10;

class OutPolygon        // Output polygon
{
  private:
    class OutVertex     // Output vertex
    {
      private:
        Spectra color;  // Color
        Point3 posn;    // 3-D position

      public:
        Point3 &GetPosn() { return posn; }
        Spectra &GetColor() { return color; }

        void Set( Vertex4 &v )
        {
          // Perform perspective division
          v.GetCoord().Perspective(&posn);

          color = v.GetColor();
        }
    }
    vertex[MaxOutVert];     // Output vertex array
    int num_vert;           // Number of vertices

    void AddVertex( Vertex4 &v )
    { vertex[num_vert++].Set(v); }
    void Reset() { num_vert = 0; }

    friend class ClipEdge;
    friend class PolyClip4;

  public:
    OutPolygon() { num_vert = 0; }

    int GetNumVert() { return num_vert; }
    Point3 &GetVertexPosn( int i )
    { return vertex[i].GetPosn(); }
    Spectra &GetVertexColor( int i )
    { return vertex[i].GetColor(); }
};

#endif

