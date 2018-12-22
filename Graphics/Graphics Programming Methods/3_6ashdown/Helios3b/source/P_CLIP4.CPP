////////////////////////////////////////////////////////////
//
//  P_CLIP4.CPP - 4-D Polygon Clipper Class
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

#include "p_clip4.h"

PolyClip4::PolyClip4()  // PolyClip4 class constructor
{
  Vector4 temp;     // Temporary vector

  // Link edge-plane clippers
  pclip = &(clipper[Front]);
  clipper[Front].Add(&(clipper[Back]));
  clipper[Back].Add(&(clipper[Left]));
  clipper[Left].Add(&(clipper[Right]));
  clipper[Right].Add(&(clipper[Top]));
  clipper[Top].Add(&(clipper[Bottom]));
  clipper[Bottom].Add(NULL);

  // Set clipper plane normals

  temp = Vector4(0.0, 0.0, 1.0, 0.0);
  clipper[Front].SetNormal(temp.Norm());

  temp = Vector4(0.0, 0.0, -1.0, 1.0);
  clipper[Back].SetNormal(temp.Norm());

  temp = Vector4(1.0, 0.0, 0.0, 0.0);
  clipper[Left].SetNormal(temp.Norm());

  temp = Vector4(-1.0, 0.0, 0.0, 1.0);
  clipper[Right].SetNormal(temp.Norm());

  temp = Vector4(0.0, -1.0, 0.0, 1.0);
  clipper[Top].SetNormal(temp.Norm());

  temp = Vector4(0.0, 1.0, 0.0, 0.0);
  clipper[Bottom].SetNormal(temp.Norm());
}

// Clip polygon
int PolyClip4::Clip( Element3 *pelem, OutPolygon &out,
    double (*ptm)[4] )
{
  int i;            // Loop index
  int num_vert;     // Number of vertices
  Vertex3 *pvert;   // 3-D world space vertex pointer
  Vertex4 hv;       // 4-D homogeneous co-ord vertex

  out.Reset();  // Reset output polygon

  num_vert = pelem->GetNumVert();
  for (i = 0; i < num_vert; i++)
  {
    // Get world space vertex position pointer
    pvert = pelem->GetVertexPtr(i);

    // Set homogeneous co-ordinates vertex
    hv.Set(pvert->GetPosn(), pvert->GetExitance(), ptm);

    pclip->Clip(hv, out);       // Clip polygon edge
  }

  pclip->Close(out);    // Close polygon
      
  return out.GetNumVert();
}

// Output view space vertex
void ClipEdge::Output( Vertex4 &v, OutPolygon &out )
{
  if (pnext != NULL)    // More planes ?
    pnext->Clip(v, out);
  else
    out.AddVertex(v);
}

// Calculate intersection vertex
Vertex4 ClipEdge::Intersect( Vertex4 &s, Vertex4 &e )
{
  double d, t;      // Temporary variables
  Spectra color;    // Temporary color
  Vector4 p, r;     // Temporary vectors
  Vertex4 v;        // Temporary vertex

  // Calculate parameter
  r = (e.GetCoord() - s.GetCoord());
  d = Dot(normal, r);

  if (fabs(d) > MIN_VALUE)
    t = -Dot(normal, s.GetCoord()) / d;
  else
    t = 1.0;

  if (t < 0.0)  // Ensure lower limit
    t = 0.0;

  if (t > 1.0)  // Ensure upper limit
    t = 1.0;

  // Calculate intersection vertex co-ordinates
  r *= t;
  p = s.GetCoord() + r;
      
  // Linearly interpolate vertex color
  color = Blend(s.GetColor(), e.GetColor(), t);

  v.Set(p, color);

  return v;
}

// Clip polygon edge
void ClipEdge::Clip( Vertex4 &current, OutPolygon &out )
{
  BOOL curr_inside;     // Current point inside flag
  Vertex4 isect;        // Intersection vertex

  // Determine vertex visibility
  curr_inside = IsInside(current);

  if (first_flag == FALSE)      // First vertex seen ?
  {
    first = current;
    first_inside = curr_inside;
    first_flag = TRUE;
  }
  else
  {
    // Does edge intersect plane ?
    if (start_inside ^ curr_inside)
    {
      isect = Intersect(start, current);
      Output(isect, out);
    }
  }

  if (curr_inside == TRUE)
    Output(current, out);

  start = current;
  start_inside = curr_inside;
}

// Close polygon
void ClipEdge::Close( OutPolygon &out )
{
  Vertex4 isect;        // Intersection vertex

  if (first_flag == TRUE)
  {
    // Does edge intersect plane ?
    if (start_inside ^ first_inside)
    {
      isect = Intersect(start, first);
      Output(isect, out);
    }

    if (pnext != NULL)  // More planes ?
      pnext->Close(out);

    // Reset first vertex seen flag
    first_flag = FALSE;
  }
}

