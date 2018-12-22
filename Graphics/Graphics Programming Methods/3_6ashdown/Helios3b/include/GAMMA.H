////////////////////////////////////////////////////////////
//
//  GAMMA.H - Gamma Correction Class
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

#ifndef _GAMMA_H
#define _GAMMA_H

#include "color.h"

static const int G_Domain = 256;        // Input domain
static const int G_Range = 256;         // Output range

class Gamma     // Gamma correction
{
  private:
    // Gamma correction lookup table
    static BYTE GammaTable[256];

    double g_value;     // Gamma value

    void InitTable()
    {
      int i;    // Loop index

      // Calculate gamma correction lookup table entries
      for (i = 0; i < G_Domain; i++)
        GammaTable[i] = (BYTE) ((double) G_Range *
            pow((double) i / (double) G_Domain, 1.0 /
            g_value));
    }

  public:
    Gamma( double g = 2.2 )
    {
      g_value = g;
      InitTable();
    }

    double GetGamma() { return g_value; }

    void Correct( ColorRGB &c )
    {
      c.SetRed(GammaTable[c.GetRed()]);
      c.SetGreen(GammaTable[c.GetGreen()]);
      c.SetBlue(GammaTable[c.GetBlue()]);
    }

    void SetGamma( double g )
    {
      g_value = g;
      InitTable();
    }
};

#endif

