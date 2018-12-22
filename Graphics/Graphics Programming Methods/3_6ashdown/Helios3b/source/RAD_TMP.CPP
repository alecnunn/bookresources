////////////////////////////////////////////////////////////
//
//  RAD_TMP.H - Dummy Radiosity Equation Solver Base Class
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/09/24 - Added GetElapsedTime function.
//              94/11/26 - Added Solve function.
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

// NOTE: This file provides TEMPORARY function stubs for the
//       RadEqnSolve class.

#include "rad_eqn.h"

char *RadEqnSolve::GetElapsedTime() { return "0:00:00"; }
void RadEqnSolve::CalcAmbient() { }
void RadEqnSolve::CalcInterReflect() { }
void RadEqnSolve::InitExitance() { }
void RadEqnSolve::UpdateUnsentStats() { }

// Shade vertex exitances
void RadEqnSolve::Shade( Environ *penv )
{
  Instance *pinst;
  Vertex3 *pvert;

  // Walk the instance list
  pinst = penv->GetInstPtr();
  while (pinst != NULL)
  {
    // Walk the vertex list
    pvert = pinst->GetVertPtr();
    while (pvert != NULL)
    {
      // Set vertex exitance to parent surface reflectance
      pvert->SetExitance(pvert->GetElemListPtr()->
          GetElemPtr()->GetParentPtr()->GetParentPtr()->
          GetReflectance());

      pvert = pvert->GetNext();
    }
    pinst = pinst->GetNext();
  }
}

