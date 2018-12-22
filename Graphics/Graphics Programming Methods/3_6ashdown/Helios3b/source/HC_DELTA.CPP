////////////////////////////////////////////////////////////
//
//  HC_DELTA.CPP - Hemicube Delta Form Factor Class
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

#include "hc_delta.h"

// Static delta form factor arrays
float HemiDelta::side_array[HC_ArrayDim][HC_ArrayDim];
float HemiDelta::top_array[HC_ArrayDim][HC_ArrayDim];

HemiDelta::HemiDelta()  // Class constructor
{
  int i, j;             // Loop indices
  double da;            // Cell area
  double dx, dy, dz;    // Cell dimensions
  double r, x, y, z;    // Cell co-ordinates

  // Initialize cell dimensions and area
  dx = dy = dz = 2.0 / (float) FF_ArrayRes;
  da = 4.0 / ((float) FF_ArrayRes * (float) FF_ArrayRes);

  // Calculate top face delta form factors
  x = dx / 2.0;
  for (i = 0; i < HC_ArrayDim; i++)
  {
    y = dy / 2.0;
    for (j = 0; j < HC_ArrayDim; j++)
    {
      r = x * x + y * y + 1.0;
      top_array[j][i] = (float) (da / (PI * r * r));
      y += dy;
    }
    x += dx;
  }

  // Calculate side face delta form factors
  x = dx / 2.0;
  for (i = 0; i < HC_ArrayDim; i++)
  {
    z = dz / 2.0;
    for (j = 0; j < HC_ArrayDim; j++)
    {
      r = x * x + z * z + 1.0;
      side_array[j][i] = (float) (z * da / (PI * r * r));
      z += dy;
    }
    x += dx;
  }
}

