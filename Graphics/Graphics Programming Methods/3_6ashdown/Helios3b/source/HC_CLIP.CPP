////////////////////////////////////////////////////////////
//
//  HC_CLIP.CPP - Hemicube Polygon Clipper Class
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

#include "hc_clip.h"

HemiClip::HemiClip()    // HemiClip class constructor
{
  Vector4 temp;     // Temporary vector

  // Link edge-plane clippers
  pclip = &(clipper[HC_Front]);
  clipper[HC_Front].Add(&(clipper[HC_Left]));
  clipper[HC_Left].Add(&(clipper[HC_Right]));
  clipper[HC_Right].Add(&(clipper[HC_Top]));
  clipper[HC_Top].Add(&(clipper[HC_Bottom]));
  clipper[HC_Bottom].Add(NULL);

  // Set clipper plane normals

  temp = Vector4(0.0, 0.0, 1.0, 0.0);
  clipper[HC_Front].SetNormal(temp.Norm());

  temp = Vector4(1.0, 0.0, 0.0, 0.0);
  clipper[HC_Left].SetNormal(temp.Norm());

  temp = Vector4(-1.0, 0.0, 0.0, 1.0);
  clipper[HC_Right].SetNormal(temp.Norm());

  temp = Vector4(0.0, -1.0, 0.0, 1.0);
  clipper[HC_Top].SetNormal(temp.Norm());

  temp = Vector4(0.0, 1.0, 0.0, 0.0);
  clipper[HC_Bottom].SetNormal(temp.Norm());
}

// Choose random hemicube orientation
void HemiClip::SetView( Patch3 *ppatch )
{
  Vector3 rv;   // Random vector

  // Get eye position (hemicube center)
  center = ppatch->GetCenter();

  // Select random vector for hemicube orientation
  rv = RandomVector();

  n = ppatch->GetNormal();      // Get patch normal

  do   // Get valid u-axis vector
  {
    u = Cross(n, rv);
  }
  while (u.Length() < MIN_VALUE);

  u.Norm();             // Normalize u-axis
  v = Cross(u, n);      // Determine v-axis
}

void HemiClip::BuildTransform( Vector3 &nu, Vector3 &nv,
    Vector3 &nn)
{
  Vector3 origin;       // View space origin

  origin = Vector3(center);

  // Set view transformation matrix
  vtm[0][0] = nu.GetX();
  vtm[0][1] = nu.GetY();
  vtm[0][2] = nu.GetZ();
  vtm[0][3] = -(Dot(origin, nu));

  vtm[1][0] = nv.GetX();
  vtm[1][1] = nv.GetY();
  vtm[1][2] = nv.GetZ();
  vtm[1][3] = -(Dot(origin, nv));

  vtm[2][0] = nn.GetX();
  vtm[2][1] = nn.GetY();
  vtm[2][2] = nn.GetZ();
  vtm[2][3] = -(Dot(origin, nn));

  vtm[3][0] = 0.0;
  vtm[3][1] = 0.0;
  vtm[3][2] = 0.0;
  vtm[3][3] = 1.0;

  // Premultiply by translation matrix
  vtm[2][3] -= 1.0;

  // Premultiply by perspective transformation matrix
  vtm[3][0] += vtm[2][0];
  vtm[3][1] += vtm[2][1];
  vtm[3][2] += vtm[2][2];
  vtm[3][3] += vtm[2][3];

  // Premultiply by normalization matrix

  vtm[0][0] = 0.5 * (vtm[0][0] + vtm[3][0]);
  vtm[0][1] = 0.5 * (vtm[0][1] + vtm[3][1]);
  vtm[0][2] = 0.5 * (vtm[0][2] + vtm[3][2]);
  vtm[0][3] = 0.5 * (vtm[0][3] + vtm[3][3]);

  vtm[1][0] = 0.5 * (vtm[1][0] + vtm[3][0]);
  vtm[1][1] = 0.5 * (vtm[1][1] + vtm[3][1]);
  vtm[1][2] = 0.5 * (vtm[1][2] + vtm[3][2]);
  vtm[1][3] = 0.5 * (vtm[1][3] + vtm[3][3]);

  vtm[2][0] = SN * vtm[2][0] + RN * vtm[3][0];
  vtm[2][1] = SN * vtm[2][1] + RN * vtm[3][1];
  vtm[2][2] = SN * vtm[2][2] + RN * vtm[3][2];
  vtm[2][3] = SN * vtm[2][3] + RN * vtm[3][3];
}

// Update hemicube view transformation matrix
void HemiClip::UpdateView( int face_id )
{
  Vector3 nu, nv, nn;   // View space co-ordinates

  switch (face_id )     // Exchange co-ordinates
  {
    case HC_TopFace:
      nu = u; nv = v; nn = n;
      break;
    case HC_FrontFace:
      nu = -u; nv = n; nn = v;
      break;
    case HC_RightFace:
      nu = v; nv = n; nn = u;
      break;
    case HC_BackFace:
      nu = u; nv = n; nn = -v;
      break;
    case HC_LeftFace:
      nu = -v; nv = n; nn = -u;
      break;
    default:
      break;
  }
  
  // Build new view transformation matrix
  BuildTransform(nu, nv, nn);
}
