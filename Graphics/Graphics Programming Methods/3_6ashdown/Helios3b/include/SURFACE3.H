////////////////////////////////////////////////////////////
//
//  SURFACE3.H - 3-D Surface Class
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

#ifndef _SURFACE3_H
#define _SURFACE3_H

#include "patch3.h"

class Surface3  // 3-D surface
{
  private:
    Spectra reflectance;    // Spectral reflectance
    Spectra emittance;      // Initial radiant exitance
    Patch3 *pplhd;          // Patch list head pointer
    Surface3 *pnext;        // Next surface pointer

  public:
    Surface3( Spectra reflect, Spectra emit )
    {
      reflectance = reflect;
      emittance = emit;

      pplhd = NULL;
      pnext = NULL;
    }

    ~Surface3()
    {
      Patch3 *pp = pplhd;
      Patch3 *ppnext;

      while (pp != NULL)        // Delete patches
      {
        ppnext = pp->GetNext();
        delete pp;
        pp = ppnext;
      }
    }

    Spectra &GetReflectance() { return reflectance; }
    Spectra &GetEmittance() { return emittance; }
    Patch3 *GetPatchPtr() { return pplhd; }
    Surface3 *GetNext() { return pnext; }
    void SetNext( Surface3 *pn ) { pnext = pn; }
    void SetPatchPtr( Patch3 *pp ) { pplhd = pp; }
};

#endif

