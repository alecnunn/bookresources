////////////////////////////////////////////////////////////
//
//  CT_CLIP.H - Cubic Tetrahedron Polygon Clipper Class
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

#ifndef _CT_CLIP_H
#define _CT_CLIP_H

#include "ct_poly.h"
#include "ff_clip.h"

// Clipping plane identifiers
enum CT_Plane { CT_Front, CT_Left, CT_Bottom, CT_Diag };

// Cubic tetrahedron polygon clipper
class CubicClip : public FormClip
{
  private:
    void BuildTransform( Vector3 &, Vector3 &, Vector3 & );

  public:
    CubicClip();

    void SetView( Patch3 * );
    void UpdateView( int );
};

#endif

