////////////////////////////////////////////////////////////
//
//  CT_DELTA.CPP- Cubic Tetrahedron Delta Form Factor Class
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/07/21 - Version 1.02B release.
//              96/02/14 - Version 1.02C release.
//              96/04/01 - Version 1.03A release.
//				96/12/08 - Removed top and bottom local
//						   variables from CubicDelta
//						   constructor.
//						 - Modified CubicDelta constructor
//						   to precalculate square roots.
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

#include "ct_delta.h"

// Static delta form factor row pointer array
float *CubicDelta::delta_array[CT_DeltaDim];

// Static delta form factor array
float CubicDelta::ff_array[CT_FormDim];

CubicDelta::CubicDelta()        // Class constructor
{
  int i = 0;                // Form factor array index
  int left, right;          // Index boundaries
  int row, col;             // Current indices
  double delta;             // Cell width
  double diag_delta;        // Diagonal cell width
  double area;              // Cell area
  double diag_area;         // Diagonal cell area
  double y, z;              // Cell center
  double diag_y, diag_z;    // Diagonal cell center
  double r2;                // Cell distance squared
  double sqrt_3;			// Square root of three

  // Calculate square root of three
  sqrt_3 = sqrt(3.0);

  // Initialize index boundaries
  left = 0;
  right = FF_ArrayRes - 1;

  // Initialize cell values
  delta = (CT_MaxCoord - CT_MinCoord) / FF_ArrayRes;
  diag_delta = delta / 2.0;
  area  = delta * delta;
  diag_area  = area / 2.0;
  y = z = CT_MaxCoord - diag_delta;

  // Calculate delta form factors
  for (row = 0; row < CT_DeltaDim; row++)
  {
    // Save delta form factor array row pointer
    delta_array[row] = &(ff_array[i]);

    for (col = left; col < right; col++)
    {
      // Calculate square of cell distance
      r2 = y * y + z * z + 1;

      // Calculate cell delta form factor
      ff_array[i++] = (float) (area * (y + z + 1) / (PI * r2
          * r2 * sqrt_3));

      y -= delta;
    }

    // Calculate square of diagonal cell distance
    diag_y = y + diag_delta;
    diag_z = z + diag_delta;
    r2 = diag_y * diag_y + diag_z * diag_z + 1;

    // Calculate diagonal cell delta form factor
    ff_array[i++] = (float) (diag_area * (diag_y + diag_z +
        1) / (PI * r2 * r2 * sqrt_3));

    left++;
    right--;
    y = z -= delta;
  }
}

