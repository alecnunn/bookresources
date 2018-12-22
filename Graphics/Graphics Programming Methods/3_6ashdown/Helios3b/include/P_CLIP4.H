////////////////////////////////////////////////////////////
//
//  P_CLIP4.H - 4-D Polygon Clipper Class
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

#ifndef _P_CLIP4_H
#define _P_CLIP4_H

#include "out_poly.h"

enum Plane { Front, Back, Left, Right, Top, Bottom };

class ClipEdge              // Edge-plane clipper
{
  private:
    ClipEdge *pnext;        // Next clipper pointer
    Vector4 normal;         // Plane normal
    Vertex4 first;          // First vertex
    Vertex4 start;          // Start vertex
    BOOL first_inside;      // First vertex inside flag
    BOOL start_inside;      // Start vertex inside flag
    BOOL first_flag;        // First vertex seen flag

    BOOL IsInside( Vertex4 &v )
    { return (Dot(normal, v.GetCoord()) >= 0.0); }
    Vertex4 Intersect( Vertex4 &, Vertex4 & );
    void Output( Vertex4 &, OutPolygon & );

  public:
    ClipEdge() { first_flag = FALSE; }

    void Add( ClipEdge *pc ) { pnext = pc; }
    void Clip( Vertex4 &, OutPolygon & );
    void Close( OutPolygon & );
    void SetNormal( Vector4 &n ) { normal = n; }
};

class PolyClip4             // 4-D polygon clipper
{
  private:
    int num_vert;           // Number of output vertices
    ClipEdge clipper[6];    // Clipper array
    ClipEdge *pclip;        // Clipper list head pointer

  public:
    PolyClip4();

    int Clip( Element3 *, OutPolygon &, double (*)[4] );
};

#endif

