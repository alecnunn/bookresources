////////////////////////////////////////////////////////////
//
//  ENVIRON.CPP - Environment Class
//
//  Version:    1.03B
//
//  History:    96/02/29 - Created.
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

#include "environ.h"

// Calculate environment extents
void Environ::CalcExtents()
{
  double vx, vy, vz;    // Vertex co-ordinates
  Instance *pinst;      // Instance pointer
  Vertex3 *pvert;       // Vertex pointer

  // Initialize extents
  max_x = max_y = max_z = -MAX_VALUE;
  min_x = min_y = min_z = MAX_VALUE;

  // Walk the instance list
  pinst = pinsthd;
  while (pinst != NULL)
  {
    // Walk the vertex list
    pvert = pinst->GetVertPtr();
    while (pvert != NULL)
    {

      // Get vertex co-ordinates
      vx = pvert->GetPosn().GetX();
      vy = pvert->GetPosn().GetY();
      vz = pvert->GetPosn().GetZ();

      // Update x-axis extents
      if (vx > max_x)
        max_x = vx;
      if (vx < min_x)
        min_x = vx;

      // Update y-axis extents
      if (vy > max_y)
        max_y = vy;
      if (vy < min_y)
        min_y = vy;

      // Update z-axis extents
      if (vz > max_z)
        max_z = vz;
      if (vz < min_z)
        min_z = vz;

      pvert = pvert->GetNext();
    }
    pinst = pinst->GetNext();
  }
}
