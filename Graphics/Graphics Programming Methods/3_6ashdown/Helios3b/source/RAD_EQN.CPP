////////////////////////////////////////////////////////////
//
//  RAD_EQN.CPP - Radiosity Equation Solver Base Class
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/09/24 - Added GetElapsedTime function.
//              94/11/26 - Added AddEmittance and Shade
//                         functions.
//                       - Modified CalcAmbient function.
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

#include "rad_eqn.h"

// Initialize patch and element exitances
void RadEqnSolve::InitExitance()
{
  int i;                // Loop index
  int num_vert;         // Number of element vertices
  Instance *pinst;      // Instance pointer
  Element3 *pelem;      // Element pointer
  Patch3 *ppatch;       // Patch pointer
  Surface3 *psurf;      // Surface pointer
  Spectra emit;         // Surface emittance
  Vertex3 *pvert;       // Vertex pointer

  total_flux = 0.0;

  // Walk the instance list
  pinst = penv->GetInstPtr();
  while (pinst != NULL)
  {
    // Walk the surface list
    psurf = pinst->GetSurfPtr();
    while (psurf != NULL)
    {
      // Get surface emittance
      emit = psurf->GetEmittance();

      // Walk the patch list
      ppatch = psurf->GetPatchPtr();
      while (ppatch != NULL)
      {
        // Set patch unsent exitance
        ppatch->SetExitance(emit);

        // Update total environment flux
        total_flux += ppatch->GetUnsentFlux();

        // Walk the element list
        pelem = ppatch->GetElementPtr();
        while (pelem != NULL)
        {
          // Initialize element exitance
          pelem->GetExitance().Reset();

          num_vert = pelem->GetNumVert();
          for (i = 0; i < num_vert; i++)
          {
            // Get element vertex pointer
            pvert = pelem->GetVertexPtr(i);

            // Initialize vertex exitance
            pvert->GetExitance().Reset();
          }
          pelem = pelem->GetNext();
        }
        ppatch = ppatch->GetNext();
      }
      psurf = psurf->GetNext();
    }
    pinst = pinst->GetNext();
  }
}

// Update unsent flux statistics
void RadEqnSolve::UpdateUnsentStats()
{
  double curr_unsent;   // Current unsent flux
  double max_unsent;    // Maximum unsent flux
  Instance *pinst;      // Instance pointer
  Patch3 *ppatch;       // Patch pointer
  Surface3 *psurf;      // Surface pointer

  // Initialize unsent flux values
  total_unsent = 0.0;
  max_unsent = 0.0;

  // Walk the instance list
  pinst = penv->GetInstPtr();
  while (pinst != NULL)
  {
    // Walk the surface list
    psurf = pinst->GetSurfPtr();
    while (psurf != NULL)
    {
      // Walk the patch list
      ppatch = psurf->GetPatchPtr();
      while (ppatch != NULL)
      {
        // Get current unsent flux value
        curr_unsent = ppatch->GetUnsentFlux();

        // Update total unsent flux
        total_unsent += curr_unsent;

        // Update maximum unsent flux and patch pointer
        if (curr_unsent > max_unsent)
        {
          max_unsent = curr_unsent;
          pmax = ppatch;
        }
        ppatch = ppatch->GetNext();
      }
      psurf = psurf->GetNext();
    }
    pinst = pinst->GetNext();
  }

  // Update convergence value
  if (total_flux > MIN_VALUE)
    convergence = fabs(total_unsent) / total_flux;
  else
    convergence = 0.0;
}

// Calculate interreflection factors
void RadEqnSolve::CalcInterReflect()
{
  Instance *pinst;  // Instance pointer
  Patch3 *ppatch;   // Patch pointer
  Spectra sr;       // Surface reflectance
  Spectra sum;      // Sum areas times reflectances
  Surface3 *psurf;  // Surface pointer

  irf.Reset();
  sum.Reset();
  total_area = 0.0;

  // Walk the instance list
  pinst = penv->GetInstPtr();
  while (pinst != NULL)
  {
    // Walk the surface list
    psurf = pinst->GetSurfPtr();
    while (psurf != NULL)
    {
      // Walk the patch list
      ppatch = psurf->GetPatchPtr();
      while (ppatch != NULL)
      {
        // Update sum of patch areas times reflectances
        sr = ppatch->GetParentPtr()->GetReflectance();
        sr.Scale(ppatch->GetArea());
        sum.Add(sr);

        // Update sum of patch areas
        total_area += ppatch->GetArea();

        ppatch = ppatch->GetNext();
      }
      psurf = psurf->GetNext();
    }
    pinst = pinst->GetNext();
  }

  // Calculate area-weighted average reflectance
  sum.Scale(1.0 / total_area);

  // Calculate interreflection factors
  irf.SetRedBand((float) 1.0 / ((float) 1.0 -
      sum.GetRedBand()));
  irf.SetGreenBand((float) 1.0 / ((float) 1.0 -
      sum.GetGreenBand()));
  irf.SetBlueBand((float) 1.0 / ((float) 1.0 -
      sum.GetBlueBand()));
}

// Calculate ambient exitance
void RadEqnSolve::CalcAmbient()
{
  Instance *pinst;  // Instance pointer
  Patch3 *ppatch;   // Patch pointer
  Spectra sum;      // Sum areas times unsent exitances
  Spectra unsent;   // Patch unsent exitance
  Surface3 *psurf;  // Surface pointer

  sum.Reset();

  // Walk the instance list
  pinst = penv->GetInstPtr();
  while (pinst != NULL)
  {
    // Walk the surface list
    psurf = pinst->GetSurfPtr();
    while (psurf != NULL)
    {
      // Walk the patch list
      ppatch = psurf->GetPatchPtr();
      while (ppatch != NULL)
      {
        // Update sum of unsent exitances times areas
        unsent = ppatch->GetExitance();
        unsent.Scale(ppatch->GetArea());
        sum.Add(unsent);

        ppatch = ppatch->GetNext();
      }
      psurf = psurf->GetNext();
    }
    pinst = pinst->GetNext();
  }

  // Calculate area-weighted average unsent exitance
  sum.Scale(1.0 / total_area);

  // Calculate ambient exitance
  ambient = Mult(irf, sum);
}

// Get elapsed calculation time
char *RadEqnSolve::GetElapsedTime()
{
  unsigned long elapsed;        // Elapsed time in seconds
  unsigned long hrs;            // Hours
  unsigned long min;            // Minutes
  unsigned long sec;            // Seconds

  // Get elapsed calculation time
  elapsed = (unsigned long) GetElapsed() / CLOCKS_PER_SEC;

  hrs = elapsed / (60L * 60L);
  min = (elapsed / 60L) % 60L;
  sec = elapsed % 60L;

  // Format elapsed calculation time string
  sprintf(timebuff, "%lu:%02lu:%02lu", hrs, min, sec);

  return timebuff;
}

// Added surface emittance and determine maximum reflected
// color band value
void RadEqnSolve::AddEmittance()
{
  double cmax;          // Current color band value
  double rmax = 0.0;    // Maximum color band value
  Instance *pinst;      // Instance pointer
  Spectra emit;         // Surface emittance
  Vertex3 *pvert;       // Vertex pointer

  // Walk the instance list
  pinst = penv->GetInstPtr();
  while (pinst != NULL)
  {
    // Walk the vertex list
    pvert = pinst->GetVertPtr();
    while (pvert != NULL)
    {
      // Find maximum reflected color band value
      if ((cmax = pvert->GetExitance().GetMaxColor()) >
          rmax)
        rmax = cmax;

      // Get parent surface emittance
      emit = pvert->GetElemListPtr()->GetElemPtr()->
          GetParentPtr()->GetParentPtr()->GetEmittance();

      // Add surface initial exitance to reflected vertex
      // exitance
      pvert->GetExitance().Add(emit);

      pvert = pvert->GetNext();
    }
    pinst = pinst->GetNext();
  }

  // Set maximum reflected color band value
  penv->SetMaxReflect(rmax);
}

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

  // Set maximum reflected color band value
  penv->SetMaxReflect(1.0);
}

