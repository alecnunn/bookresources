////////////////////////////////////////////////////////////
//
//  RAY_CAST.H - Ray Cast Form Factor Class
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/07/21 - Version 1.02B release.
//              96/02/14 - Version 1.02C release.
//              96/04/01 - Version 1.03A release.
//				97/02/05 - Deleted "parse.h" include
//						   directive.
//						 - Added "instance.h" include
//                         directive.
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

#ifndef _RAY_CAST_H
#define _RAY_CAST_H

#include "instance.h"

// Maximum number of rays to be cast
static const int RC_NumRays = 4;

class RayCast   // Ray cast form factor determination
{
  private:
    double ray_area;            // Intersection area
    double src_area;            // Source patch area
    double selector;            // Triangle selector
    Patch3 *psrc;               // Source patch pointer
    Patch3 *pcache;             // Last occluding patch
    Vector3 end;                // Intersection vector
    Vector3 ray_dir;            // Ray direction
    Vector3 src_center;         // Source patch center
    Vector3 src_norm;           // Source patch normal
    Vector3 start;              // Receiver vertex vector
    Vector3 v0, v1, v2, v3;     // Vertex vectors

    void Select( Vector3 * );
    BOOL CheckOcclusion( Instance * );
    BOOL TestPatch( Patch3 * );

  public:
    void Init( Patch3 * );
    double CalcFormFactor( Vertex3 *, Instance * );
};

#endif

