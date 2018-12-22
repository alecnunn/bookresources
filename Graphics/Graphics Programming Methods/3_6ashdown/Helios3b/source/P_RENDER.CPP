////////////////////////////////////////////////////////////
//
//  P_RENDER.CPP - Polygon Renderer Class
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/11/26 - Added contrast and intensity
//                         adjustment to GetVertexInfo.
//                       - Added vertex color scaling and
//                         clipping to GetVertexInfo.
//                       - Modified Open function to
//                         calculate color scaling factor.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/03/21 - Modified Render and GetVertexInfo
//                         functions to accept subpixel
//                         offsets for antialiasing.
//              95/03/22 - Added Reset function.
//              95/03/25 - Removed gamma correction and
//                         color jittering from DrawEdgeList
//                         function.
//              95/06/05 - Modified DrawEdgeList and
//                         ScanEdges functions to convert
//                         pseudodepth to long data type.
//                       - Added EdgeSetup, EdgeScan,
//                         SpanSetup and SpanScan functions.
//                       - Changed DrawEdgeList and
//                         ScanEdges functions from floating
//                         point to integer and long data
//                         types for DDA caculations.
//              95/06/30 - Modified SpanScan function to use
//                         bitmap pixel functions.
//              95/07/05 - Modified Render function to
//                         accept supersampling rate
//                         parameter and initialize ss_rate
//                         data member.
//                       - Modified GetVertexInfo, EdgeScan
//                         and SpanScan to use supersampling
//                         bitmap.
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

#include "p_render.h"

// Open polygon renderer
BOOL PolyRender::Open( WinBitmap *pb, double rmax )
{
  int row, col;         // Loop indices

  // Calculate color scaling factor
  CalcColorScale(rmax);

  pbmap = pb;   // Save bitmap object pointer

  height = pbmap->GetHeight();
  width = pbmap->GetWidth();

  // Allocate edge list
  if ((edge_list = new EdgeInfo[height]) == NULL)
    return FALSE;

  // Allocate depth buffer
  if ((z_buffer = new (long (*[height]))) != NULL)
  {
    for (row = 0; row < height; row++)
    {
      if ((z_buffer[row] = new long[width]) == NULL)
      {
        // Release partially allocated depth buffer
        row--;
        for ( ; row >= 0; row--)
          delete [] z_buffer[row];
        delete [] z_buffer;

        // Release edge list memory 
        delete [] edge_list;

        return FALSE;
      }
    }
  }
  else
  {
    delete [] edge_list;        // Release edge list memory
    return FALSE;
  }

  // Initialize depth buffer
  for (row = 0; row < height; row++)
    for (col = 0; col < width; col++)
      z_buffer[row][col] = PR_Infinity;

  return TRUE;
}

void PolyRender::Reset()        // Reset polygon renderer
{
  int row, col;         // Loop indices
  ColorRGB rgb;         // Temporary color

  // Define background color
  rgb.SetRed((BYTE) 0);
  rgb.SetGreen((BYTE) 0);
  rgb.SetBlue((BYTE) 0);

  for (row = 0; row < height; row++)
    for (col = 0; col < width; col++)
    {
      // Clear bitmap pixel
      pbmap->SetPixel(col, row, rgb);

      // Reinitialize depth buffer element
      z_buffer[row][col] = PR_Infinity;
    }
}

void PolyRender::Close()        // Close polygon shader
{
  int row;      // Loop index
  
  delete [] edge_list;          // Release edge list memory

  // Delete depth buffer
  for (row = 0; row < height; row++)
    delete [] z_buffer[row];
  delete [] z_buffer;
}

// Render polygon
void PolyRender::Render( OutPolygon &out, int rate, double
    x_offset, double y_offset )
{
  ss_rate = rate;       // Initialize supersampling rate

  // Get vertex information
  GetVertexInfo(out, x_offset, y_offset);

  ScanEdges();          // Scan convert edges
  DrawEdgeList();       // Draw edge list
}

// Get vertex information
void PolyRender::GetVertexInfo( OutPolygon &out, double
    x_offset, double y_offset )
{
  int i;                // Loop index
  double emax;          // Maximum color band value
  double u;             // u-axis coordinate
  double v;             // v-axis coordinate
  VertexInfo *pv;       // Vertex info element pointer
  Point3 posn;          // Normalized vertex position

  // Initialize polygon y-axis limits
  ymax = 0;
  ymin = height - 1;
  ss_ymax = 0;
  ss_ymin = ymin * ss_rate;

  // Get number of vertices
  num_vert = out.GetNumVert();

  for (i = 0; i < num_vert; i++)
  {
    pv = &(v_info[i]);  // Get vertex info element pointer

    // Get vertex normalized view space co-ordinates
    posn = out.GetVertexPosn(i);

    // Scale and limit view space u-axis co-ordinate
    u = posn.GetX() * (width + x_offset);
    if (u > width)
      u = width;
    else if (u < 0.0)
      u = 0.0;
    pv->posn.SetX(u);

    // Scale and limit view space v-axis co-ordinate
    v = posn.GetY() * (height + y_offset);
    if (v > height)
      v = height;
    else if (v < 0.0)
      v = 0.0;
    pv->posn.SetY(v);

    // Get view space n-axis co-ordinate
    pv->posn.SetZ(posn.GetZ());

    // Convert to supersampling screen space co-ordinates
    //
    // NOTE: top scan line and rightmost pixels are never
    //       drawn, so there is no need to limit maximum
    //       screen co-ordinates to (width - 1) and
    //       (height - 1)
    //
    pv->ss_screen.x = (int) (pv->posn.GetX() * ss_rate);
    pv->ss_screen.y = (int) (pv->posn.GetY() * ss_rate);

    // Convert to screen space co-ordinates (round up)
    pv->screen.x = (pv->ss_screen.x + ss_rate - 1) / ss_rate;
    pv->screen.y = (pv->ss_screen.y + ss_rate - 1) / ss_rate;

    // Update polygon y-axis limits
    if (pv->screen.y < ymin)
      ymin = pv->screen.y;
    if (pv->ss_screen.y < ss_ymin)
      ss_ymin = pv->ss_screen.y;

    if (pv->screen.y > ymax)
      ymax = pv->screen.y;
    if (pv->ss_screen.y > ss_ymax)
      ss_ymax = pv->ss_screen.y;

    // Get vertex color
    pv->color = out.GetVertexColor(i);

    // Scale vertex color
    pv->color.Scale(color_scale);

    if (contrast_flag == TRUE)
    {
      // Perform contrast adjustment
      pv->color.Scale(contrast);
    }

    if (intensity_flag == TRUE)
    {
      // Perform intensity adjustment
      pv->color.Add(intensity_adj);

      // Ensure color band values are positive
      pv->color.Limit();
    }

    // Clip vertex color to unity
    if ((emax = pv->color.GetMaxColor()) > 1.0)
      pv->color.Scale(1.0 / emax);
  }
}

void PolyRender::ScanEdges()    // Scan-convert edges
{
  int i;                // Loop index
  VertexInfo *psv;      // Start vertex info pointer
  VertexInfo *pev;      // End vertex info pointer
  VertexInfo *psw;      // Swap vertex info pointer

  // Initialize edge list
  for (i = ymin; i < ymax; i++)
    edge_list[i].first = FALSE;

  for (i = 0; i < num_vert; i++)
  {
    // Get edge vertex pointers
    psv = &(v_info[i]);
    pev = &(v_info[(i + 1) % num_vert]);
    
    if (psv->screen.y > pev->screen.y)
    {
      // Swap edge vertex pointers
      psw = psv; psv = pev; pev = psw;
    }

    EdgeSetup(psv, pev);    // Initialize edge-scan DDA
    EdgeScan(psv, pev);     // Scan-convert edge
  }
}

void PolyRender::DrawEdgeList()         // Draw edge list
{
  int y;                // Loop indices
  EdgeInfo *pedge;      // Edge info pointer
  ScanInfo *pss;        // Scan line start info pointer
  ScanInfo *pes;        // Scan line end info pointer
  ScanInfo *psw;        // Swap scan line info pointer

  pedge = &(edge_list[ymin]);
  for (y = ymin; y < ymax; y++)
  {
    // Get scan line info pointers
    pss = &(pedge->isect[0]);
    pes = &(pedge->isect[1]);

    if (pss->x > pes->x)
    {
      // Swap scan line info pointers
      psw = pss; pss = pes; pes = psw;
    }

    // Ignore zero-length segments
    if ((pss->x  / ss_rate) < (pes->x / ss_rate))
    {
      SpanSetup(pss, pes);      // Initialize span-scan DDA
      SpanScan(pss, pes, y);    // Scan-convert span
    }
    pedge++;    // Point to next edge list element
  }
}

// Set up for edge scan
void PolyRender::EdgeSetup( VertexInfo *psv, VertexInfo
    *pev )
{
  int dy;           // Edge y-axis distance
  int ex, sx;       // Vertex x-axis co-ordinates
  int ey, sy;       // Vertex y-axis co-ordinates
  long ez, sz;      // Vertex z-axis co-ordinates
  DDA_Info sf;
  DDA_Info de;      // Edge distances
  IntRGB ec;        // End vertex color
  IntRGB sc;        // Start vertex color
  IntRGB temp;      // Temporary variable

  // Check for vertical edge
  if (psv->screen.y == pev->screen.y)
    return;

  // Get edge y-axis co-ordinates
  sy = psv->ss_screen.y;
  ey = pev->ss_screen.y;

  // Get edge x-axis co-ordinates
  sx = psv->ss_screen.x;
  ex = pev->ss_screen.x;

  // Get edge z-axis co-ordinates
  sz = ScaleZ(psv->posn.GetZ());
  ez = ScaleZ(pev->posn.GetZ());

  // Get vertex colors
  sc.SetColor(psv->color);
  ec.SetColor(pev->color);

  dy = ey - sy;

  // Calculate edge distances
  de.x = ex - sx;
  de.z = ez - sz;
  de.color = ec - sc;

  // Initialize edge values
  pi.x = sx;
  pi.z = sz;
  pi.color = sc;

  // Calculate delta values
  si.x = IntFloorDiv(de.x, dy);
  si.z = LongFloorDiv(de.z, (long) dy);
  si.color.SetRed(IntFloorDiv(de.color.GetRed(), dy));
  si.color.SetGreen(IntFloorDiv(de.color.GetGreen(), dy));
  si.color.SetBlue(IntFloorDiv(de.color.GetBlue(), dy));

  sf.x = de.x - si.x * dy;
  sf.z = de.z - si.z * (long) dy;
  temp = si.color * dy;
  sf.color = de.color - temp;

  r.x = sf.x * 2 - dy;
  r.z = sf.z * 2L - (long) dy;
  temp = sf.color * 2;
  r.color = temp - dy;

  // Calculate increment values
  inc.x = sf.x;
  inc.z = sf.z;
  inc.color = sf.color;

  // Calculate decrement values
  dec.x = sf.x - dy;
  dec.z = sf.z - (long) dy;
  dec.color = sf.color - dy;
}

// Scan-convert edge
void PolyRender::EdgeScan( VertexInfo *psv, VertexInfo
    *pev )
{
  int sy;               // Edge index
  EdgeInfo *pedge;      // Edge info pointer
  ScanInfo *pscan;      // Scan line info pointer
  
  // Initialize edge info pointer
  pedge = &(edge_list[psv->screen.y]);

  // Scan-convert edge
  for (sy = psv->ss_screen.y; sy < pev->ss_screen.y; sy++)
  {
    if ((sy % ss_rate) == 0)    // Edge info element ?
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
      pscan->x = pi.x;
      pscan->z = pi.z;
      pscan->color = pi.color;

      pedge++;  // Point to next edge list element
    }

    if (r.x >= 0)       // Update x-axis parameters
    {
      pi.x += si.x + 1;
      r.x += dec.x;
    }
    else
    {
      pi.x += si.x;
      r.x += inc.x;
    }

    if (r.z >= 0L)      // Update z-axis parameters
    {
      pi.z += si.z + 1L;
      r.z += dec.z;
    }
    else
    {
      pi.z += si.z;
      r.z += inc.z;
    }

    // Update red parameters
    if (r.color.GetRed() >= 0)
    {
      pi.color.AddRed(si.color.GetRed() + 1);
      r.color.AddRed(dec.color.GetRed());
    }
    else
    {
      pi.color.AddRed(si.color.GetRed());
      r.color.AddRed(inc.color.GetRed());
    }

    // Update green parameters
    if (r.color.GetGreen() >= 0)
    {
      pi.color.AddGreen(si.color.GetGreen() + 1);
      r.color.AddGreen(dec.color.GetGreen());
    }
    else
    {
      pi.color.AddGreen(si.color.GetGreen());
      r.color.AddGreen(inc.color.GetGreen());
    }

    // Update blue parameters
    if (r.color.GetBlue() >= 0)
    {
      pi.color.AddBlue(si.color.GetBlue() + 1);
      r.color.AddBlue(dec.color.GetBlue());
    }
    else
    {
      pi.color.AddBlue(si.color.GetBlue());
      r.color.AddBlue(inc.color.GetBlue());
    }
  }
}

// Set up for span scan (assumes non-zero span length)
void PolyRender::SpanSetup( ScanInfo *pss, ScanInfo *pes )
{
  int dx;       // x-axis distance
  DDA_Info sf;
  DDA_Info de;  // Edge distances
  IntRGB temp;  // Temporary variable

  dx = pes->x - pss->x;

  // Calculate span distances
  de.z = pes->z - pss->z;
  de.color = pes->color - pss->color;

  // Initialize edge values
  pi.z = pss->z;
  pi.color = pss->color;

  // Calculate delta values
  si.z = LongFloorDiv(de.z, (long) dx);
  si.color.SetRed(IntFloorDiv(de.color.GetRed(), dx));
  si.color.SetGreen(IntFloorDiv(de.color.GetGreen(), dx));
  si.color.SetBlue(IntFloorDiv(de.color.GetBlue(), dx));

  sf.z = de.z - si.z * (long) dx;
  temp = si.color * dx;
  sf.color = de.color - temp;

  r.z = sf.z * 2L - (long) dx;
  temp = sf.color * 2;
  r.color = temp - dx;

  // Calculate increment values
  inc.z = sf.z;
  inc.color = sf.color;

  // Calculate decrement values
  dec.z = sf.z - (long) dx;
  dec.color = sf.color - dx;
}

// Scan-convert span
void PolyRender::SpanScan( ScanInfo *pss, ScanInfo *pes,
    int y )
{
  int ss_x;         // Supersampling pixel index
  int first_x;      // First pixel index
  long *pzb;        // Z-buffer entry pointer
  ColorRGB rgb;     // RGB color
  
  // Calculate first pixel index
  first_x = (pss->x + ss_rate - 1) / ss_rate;

  // Initialize Z-buffer entry pointer
  pzb = &(z_buffer[y][first_x]);

  // Initialize bitmap pixel pointer
  pbmap->SetPixelPtr(first_x, y);

  // Scan-convert span
  for (ss_x = pss->x; ss_x < pes->x; ss_x++)
  {
    if ((ss_x % ss_rate) == 0)  // Bitmap pixel ?
    {
      if (pi.z < *pzb)  // Check pixel visibility
      {
        *pzb = pi.z;            // Update Z-buffer entry

        pi.color.SetColorRGB(&rgb); // Get current RGB color 
        pbmap->SetCurrPixel(rgb);   // Set bitmap pixel
      }

      pzb++;    // Increment Z-buffer entry pointer

      pbmap->IncPixelPtr();     // Increment pixel pointer
    }

    if (r.z >= 0L)          // Update z-axis parameters
    {
      pi.z += si.z + 1L;
      r.z += dec.z;
    }
    else
    {
      pi.z += si.z;
      r.z += inc.z;
    }

    // Update red parameters
    if (r.color.GetRed() >= 0)
    {
      pi.color.AddRed(si.color.GetRed() + 1);
      r.color.AddRed(dec.color.GetRed());
    }
    else
    {
      pi.color.AddRed(si.color.GetRed());
      r.color.AddRed(inc.color.GetRed());
    }

    // Update green parameters
    if (r.color.GetGreen() >= 0)
    {
      pi.color.AddGreen(si.color.GetGreen() + 1);
      r.color.AddGreen(dec.color.GetGreen());
    }
    else
    {
      pi.color.AddGreen(si.color.GetGreen());
      r.color.AddGreen(inc.color.GetGreen());
    }

    // Update blue parameters
    if (r.color.GetBlue() >= 0)
    {
      pi.color.AddBlue(si.color.GetBlue() + 1);
      r.color.AddBlue(dec.color.GetBlue());
    }
    else
    {
      pi.color.AddBlue(si.color.GetBlue());
      r.color.AddBlue(inc.color.GetBlue());
    }
  }
}

