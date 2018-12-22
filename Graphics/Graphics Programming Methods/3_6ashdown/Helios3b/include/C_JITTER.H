////////////////////////////////////////////////////////////
//
//  C_JITTER.H - Color Reduction Filter Class
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

#ifndef _C_JITTER_H
#define _C_JITTER_H

// Adapted from: Bragg, D. [1992]. "A Simple Color Reduction 
//               Filter", in Graphics Gems III (D. Kirk, 
//               Ed.), Academic Press, San Diego, CA, 20 - 
//               22, 429 - 431

#include <stdlib.h>
#include "color.h"

static const int C_LargeNum = 1024;
static const int C_TableSize = 1024;
static const int C_Mask = C_TableSize - 1;

class ColorJitter       // Color reduction filter
{
  private:
    double *pxrand;     // Jitter lookup table pointer
    double *pyrand;     // Jitter lookup table pointer
    int noise;          // Noise level ( 0 - 8 )
    int *pirand;        // Jitter lookup table pointer
    BOOL status;        // Object status

    double JitterX( int x, int y, int band )
    {
      return pxrand[((x + (y << 2)) + pirand[(x + band) &
          C_Mask]) & C_Mask];
    }

    double JitterY( int x, int y, int band )
    {
      return pyrand[((y + (x << 2)) + pirand[(y + band) &
          C_Mask]) & C_Mask];
    }

  public:
    ColorJitter();

    ~ColorJitter();

    BOOL GetStatus() { return status; }
    int GetNoiseLevel() { return noise; }
    void SetNoiseLevel( int n ) { noise = n % 9; }
    void Reduce( ColorRGB *, int, int );
};

#endif

