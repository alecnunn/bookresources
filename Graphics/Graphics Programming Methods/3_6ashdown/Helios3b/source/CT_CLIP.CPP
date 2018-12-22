////////////////////////////////////////////////////////////
//
//  CT_CLIP.CPP - Cubic Tetrahedron Polygon Clipper Class
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/07/21 - Version 1.02B release.
//              95/09/04 - Fixed random vector selection
//                         error in SetView.
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

#include "ct_clip.h"

CubicClip::CubicClip()  // CubicClip class constructor
{
  Vector4 temp;     // Temporary vector

  // Link edge-plane clippers
  pclip = &(clipper[CT_Front]);
  clipper[CT_Front].Add(&(clipper[CT_Left]));
  clipper[CT_Left].Add(&(clipper[CT_Bottom]));
  clipper[CT_Bottom].Add(&(clipper[CT_Diag]));
  clipper[CT_Diag].Add(NULL);

  // Set clipper plane normals

  temp = Vector4(0.0, 0.0, 1.0, 0.0);
  clipper[CT_Front].SetNormal(temp.Norm());

  temp = Vector4(1.0, 0.0, 0.0, 0.0);
  clipper[CT_Left].SetNormal(temp.Norm());

  temp = Vector4(0.0, 1.0, 0.0, 0.0);
  clipper[CT_Bottom].SetNormal(temp.Norm());

  temp = Vector4(-1.0, -1.0, 0.0, 1.0);
  clipper[CT_Diag].SetNormal(temp.Norm());
}

// Choose random cubic tetrahedron orientation
void CubicClip::SetView( Patch3 *ppatch )
{
  double a, b, c;   // Temporary variables
  Vector3 rv;       // Random vector
  Vector3 patch_u;  // Patch view space u-axis vector
  Vector3 patch_v;  // Patch view space v-axis vector
  Vector3 patch_n;  // Patch view space n-axis vector

  // Get eye position (cubic tetrahedron center)
  center = ppatch->GetCenter();

  patch_n = ppatch->GetNormal();    // Get patch normal

  do   // Get valid u-axis vector
  {
    // Select random vector for cubic tetrahedron
    // orientation
    rv = RandomVector();

    patch_u = Cross(patch_n, rv);
  }
  while (patch_u.Length() < MIN_VALUE);

  patch_u.Norm();                       // Normalize u-axis
  patch_v = Cross(patch_u, patch_n);    // Determine v-axis

  // Rotate cubic tetrahedron view space co-ordinate system
  // to align it with respect to patch view space such
  // that:
  //
  //   u = a * patch_u + b * patch_v - c * patch_n
  //   v = b * patch_u + a * patch_v - c * patch_n
  //   n = c * patch_u + c * patch_v - c * patch_n
  //
  // where:
  //
  //   a = 1 / (2 * sqrt(3)) + 1 / 2
  //   b = 1 / (2 * sqrt(3)) - 1 / 2
  //   c = -1 / sqrt(3)

  c = -1.0 / sqrt(3.0);
  a = (c * -0.5) + 0.5;
  b = (c * -0.5) - 0.5;

  u = a * patch_u + b * patch_v - c * patch_n;
  v = b * patch_u + a * patch_v - c * patch_n;
  n = c * patch_u + c * patch_v - c * patch_n;
}

void CubicClip::BuildTransform( Vector3 &nu, Vector3 &nv,
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

  vtm[0][0] = (vtm[0][0] + vtm[3][0]) / 3.0;
  vtm[0][1] = (vtm[0][1] + vtm[3][1]) / 3.0;
  vtm[0][2] = (vtm[0][2] + vtm[3][2]) / 3.0;
  vtm[0][3] = (vtm[0][3] + vtm[3][3]) / 3.0;

  vtm[1][0] = (vtm[1][0] + vtm[3][0]) / 3.0;
  vtm[1][1] = (vtm[1][1] + vtm[3][1]) / 3.0;
  vtm[1][2] = (vtm[1][2] + vtm[3][2]) / 3.0;
  vtm[1][3] = (vtm[1][3] + vtm[3][3]) / 3.0;

  vtm[2][0] = SN * vtm[2][0] + RN * vtm[3][0];
  vtm[2][1] = SN * vtm[2][1] + RN * vtm[3][1];
  vtm[2][2] = SN * vtm[2][2] + RN * vtm[3][2];
  vtm[2][3] = SN * vtm[2][3] + RN * vtm[3][3];
}

// Update cubic tetrahedron view transformation matrix
void CubicClip::UpdateView( int face_id )
{
  Vector3 nu, nv, nn;   // View space co-ordinates

  switch (face_id )     // Exchange co-ordinates
  {
    case CT_TopFace:
      nu = -u; nv = -v; nn = n;
      break;
    case CT_RightFace:
      nu = -v; nv = -n; nn = u;
      break;
    case CT_LeftFace:
      nu = -u; nv = -n; nn = v;
      break;
    default:
      break;
  }
  
  // Build new view transformation matrix
  BuildTransform(nu, nv, nn);
}

