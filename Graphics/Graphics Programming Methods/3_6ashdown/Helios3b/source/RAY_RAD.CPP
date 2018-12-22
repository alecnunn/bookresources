////////////////////////////////////////////////////////////
//
//  RAY_RAD.CPP - Ray Casting Radiosity Class
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/09/24 - Modified Open function.
//              94/11/24 - Modified Calculate and AddAmbient
//                         functions.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/07/16 - Removed ambient exitance addition
//                         from Calculate function.
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

#include "ray_rad.h"

// Open ray casting radiosity renderer
BOOL RayRad::Open( Environ *pe )
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

  return TRUE;
} 

// Calculate element exitances
BOOL RayRad::Calculate()
{
  int i;                // Loop index
  int num_vert;         // Number of element vertices
  float vsff;           // Vertex-to-source form factor
  BOOL self;            // Self patch flag
  Element3 *pelem;      // Element pointer
  Instance *pinst;      // Instance pointer
  Patch3 *ppatch;       // Patch pointer
  Surface3 *psurf;      // Surface pointer
  Spectra p_delta;      // Patch delta exitance
  Spectra v_delta;      // Vertex delta exitance
  Spectra reflect;      // Surface reflectance
  Spectra shoot;        // Shoot exitance
  Vertex3 *pvert;       // Vertex pointer

  // Check for maximum number of steps
  if (step_count >= max_step)
    return TRUE;

  UpdateUnsentStats();  // Update unsent flux statistics

  // Check for convergence
  if (convergence < stop_criterion)
    return TRUE;

  // Initialize form factor determination object
  ffd.Init(pmax);

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
            // Get shooting patch unsent exitance
            shoot = pmax->GetExitance();

            // Reset patch delta exitance
            p_delta.Reset();

            num_vert = pelem->GetNumVert();
            for (i = 0; i < num_vert; i++)
            {
              // Get element vertex pointer
              pvert = pelem->GetVertexPtr(i);

              // Get vertex-to-source form factor
              if ((vsff = (float) ffd.CalcFormFactor(pvert,
                  penv->GetInstPtr())) > 0.0)
              {
                // Calculate vertex delta exitance
                v_delta = Mult(reflect, shoot);
                v_delta.Scale(vsff);

                // Update vertex exitance
                pvert->GetExitance().Add(v_delta);

                // Update patch delta exitance
                p_delta.Add(v_delta);
              }
            }

            // Update patch unsent exitance
            p_delta.Scale(pelem->GetArea() / ((double)
                num_vert * ppatch->GetArea()));
            ppatch->GetExitance().Add(p_delta);
          }
          pelem = pelem->GetNext();
        }
        ppatch = ppatch->GetNext();
      }
      psurf = psurf->GetNext();
    }
    pinst = pinst->GetNext();
  }

  // Reset unsent exitance to zero
  pmax->GetExitance().Reset();

  if (amb_flag == TRUE)
  {
    CalcAmbient();      // Recalculate ambient exitance
  }

  step_count++;         // Increment step count
  return FALSE;         // Convergence not achieved yet
}

void RayRad::AddAmbient()       // Add ambient exitance
{
  int i;                // Loop index
  int num_vert;         // Number of element vertices
  Element3 *pelem;      // Element pointer
  Instance *pinst;      // Instance pointer
  Patch3 *ppatch;       // Patch pointer
  Spectra delta_amb;    // Delta ambient exitance
  Spectra reflect;      // Surface reflectance
  Surface3 *psurf;      // Surface pointer
  Vertex3 *pvert;       // Vertex pointer

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

          num_vert = pelem->GetNumVert();
          for (i = 0; i < num_vert; i++)
          {
            // Get element vertex pointer
            pvert = pelem->GetVertexPtr(i);

            // Update vertex exitance
            pvert->GetExitance().Add(delta_amb);
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

