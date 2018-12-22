////////////////////////////////////////////////////////////
//
//  HC_DELTA.H - Hemicube Delta Form Factor Class
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

#ifndef _HC_DELTA_H
#define _HC_DELTA_H

#include "general.h"
#include "ff_delta.h"

static const int HC_ArrayDim = FF_ArrayRes / 2;

class HemiDelta         // Hemicube delta form factors
{
  private:
    static float side_array[HC_ArrayDim][HC_ArrayDim];
    static float top_array[HC_ArrayDim][HC_ArrayDim];

  public:
    HemiDelta();

    // Get top face cell form factor
    float GetTopFactor( int row, int col )
    {
      if (row >= HC_ArrayDim)
        row -= HC_ArrayDim;
      else
        row = HC_ArrayDim - row - 1;

      if (col >= HC_ArrayDim)
        col -= HC_ArrayDim;
      else
        col = HC_ArrayDim - col - 1;

      return top_array[row][col];
    }

    // Get side face cell form factor
    float GetSideFactor( int row, int col )
    {
      if (col >= HC_ArrayDim)
        col -= HC_ArrayDim;
      else
        col = HC_ArrayDim - col - 1;

      return side_array[row - HC_ArrayDim][col];
    }
};

#endif

