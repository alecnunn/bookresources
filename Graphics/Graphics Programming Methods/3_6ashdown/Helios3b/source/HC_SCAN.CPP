////////////////////////////////////////////////////////////
//
//  HC_SCAN.CPP - Hemicube Scan Conversion Class
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
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

#include "hc_poly.h"
#include "hc_scan.h"

HemiScan::HemiScan()    // Class constructor
{
  int row;      // Loop index
  
  status = TRUE;        // Initialize object status

  // Allocate edge list
  if ((edge_list = new FormEdgeInfo[FF_ArrayRes]) != NULL)
  {
    // Allocate cell information buffer
    if ((cell_buffer = new (FormCellInfo (*[FF_ArrayRes])))
        != NULL)
    {
      for (row = 0; row < FF_ArrayRes; row++)
      {
        if ((cell_buffer[row] =
            new FormCellInfo[FF_ArrayRes]) == NULL)
        {
          // Release partially allocated buffer
          row--;
          for ( ; row >= 0; row--)
            delete [] cell_buffer[row];
          delete [] cell_buffer;

          // Release edge list memory 
          delete [] edge_list;

          status = FALSE;
          break;
        }
      }
    }
  }
  else
  {
    delete [] edge_list;        // Release edge list memory
    status = FALSE;
  }
}

HemiScan::~HemiScan()   // Class destructor
{
  int row;      // Loop index
  
  delete [] edge_list;          // Release edge list memory

  // Delete cell information buffer
  for (row = 0; row < FF_ArrayRes; row++)
    delete [] cell_buffer[row];
  delete [] cell_buffer;
}

// Initialize cell information buffer
void HemiScan::InitBuffer()
{
  int row, col;     // Loop indices

  for (row = 0; row < FF_ArrayRes; row++)
    for (col = 0; col < FF_ArrayRes; col++)
    {
      cell_buffer[row][col].depth = FF_Infinity;
      cell_buffer[row][col].id = FF_None;
    }
}

void HemiScan::DrawEdgeList()   // Draw edge list
{
  int x, y;             // Loop indices
  int sx, ex;           // Scan line x-axis co-ordinates
  double dz;            // Pseudodepth delta
  double iz;            // Element pseudodepth
  double x_dist;        // X-axis distance
  FormEdgeInfo *pedge;  // Edge info pointer
  FormScanInfo *pss;    // Scan line start info pointer
  FormScanInfo *pse;    // Scan line end info pointer
  FormScanInfo *psw;    // Swap scan line info pointer

  pedge = &(edge_list[ymin]);
  for (y = ymin; y < ymax; y++)
  {
    // Get scan line info pointers
    pss = &(pedge->isect[0]);
    pse = &(pedge->isect[1]);

    if (pss->x > pse->x)
    {
      // Swap scan line info pointers
      psw = pss; pss = pse; pse = psw;
    }

    // Get scan line x-axis co-ordinates
    sx = (int) pss->x;
    ex = (int) pse->x;

    if (sx < ex)        // Ignore zero-length segments
    {
      // Determine scan line start info
      iz = pss->z;

      // Determine inverse slopes
      x_dist = pse->x - pss->x;

      dz = (pse->z - iz) / x_dist;

      // Enter scan line
      for (x = sx; x < ex; x++)
      {
        // Check element visibility
        if (iz < (double) cell_buffer[y][x].depth)
        {
          // Update Z-buffer
          cell_buffer[y][x].depth = (float) iz;

          // Set polygon identifier
          cell_buffer[y][x].id = poly_id;
        }

        // Update element pseudodepth
        iz += dz;
      }
    }
    pedge++;    // Point to next edge list element
  }
}

// Sum delta form factors
void HemiScan::SumDeltas( float *ff_array, int face_id )
{
  WORD poly_id;     // Polygon identifier
  int row, col;     // Face cell indices

  if (face_id == HC_TopFace)
  {
    // Scan entire face buffer
    for (row = 0; row < FF_ArrayRes; row++)
      for (col = 0; col < FF_ArrayRes; col++)
      {
        if ((poly_id = cell_buffer[row][col].id) !=
            FF_None)
          ff_array[poly_id - 1] +=
              dff.GetTopFactor(row, col);
      }
  }
  else
  {
    // Scan upper half of face buffer only
    for (row = HC_ArrayDim; row < FF_ArrayRes; row++)
      for (col = 0; col < FF_ArrayRes; col++)
      {
        if ((poly_id = cell_buffer[row][col].id) != FF_None)
          ff_array[poly_id - 1] +=
              dff.GetSideFactor(row, col);
      }
  }
}

