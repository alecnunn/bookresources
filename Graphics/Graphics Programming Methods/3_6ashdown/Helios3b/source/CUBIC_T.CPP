////////////////////////////////////////////////////////////
//
//  CUBIC_T.CPP - Cubic Tetrahedron Class
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/07/21 - Version 1.02B release.
//              96/02/14 - Version 1.02C release.
//              96/04/01 - Version 1.03A release.
//              96/11/17 - Added float typecast to
//                         ff_array initialization.
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

#include "cubic_t.h"

void CubicTetra::CalcFormFactors( Patch3 *pp, Instance
    *pi, float *ff_array, WORD num_elem )
{
  int i;                // Loop index
  BOOL hidden;          // Patch visibility flag
  BOOL self;            // Self patch flag
  WORD j;               // Loop index
  WORD elem_id;         // Element identifier
  Element3 *pelem;      // Element pointer
  Instance *pinst;      // Instance pointer
  Patch3 *ppatch;       // Patch pointer
  Surface3 *psurf;      // Surface pointer

  // Clear the form factors array
  for (j = 0; j < num_elem; j++)
    ff_array[j] = (float) 0.0;

  // Set the cubic tetrahedron view transformation matrix
  clip.SetView(pp);

  // Project environment onto each cubic tetrahedron face
  for (i = 0; i < CubicFaceNum; i++)
  {
    // Update view transformation matrix
    clip.UpdateView(i);

    scan.InitBuffer();  // Reinitialize depth buffer

    // Walk the instance list
    elem_id = 1;
    pinst = pi;
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
          self = (ppatch == pp) ? TRUE : FALSE;

          // Determine patch visibility 
          hidden = clip.BackFaceCull(ppatch);

          // Walk the element list
          pelem = ppatch->GetElementPtr();
          while (pelem != NULL)
          {
            if (hidden == FALSE && self == FALSE)
            {
              // Clip element to face view volume
              if (clip.Clip(pelem, out, elem_id) > 0)
              {
                scan.Scan(out);     // Scan convert polygon
              }
            }
            pelem = pelem->GetNext();
            elem_id++;
          }
          ppatch = ppatch->GetNext();
        }
        psurf = psurf->GetNext();
      }
      pinst = pinst->GetNext();
    }

    // Sum delta form factors
    scan.SumDeltas(ff_array);
  }
}

