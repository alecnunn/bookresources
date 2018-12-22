////////////////////////////////////////////////////////////
//
//  PROG_RAD.CPP - Progressive Refinement Radiosity Class
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/09/24 - Modified Open function.
//              94/11/26 - Added Interpolate function.
//                       - Modified Close, Calculate,
//                         AddAmbient and CalcOverShoot
//                         functions.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/07/16 - Removed ambient exitance addition
//                         from Calculate function.
//                       - Added ambient exitance addition
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

#include "prog_rad.h"

// Open progressive radiosity renderer
BOOL ProgRad::Open( Environ *pe )
{
  penv = pe;            // Save environment pointer
  step_count = 0;       // Reset step count
  convergence = 1.0;    // Convergence
  start = clock();      // Initialize start time
  InitExitance();       // Initialize exitances

  if (amb_flag == TRUE) // Ambient exitance required ?
  {
    CalcInterReflect(); // Calculate interreflection factor
    CalcAmbient();      // Calculate initial ambient term
  }

  // Allocate form factor array
  if ((ff_array = new float[penv->GetNumElem()]) == NULL)
    return FALSE;

  return TRUE;
} 

// Close progressive radiosity renderer
void ProgRad::Close()
{
  // Release form factor array
  if (ff_array != NULL)
  {
    delete [] ff_array;
    ff_array = NULL;
  }

  if (penv != NULL)
  {
    if (amb_flag == TRUE)
    {
      AddAmbient();     // Add ambient exitance
    }

    Interpolate();      // Interpolate vertex exitances
    AddEmittance();     // Add parent surface emittances
  }
}

// Calculate element exitances
BOOL ProgRad::Calculate()
{
  float rff;            // Reciprocal form factor
  BOOL self;            // Self patch flag
  WORD ff_index = 0;    // Form factor array index
  Element3 *pelem;      // Element pointer
  Instance *pinst;      // Instance pointer
  Patch3 *ppatch;       // Patch pointer
  Surface3 *psurf;      // Surface pointer
  Spectra delta;        // Delta exitance
  Spectra reflect;      // Surface reflectance
  Spectra shoot;        // Shoot exitance

  // Check for maximum number of steps
  if (step_count >= max_step)
    return TRUE;

  UpdateUnsentStats();  // Update unsent flux statistics

  // Check for convergence
  if (convergence < stop_criterion)
    return TRUE;

  // Calculate form factors
  ffd.CalcFormFactors(pmax, penv->GetInstPtr(), ff_array,
      penv->GetNumElem());

  if (over_flag == TRUE)
  {
    CalcOverShoot();    // Calculate overshooting parameters
  }

  // Walk the instance list
  pinst = penv->GetInstPtr();
  while (pinst != NULL)
  {
    // Walk the surface list
    psurf = pinst->GetSurfPtr();
    while (psurf != NULL)
    {
      // Get surface reflectance
      reflect = psurf->GetReflectance();

      // Walk the patch list
      ppatch = psurf->GetPatchPtr();
      while (ppatch != NULL)
      {
        // Check for self patch
        self = (ppatch == pmax) ? TRUE : FALSE;

        // Walk the element list
        pelem = ppatch->GetElementPtr();
        while (pelem != NULL)
        {
          if (self == FALSE)    // Ignore self
          {
            // Check element visibility
            if (ff_array[ff_index] > 0.0)
            {
              // Compute reciprocal form factor
              rff = (float) min((double) ff_array[ff_index]
                  * pmax->GetArea() / pelem->GetArea(),
                  1.0);

              // Get shooting patch unsent exitance
              shoot = pmax->GetExitance();

              if (over_flag == TRUE)
              {
                // Add overshoot exitance
                shoot.Add(overshoot);
              }

              // Calculate delta exitance
              delta = Mult(reflect, shoot);
              delta.Scale(rff);

              // Update element exitance
              pelem->GetExitance().Add(delta);

              // Update patch unsent exitance
              delta.Scale(pelem->GetArea() /
                  ppatch->GetArea());
              ppatch->GetExitance().Add(delta);
            }
          }
          pelem = pelem->GetNext();
          ff_index++;
        }
        ppatch = ppatch->GetNext();
      }
      psurf = psurf->GetNext();
    }
    pinst = pinst->GetNext();
  }

  if (over_flag == TRUE)
  {
    // Subtract shot exitance from patch unsent exitance
    pmax->GetExitance().Subtract(shoot);
  }
  else
  {
    // Reset unsent exitance to zero
    pmax->GetExitance().Reset();
  }

  if (amb_flag == TRUE)
  {
    CalcAmbient();      // Recalculate ambient exitance
  }

  step_count++;         // Increment step count
  return FALSE;         // Convergence not achieved yet
}

// Interpolate vertex reflected exitances
void ProgRad::Interpolate()
{
  int num_elem;         // Number of elements
  Element3 *pelem;      // Element pointer
  ElemList *pel;        // Element list pointer
  Instance *pinst;      // Instance pointer
  Vertex3 *pvert;       // Vertex pointer

  // Walk the instance list
  pinst = penv->GetInstPtr();
  while (pinst != NULL)
  {
    // Walk the vertex list
    pvert = pinst->GetVertPtr();
    while (pvert != NULL)
    {
      // Initialize vertex reflected exitance
      pvert->GetExitance().Reset();

      // Walk the element list
      pel = pvert->GetElemListPtr();
      num_elem = 0;
      while (pel != NULL)
      {
        // Get the element pointer
        pelem = pel->GetElemPtr();

        // Add element reflected exitance
        pvert->GetExitance().Add(pelem->GetExitance());

        pel = pel->GetNext();
        num_elem++;
      }

      // Scale vertex reflected exitance according to number
      // of shared elements
      pvert->GetExitance().Scale(1.0 / (double) num_elem);

      pvert = pvert->GetNext();
    }
    pinst = pinst->GetNext();
  }
}

void ProgRad::AddAmbient()      // Add ambient exitance
{
  Element3 *pelem;      // Element pointer
  Instance *pinst;      // Instance pointer
  Patch3 *ppatch;       // Patch pointer
  Spectra delta_amb;    // Delta ambient exitance
  Spectra reflect;      // Surface reflectance
  Surface3 *psurf;      // Surface pointer

  // Walk the instance list
  pinst = penv->GetInstPtr();
  while (pinst != NULL)
  {
    // Walk the surface list
    psurf = pinst->GetSurfPtr();
    while (psurf != NULL)
    {
      // Get surface reflectance
      reflect = psurf->GetReflectance();

      // Walk the patch list
      ppatch = psurf->GetPatchPtr();
      while (ppatch != NULL)
      {
        // Walk the element list
        pelem = ppatch->GetElementPtr();
        while (pelem != NULL)
        {
          // Calculate delta ambient exitance
          delta_amb = Mult(ambient, reflect);

          // Update element exitance
          pelem->GetExitance().Add(delta_amb);

          pelem = pelem->GetNext();
        }
        ppatch = ppatch->GetNext();
      }
      psurf = psurf->GetNext();
    }
    pinst = pinst->GetNext();
  }
}

// Calculate overshooting parameters
void ProgRad::CalcOverShoot()
{
  BOOL self;            // Self patch flag
  WORD ff_index = 0;    // Form factor array index
  Element3 *pelem;      // Element pointer
  Instance *pinst;      // Instance pointer
  Patch3 *ppatch;       // Patch pointer
  Spectra spr;          // Shooting patch reflectance
  Spectra unsent;       // Patch unsent exitance
  Surface3 *psurf;      // Surface pointer

  overshoot.Reset();    // Reset overshooting parameters

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
        // Check for self patch
        self = (ppatch == pmax) ? TRUE : FALSE;

        // Walk the element list
        pelem = ppatch->GetElementPtr();
        while (pelem != NULL)
        {
          if (self == FALSE)    // Ignore self
          {
            // Get unsent patch exitance
            unsent = ppatch->GetExitance();
                
            // Limit unsent exitance to positive color band
            // values
            unsent.Limit();

            // Multiply unsent exitance by patch-to-
            // element form factor
            unsent.Scale(ff_array[ff_index]);

            // Update overshooting parameters
            overshoot.Add(unsent);
          }
          pelem = pelem->GetNext();
          ff_index++;
        }
        ppatch = ppatch->GetNext();
      }
      psurf = psurf->GetNext();
    }
    pinst = pinst->GetNext();
  }

  // Get shooting patch reflectance
  spr = pmax->GetParentPtr()->GetReflectance();

  // Multiply overshooting parameters by shooting patch
  // reflectance
  overshoot.Mult(spr);
}

