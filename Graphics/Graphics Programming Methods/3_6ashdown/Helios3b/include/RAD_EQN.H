////////////////////////////////////////////////////////////
//
//  RAD_EQN.H - Radiosity Equation Solver Base Class Include
//              File
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/09/24 - Added start data member.
//                       - Added GetElapsed function.
//                       - Added GetElapsedTime function
//                         prototype.
//                       - Modified Open function.
//              94/11/26 - Added AddEmittance function
//                         prototype.
//                       - Changed Shade function to
//                         function prototype.
//                       - Deleted "tone_rep.h" include
//                         directive.
//                       - Deleted tone data member.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/07/21 - Version 1.02B release.
//              95/09/17 - Changed RadEqnSolve constructor
//                         to initialize max_step to 250.
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

#ifndef _RAD_EQN_H
#define _RAD_EQN_H

#include <time.h>
#include "environ.h"

class RadEqnSolve   // Radiosity equation solver
{
  protected:
    int step_count;         // Step count
    int max_step;           // Maximum number of steps
    double stop_criterion;  // Stopping criterion
    double convergence;     // Convergence
    double total_area;      // Total patch area
    double total_flux;      // Total environment flux
    double total_unsent;    // Total unsent exitance
    char timebuff[10];      // Calculation time string buff
    clock_t start;          // Calculation start time
    BOOL amb_flag;          // Ambient exitance flag
    Environ *penv;          // Environment pointer
    Patch3 *pmax;           // Maximum unsent flux patch ptr
    Spectra ambient;        // Ambient exitance
    Spectra irf;            // Interreflection factors

    clock_t GetElapsed() { return (clock() - start); }
    void AddEmittance();
    void CalcAmbient();
    void CalcInterReflect();
    void InitExitance();
    void UpdateUnsentStats();

  public:
    RadEqnSolve()
    {
      amb_flag = FALSE;
      max_step = 250;
      stop_criterion = 0.001;
    }

    virtual ~RadEqnSolve() { Close(); }

    BOOL AmbientFlag() { return amb_flag; }
    BOOL Calculate() { return TRUE; }
    BOOL GetStatus() { return TRUE; }

    BOOL Open( Environ * )
    {
      start = clock();
      return TRUE;
    }

    BOOL OverShootFlag() { return FALSE; }
    char *GetElapsedTime();
    double GetStopCriterion() { return stop_criterion; }
    double GetConvergence() { return convergence; }
    int GetMaxStep() { return max_step; }
    int GetStepCount() { return step_count; }
    void Close() { }
    void DisableAmbient() { amb_flag = FALSE; }
    void DisableOverShoot() { }
    void EnableAmbient() { amb_flag = TRUE; }
    void EnableOverShoot() { }
    void SetMaxStep( int max ) { max_step = max; }
    void SetStopCriterion( double s )
    { stop_criterion = s; }
    void Shade( Environ * );
};

#endif

