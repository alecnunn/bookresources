////////////////////////////////////////////////////////////
//
//  CT_DELTA.H- Cubic Tetrahedron Delta Form Factor Class
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

#ifndef _CT_DELTA_H
#define _CT_DELTA_H

#include "general.h"
#include "ff_delta.h"

// Delta form factor array size
static const CT_DeltaDim = FF_ArrayRes / 2;
static const int CT_FormDim = CT_DeltaDim * CT_DeltaDim +
    CT_DeltaDim;

// Cubic tetrahedron face co-ordinate limits
static const double CT_MinCoord = -2.0;
static const double CT_MaxCoord = 1.0;

class CubicDelta    // Cubic tetrahedron delta form factors
{
  private:
    // Delta form factor row pointer array
    static float *delta_array[CT_DeltaDim];

    // Delta form factor array
    static float ff_array[CT_FormDim];

  public:
    CubicDelta();

    // Get delta form factor
    float GetFactor( int row, int col )
    {
      int temp;         // Temporary variable

      if (row > col)
      {
        temp = row;
        row = col;
        col = temp;
      }

      return delta_array[row][col - row];
    }
};

#endif

