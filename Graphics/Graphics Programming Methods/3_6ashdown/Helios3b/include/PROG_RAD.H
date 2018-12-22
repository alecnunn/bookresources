////////////////////////////////////////////////////////////
//
//  PROG_RAD.CPP - Progressive Refinement Radiosity Class
//                 Include File
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/11/26 - Added Interpolate function
//                         prototype.
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

#ifndef _PROG_RAD_H
#define _PROG_RAD_H

#include "environ.h"
#include "rad_eqn.h"

// NOTE: Either _HEMI_CUBE or _CUBIC_TETRA must be defined
//       in order to specify the appropriate form factor
//       determination class for FormFactor. This will
//       typically be done from the command line or through
//       the integrated development environment (IDE).

#if defined(_HEMI_CUBE)
#include "hemicube.h"
#elif defined(_CUBIC_TETRA)
#include "cubic_t.h"
#else
#error Either _HEMI_CUBE or _CUBIC_TETRA must be defined
#endif

// Progressive refinement radiosity equation solver
class ProgRad : public RadEqnSolve
{
  protected:
    float *ff_array;        // Form factor array pointer
    BOOL over_flag;         // Overshoot flag
    BOOL status;            // Object status
    FormFactor ffd;         // Form factor determination
    Spectra overshoot;      // Overshooting parameters

    void AddAmbient();
    void CalcOverShoot();

  public:
    ProgRad() : RadEqnSolve() { over_flag = TRUE; }

    ~ProgRad() { Close(); }

    BOOL Calculate();
    BOOL OverShootFlag() { return over_flag; }
    BOOL GetStatus() { return ffd.GetStatus(); }
    BOOL Open( Environ * );
    void Close();
    void DisableOverShoot() { over_flag = FALSE; }
    void EnableOverShoot() { over_flag = TRUE; }
    void Interpolate();
};

#endif

