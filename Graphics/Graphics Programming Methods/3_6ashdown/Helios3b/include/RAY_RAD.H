////////////////////////////////////////////////////////////
//
//  RAY_RAD.H - Ray Casting Radiosity Class Include File
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/11/26 - Modified Close function.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/07/16 - Added ambient exitance addition
//                         to Close function.
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

#ifndef _RAY_RAD_H
#define _RAY_RAD_H

#include "environ.h"
#include "rad_eqn.h"
#include "ray_cast.h"

// Ray casting radiosity equation solver
class RayRad : public RadEqnSolve
{
  private:
    RayCast ffd;        // Form factor determination

    void AddAmbient();

  public:
    RayRad() : RadEqnSolve() { }

    ~RayRad() { }

    BOOL Calculate();
    BOOL Open( Environ * );
    void Close()
    {
      if (amb_flag == TRUE)
        AddAmbient();
      AddEmittance();
    }
};

#endif

