////////////////////////////////////////////////////////////
//
//  C_JITTER.CPP - Color Reduction Filter Class
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/07/21 - Version 1.02B release.
//              96/02/14 - Version 1.02C release.
//              96/04/01 - Version 1.03A release.
//              96/11/23 - Added BYTE typecast to assignment
//                         in Reduce.
//              97/02/23 - Changed default noise level to 0.
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

// Adapted from: Bragg, D. [1992]. "A Simple Color Reduction 
//               Filter", in Graphics Gems III (D. Kirk, 
//               Ed.), Academic Press, San Diego, CA, 20 - 
//               22, 429 - 431

#include "c_jitter.h"

ColorJitter::ColorJitter()      // Class constructor
{
  int i;    // Loop index

  status = TRUE;

  // Initialize jitter lookup table pointers
  pirand = NULL;
  pxrand = NULL;
  pyrand = NULL;
  
  noise = 0;    // Set default noise level

  // Allocate jitter lookup tables
  if ((pirand = new int[C_TableSize]) == NULL)
  {
    status = FALSE;
    return;
  }

  if ((pxrand = new double[C_TableSize]) == NULL)
  {
    status = FALSE;
    return;
  }

  if ((pyrand = new double[C_TableSize]) == NULL)
  {
    status = FALSE;
    return;
  }

  // Initialize jitter lookup tables
  for (i = 0; i < C_TableSize; i++)
  {
    pirand[i] = (int) ((double) C_TableSize * ((double)
        (rand() % C_LargeNum) / (double) C_LargeNum));
    pxrand[i] = (double) (rand() % C_LargeNum) / (double)
        C_LargeNum;
    pyrand[i] = (double) (rand() % C_LargeNum) / (double)
        C_LargeNum;
  }
}

ColorJitter::~ColorJitter()     // Class destructor
{
  // Release jitter lookup tables
  if (pirand != NULL)
    delete [] pirand;

  if (pxrand != NULL)
    delete [] pxrand;

  if (pyrand != NULL)
    delete [] pyrand;
}

// Perform color reduction by jittering color values
void ColorJitter::Reduce( ColorRGB *pc, int x, int y )
{
  int i;            // Loop index
  int p, q;         // Temporary variables
  BYTE color[3];    // Color band values

  // Get color band values
  color[0] = pc->GetRed();
  color[1] = pc->GetGreen();
  color[2] = pc->GetBlue();

  for (i = 0; i < 3; i++)
  {
    if (color[i] < 248)
    {
      // Map color band value to one of 32 possible output
      // values and determine remainder
      p = (int) (color[i] % 8);

      // Look up random jitter value based on color band
      // index and pixel x-y co-ordinates
      q = (int) (JitterX(x, y, i) * 9.0);

      // Jitter color band value
      if (p >= q)
        color[i] += (BYTE) 8;

      // Calculate second jitter value and add to color
      // band value
      q = 8 * ((int) ((JitterY(x, y, i) * (double) (2 *
          noise)) + 0.5) - noise) + (int) color[i];

      // Ensure jittered color band value is within
      // allowable range
      if (q >= 0 && q <= 255)
        color[i] = (BYTE) q;
    }

    // Mask off lowest three bits to create 5-bit value
    color[i] &= 0xf8;
  }

  // Set jittered color band values
  pc->SetRed(color[0]);
  pc->SetGreen(color[1]);
  pc->SetBlue(color[2]);
}

