////////////////////////////////////////////////////////////
//
//  FF_SCAN.CPP - Form Factor Scan Conversion Class
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

#include "ff_delta.h"
#include "ff_scan.h"

// Scan convert polygon
void FormScan::Scan( FormPoly &poly )
{
  poly_id = poly.GetPolyId();   // Get polygon identifier
  GetVertexInfo(poly);          // Get vertex information
  ScanEdges();                  // Scan convert edges
  DrawEdgeList();               // Draw edge list
}

// Get vertex information
void FormScan::GetVertexInfo( FormPoly &poly )
{
  int i;                // Loop index
  FormVertexInfo *pv;   // Vertex info element pointer
  Point3 posn;          // Normalized vertex position

  // Initialize polygon y-axis limits
  ymax = 0;
  ymin = FF_ArrayRes - 1;

  // Get number of vertices
  num_vert = poly.GetNumVert();

  for (i = 0; i < num_vert; i++)
  {
    pv = &(v_info[i]);  // Get vertex info element pointer

    // Get vertex normalized view space co-ordinates
    posn = poly.GetVertex(i);

    // Scale view space u-v co-ordinates
    pv->posn.SetX(posn.GetX() * FF_ArrayRes);
    pv->posn.SetY(posn.GetY() * FF_ArrayRes);
    pv->posn.SetZ(posn.GetZ());

    // Convert to cell array x-y co-ordinates
    pv->face.x = (int) pv->posn.GetX();
    pv->face.y = (int) pv->posn.GetY();

    // Update polygon y-axis limits
    if (pv->face.y < ymin)
      ymin = pv->face.y;
    if (pv->face.y > ymax)
      ymax = pv->face.y;
  }
}

void FormScan::ScanEdges()      // Scan convert edges
{
  int i, j;             // Loop indices
  double dx;            // X-axis delta
  double dz;            // Pseudodepth delta
  double ix;            // Intersection X-axis co-ordinate
  double iz;            // Intersection pseudodepth
  double y_dist;        // Y-axis distance
  FormEdgeInfo *pedge;  // Edge info pointer
  FormScanInfo *pscan;  // Scan line info pointer
  FormVertexInfo *psv;  // Start vertex info pointer
  FormVertexInfo *pev;  // End vertex info pointer
  FormVertexInfo *psw;  // Swap vertex info pointer

  // Initialize edge list
  for (i = ymin; i < ymax; i++)
    edge_list[i].first = FALSE;

  for (i = 0; i < num_vert; i++)
  {
    // Get edge vertex pointers
    psv = &(v_info[i]);
    pev = &(v_info[(i + 1) % num_vert]);
    
    if (psv->face.y == pev->face.y)
    {
      continue;         // Ignore horizontal edges
    }

    if (psv->face.y > pev->face.y)
    {
      // Swap edge vertex pointers
      psw = psv; psv = pev; pev = psw;
    }

    // Get start vertex info
    ix = psv->posn.GetX();
    iz = psv->posn.GetZ();

    // Determine inverse slopes
    y_dist = (double) (pev->face.y - psv->face.y);

    dx = (pev->posn.GetX() - ix) / y_dist;
    dz = (pev->posn.GetZ() - iz) / y_dist;

    // Scan convert edge
    pedge = &(edge_list[psv->face.y]);
    for (j = psv->face.y; j < pev->face.y; j++)
    {
      // Determine intersection info element
      if (pedge->first == FALSE)
      {
        pscan = &(pedge->isect[0]);
        pedge->first = TRUE;
      }
      else
        pscan = &(pedge->isect[1]);

      // Insert edge intersection info
      pscan->x = ix;
      pscan->z = iz;

      // Update edge intersection info
      ix += dx;
      iz += dz;

      pedge++;  // Point to next edge list element
    }
  }
}

