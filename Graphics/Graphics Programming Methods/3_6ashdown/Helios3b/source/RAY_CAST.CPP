////////////////////////////////////////////////////////////
//
//  RAY_CAST.CPP - Ray Cast Form Factor Class
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/07/21 - Version 1.02B release.
//              96/02/14 - Version 1.02C release.
//              96/04/01 - Version 1.03A release.
//              96/11/16 - Fixed tests for patch normal
//                         dominant axis in TestPatch.
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

#include "ray_cast.h"

double RayCast::CalcFormFactor( Vertex3 *pvertex, Instance
    *penv )
{
  int i;            // Loop index
  double ff;        // Vertex-source form factor
  double ray_len;   // Ray length
  Vector3 nv;       // Vertex normal
  Vector3 n_ray;    // Normalized ray direction
  Vector3 r_ray;    // Reverse normalized ray direction
  Vector3 view;     // Source patch view vector

  start = Vector3(pvertex->GetPosn());
  nv = pvertex->GetNormal();
  view = start - src_center;

  // Determine whether source patch is backface
  if (Dot(src_norm, view) < MIN_VALUE)
    return 0.0;

  ff = 0.0;
  for (i = 0; i < RC_NumRays; i++)
  {
    // Select random point on source patch
    Select(&end);

    // Generate ray to shoot from vertex to source
    ray_dir = end - start;

    // Check for source point behind vertex
    if (Dot(nv, ray_dir) < MIN_VALUE)
      continue;

    // Test for ray-element intersection
    if (CheckOcclusion(penv) == FALSE)
    {
      // Calculate ray length
      ray_len = ray_dir.Length();

      // Calculate normalized ray direction
      n_ray = ray_dir;
      n_ray.Norm();

      // Determine reverse normalized ray direction
      r_ray = -n_ray;

      // Update form factor estimation
      ff += Dot(n_ray, nv) * Dot(r_ray, src_norm) / ((PI *
          ray_len * ray_len) + ray_area);
    }
  }

  // Multiply by ray-source patch intersection area
  ff *= ray_area;

  return ff;
}

// Initialize parameters for source patch
void RayCast::Init( Patch3 *ppatch )
{
  double a1, a2;        // Triangle areas
  Vector3 temp;         // Temporary 3-D vector
  Vector3 e0, e1, e2;   // Edge vectors

  psrc = ppatch;
  pcache = NULL;
  src_area = psrc->GetArea();
  src_norm = psrc->GetNormal();
  src_center = Vector3(psrc->GetCenter());
  ray_area = src_area / RC_NumRays;

  // Get patch vertex vectors
  v0 = Vector3(ppatch->GetVertexPtr(0)->GetPosn());
  v1 = Vector3(ppatch->GetVertexPtr(1)->GetPosn());
  v2 = Vector3(ppatch->GetVertexPtr(2)->GetPosn());
  v3 = Vector3(ppatch->GetVertexPtr(3)->GetPosn());

  // Calculate patch edge vectors
  e0 = Vector3(v1 - v0);
  e1 = Vector3(v2 - v0);

  // Calculate first triangle area
  temp = Cross(e0, e1);
  a1 = temp.Length() / 2.0;

  if (ppatch->IsQuad() == TRUE)
  {
    // Calculate patch edge vector
    e2 = Vector3(v3 - v0);

    // Calculate second triangle area
    temp = Cross(e1, e2);
    a2 = temp.Length() / 2.0;
  }
  else
    a2 = 0.0;

  // Calculate fractional area of first triangle
  selector = a1 / (a1 + a2);
}

// Select random point within source patch area
void RayCast::Select( Vector3 *ppoint )
{
  double s, t;      // Random point parameters

  // Get random point parameters
  s = GetNormRand();
  t = GetNormRand();

  // Ensure random point is inside triangle
  if (s + t > 1.0)
  {
    s = 1.0 - s;
    t = 1.0 - t;
  }

  // Calculate random point co-ordinates
  if (GetNormRand() <= selector)
  {
    // Locate point in first triangle
    *ppoint = (1.0 - s - t) * v0 + s * v1 + t * v2;
  }
  else
  {
    // Locate point in second triangle
    *ppoint = (1.0 - s - t) * v0 + s * v2 + t * v3;
  }
}

// Check for ray occlusion
BOOL RayCast::CheckOcclusion( Instance *pinst )
{
  Patch3 *ppatch;       // Patch pointer
  Surface3 *psurf;      // Surface pointer

  // Test cached patch for ray-patch intersection
  if (TestPatch(pcache) == TRUE)
    return TRUE;

  // Walk the instance list
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
        if (ppatch != psrc)     // Ignore source patch
        {
          // Test for ray-patch intersection
          if (TestPatch(ppatch) == TRUE)
          {
            // Cache occluding patch
            pcache = ppatch;

            return TRUE;
          }
        }
        ppatch = ppatch->GetNext();
      }
      psurf = psurf->GetNext();
    }
    pinst = pinst->GetNext();
  }

  return FALSE;
}

// Check for ray-patch intersection (Badouel's Algorithm)
BOOL RayCast::TestPatch( Patch3 *ppatch )
{
  BOOL i_flag;          // Intersection flag
  int i;                // Loop index
  int i0, i1, i2;       // Projection plane axis indices
  double alpha;         // Scaling parameter
  double beta;          // Scaling parameter
  double dist;          // Patch plane distance
  double d, t;          // Temporary variables
  double isect[3];      // Ray-patch intersection
  double n_mag[3];      // Patch normal axis magnitudes
  double vert[4][3];    // Patch vertices
  double s0, s1, s2;    // Projected vector co-ordinates
  double t0, t1, t2;    // Projected vector co-ordinates
  Point3 *pvp;          // Vertex position pointer
  Vector3 normal;       // Patch normal
  Vector3 temp;         // Temporary 3-D vector

  // Check for valid patch
  if (ppatch == NULL)
    return FALSE;

  // Get patch normal
  normal = ppatch->GetNormal();

  // Calculate divisor
  d = Dot(normal, ray_dir);

  // Determine whether ray is parallel to patch
  if (fabs(d) < MIN_VALUE)
    return FALSE;

  // Calculate patch plane distance
  temp = Vector3(ppatch->GetVertexPtr(0)->GetPosn());
  dist = Dot(normal, temp);
      
  // Calculate ray hit time parameter
  t = (dist - Dot(normal, start)) / d;

  // Check whether patch plane is behind receiver vertex or
  // source patch point
  //
  // NOTE: MIN_VALUE offsets are required to prevent
  //       interpretation of adjoining surface vertices as
  //       occlusions
  if (t < MIN_VALUE || t > (1.0 - MIN_VALUE))
    return FALSE;

  // Calculate ray-patch plane intersection
  temp = start + (ray_dir * t);

  // Get intersection axes
  isect[0] = temp.GetX();
  isect[1] = temp.GetY();
  isect[2] = temp.GetZ();

  // Get patch normal axis magnitudes
  n_mag[0] = fabs(normal.GetX());
  n_mag[1] = fabs(normal.GetY());
  n_mag[2] = fabs(normal.GetZ());

  // Get patch vertex axes
  for (i = 0; i < ppatch->GetNumVert(); i++)
  {
    pvp = ppatch->GetVertexPtr(i)->GetPosnPtr();
    vert[i][0] = pvp->GetX();
    vert[i][1] = pvp->GetY();
    vert[i][2] = pvp->GetZ();
  }

  // Find patch normal dominant axis
  if ((n_mag[0] >= n_mag[1]) && (n_mag[0] >= n_mag[2]))
  {
    i0 = 0; i1 = 1; i2 = 2;     // X-axis dominant
  }
  else if ((n_mag[1] >= n_mag[0]) && (n_mag[1] >= n_mag[2]))
  {
    i0 = 1; i1 = 0; i2 = 2;     // Y-axis dominant
  }
  else
  {
    i0 = 2; i1 = 0; i2 = 1;     // Z-axis dominant
  }

  // Calculate projected vertex #0 co-ordinates
  s0 = isect[i1] - vert[0][i1];
  t0 = isect[i2] - vert[0][i2];

  // Check for intersection (consider quadrilateral as two
  // adjacent triangles
  i = 2;
  i_flag = FALSE;
  do
  {
    // Calculate projected vertex co-ordinates
    s1 = vert[i - 1][i1] - vert[0][i1];
    t1 = vert[i - 1][i2] - vert[0][i2];

    s2 = vert[i][i1] - vert[0][i1];
    t2 = vert[i][i2] - vert[0][i2];

    // Determine vector scaling parameters
    if (fabs(s1) < MIN_VALUE)   // Is s1 == 0 ?
    {
      beta = s0 / s2;
      if ((beta >= 0.0) && (beta <= 1.0))
      {
        alpha = (t0 - beta * t2) / t1;
        i_flag = ((alpha >= 0.0) && ((alpha + beta) <=
            1.0));
      }
    }
    else
    {
      beta = (s1 * t0 - s0 * t1) / (s1 * t2 - s2 * t1);
      if ((beta >= 0.0) && (beta <= 1.0))
      {
        alpha = (s0 - beta * s2) / s1;

        // Test for intersection
        i_flag = ((alpha >= 0.0) && ((alpha + beta) <=
            1.0));
      }
    }
    i++;    // Advance to next triangle (if any)
  }
  while (i_flag == FALSE && i < ppatch->GetNumVert());

  return i_flag;
}

