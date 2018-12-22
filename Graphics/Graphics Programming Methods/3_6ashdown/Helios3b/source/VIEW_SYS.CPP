////////////////////////////////////////////////////////////
//
//  VIEW_SYS.CPP - Viewing System Class
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/12/16 - Version 1.01A release.
//              94/12/24 - Modified BuildTransform to
//                         include parallel projection and
//                         and window scale.
//                       - Modified BackFaceCull to include
//                         parallel projection.
//              95/02/05 - Version 1.02A release.
//              95/07/21 - Version 1.02B release.
//              96/02/14 - Version 1.02C release.
//              96/03/30 - Added Dolly, Orbit, Pan, Rotate,
//                         Tilt, Zoom, SetEyeFocusPosn and
//                         InitViewSystem functions.
//                       - Modified BuildTransform to
//                         calculate view-up vector.
//                       - Modified SetViewUp to calculate
//                         canonical view-up vector.
//              96/04/01 - Version 1.03A release.
//              96/11/17 - Removed unused local variable
//                         and deleted par_flag
//                         initialization from
//                         InitViewSystem.
//                       - Modified BuildTransform to
//                         properly initialize projective
//                         transformation matrix for
//                         parallel projection.
//              97/03/13 - Added /Olge optimization pragma
//                         for InitViewsystem function
//                         (compiler bug workaround).
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

#include "view_sys.h"

// Build projective transformation matrix and eye position
void ViewSys::BuildTransform()
{
  double rn;            // Translation factor
  double su, sv, sn;    // Scaling factors
  Vector3 o;            // Origin vector
  Vector3 u;            // u-axis vector

  // Calculate view-up vector
  vuv = cos(tilt_angle) * cvu - sin(tilt_angle) *
      Cross(vdv, cvu);

  // Set view space origin
  origin.SetX(eye_posn.GetX() - eye * vdv.GetX()); 
  origin.SetY(eye_posn.GetY() - eye * vdv.GetY()); 
  origin.SetZ(eye_posn.GetZ() - eye * vdv.GetZ()); 

  o = Vector3(origin);  // Initialize origin vector
  u = Cross(vdv, vuv);  // Calculate u-axis vector

  // Initialize view transformation matrix
  ptm[0][0] = u.GetX();
  ptm[0][1] = u.GetY();
  ptm[0][2] = u.GetZ();
  ptm[0][3] = -(Dot(o, u));

  ptm[1][0] = vuv.GetX();
  ptm[1][1] = vuv.GetY();
  ptm[1][2] = vuv.GetZ();
  ptm[1][3] = -(Dot(o, vuv));

  ptm[2][0] = vdv.GetX();
  ptm[2][1] = vdv.GetY();
  ptm[2][2] = vdv.GetZ();
  ptm[2][3] = -(Dot(o, vdv));

  ptm[3][0] = 0.0;
  ptm[3][1] = 0.0;
  ptm[3][2] = 0.0;
  ptm[3][3] = 1.0;

  if (par_flag == FALSE)        // Perspective projection ?
  {
    // Premultiply by perspective transformation matrix
    ptm[3][0] -= ptm[2][0] / eye;
    ptm[3][1] -= ptm[2][1] / eye;
    ptm[3][2] -= ptm[2][2] / eye;
    ptm[3][3] -= ptm[2][3] / eye;
  }

  // Premultiply by normalization matrix
  if (aspect >= 1.0)
  {
    su = 0.5 * scale;
    sv = 0.5 * scale * aspect;
  }
  else
  {
    su = 0.5 * scale / aspect;
    sv = 0.5 * scale;
  }

  if (par_flag == FALSE)
  {
    sn = (eye - bpd) * (eye - fpd) / (eye * eye * (bpd -
        fpd));
    rn = fpd * (eye - bpd) / (eye * (fpd - bpd));
  }
  else
  {
    sn = 1.0 / (bpd - fpd);
    rn = -fpd * sn;
  }

  ptm[0][0] = su * ptm[0][0] + 0.5 * ptm[3][0];
  ptm[0][1] = su * ptm[0][1] + 0.5 * ptm[3][1];
  ptm[0][2] = su * ptm[0][2] + 0.5 * ptm[3][2];
  ptm[0][3] = su * ptm[0][3] + 0.5 * ptm[3][3];

  ptm[1][0] = sv * ptm[1][0] + 0.5 * ptm[3][0];
  ptm[1][1] = sv * ptm[1][1] + 0.5 * ptm[3][1];
  ptm[1][2] = sv * ptm[1][2] + 0.5 * ptm[3][2];
  ptm[1][3] = sv * ptm[1][3] + 0.5 * ptm[3][3];

  ptm[2][0] = sn * ptm[2][0] + rn * ptm[3][0];
  ptm[2][1] = sn * ptm[2][1] + rn * ptm[3][1];
  ptm[2][2] = sn * ptm[2][2] + rn * ptm[3][2];
  ptm[2][3] = sn * ptm[2][3] + rn * ptm[3][3];
}

// Set view-up vector
void ViewSys::SetViewUp( Vector3 &approx )
{
  Vector3 temp = vdv;   // Temporary vector

  // Project approximate view-up vector onto view plane
  temp *= Dot(approx, vdv);
  cvu = approx - temp;

  cvu.Norm();   // Normalize canonical view-up vector
}

// Perform backface culling
BOOL ViewSys::BackFaceCull( Patch3 *ppatch )
{
  Vector3 view;         // View vector

  if (par_flag == FALSE)     // Perpective projection ?
  {
    // Calculate view vector (first vertex to eye position)
    view = Vector3(ppatch->GetVertexPtr(0)->GetPosn(),
        eye_posn);
  
    // Indicate whether patch is backface
    return (Dot(ppatch->GetNormal(), view) < MIN_VALUE) ?
        TRUE : FALSE;
  }
  else  // Parallel projection
  {
    // Indicate whether patch is backface
    return (Dot(ppatch->GetNormal(), vdv) > MIN_VALUE) ?
        TRUE : FALSE;
  }
}

#pragma optimize("lge", off)

// Initialize view system parameters
void ViewSys::InitViewSystem( Environ *penv )
{
  double dx, dy, dz;    // Extent deltas
  Vector3 temp;         // Temporary vector

  // Calculate extent deltas
  dx = penv->GetMax_X() - penv->GetMin_X();
  dy = penv->GetMax_Y() - penv->GetMin_Y();
  dz = penv->GetMax_Z() - penv->GetMin_Z();

  // Determine default view
  fpd = -1.99;
  bpd = 10000.0;
  scale = 1.0;
  tilt_angle = 0.0;

  if (dy > (dz * aspect))
    eye_posn.SetX((dy / VS_FILL_FACTOR) + penv->GetMax_X());
  else
    eye_posn.SetX(((dz * aspect) / VS_FILL_FACTOR) +
        penv->GetMax_X());
  eye_posn.SetY(dy / 2.0 + penv->GetMin_Y());
  eye_posn.SetZ(dz / 2.0 + penv->GetMin_Z());

  focus_posn.SetX(dx / 2.0 + penv->GetMin_X());
  focus_posn.SetY(dy / 2.0 + penv->GetMin_Y());
  focus_posn.SetZ(dz / 2.0 + penv->GetMin_Z());

  temp = focus_posn - eye_posn;
  efd = temp.Length();

  vdv = Vector3(-1.0, 0.0, 0.0);
  cvu = Vector3(0.0, 0.0, 1.0);
  vuv = Vector3(0.0, 0.0, 1.0);
  eye = -2.0;

  // Update view system parameters
  BuildTransform();
}

#pragma optimize("lge", on)

void ViewSys::SetEyeFocusPosn( Point3 &e, Point3 &f )
{
  Vector3 hn, hv;       // Horizontal projection vectors
  Vector3 temp;         // Temporary vector
  Vector3 u, v, n;      // Viewing system coordinate axes

  n = vdv;  // Save current view director vector
  v = cvu;  // Save current canonical view-up vector

  u = Cross(n, v);      // Calculate u-axis vector

  // Update eye and focus positions
  eye_posn = e;
  focus_posn = f;

  // Update eye-focus distance
  temp = focus_posn - eye_posn;
  efd = temp.Length();

  // Update view direction vector
  vdv = temp.Norm();

  // Project view direction vectors onto horizontal plane
  hn = Vector3(n.GetX(), n.GetY(), 0.0);
  hv = Vector3(vdv.GetX(), vdv.GetY(), 0.0);

  // Update u-axis vector
  if (hv.Length() > MIN_VALUE)
  {
    // NOTE: If the camera was previously oriented facing
    //       straight up, the canonical view-up vector will
    //       be oriented upwards, regardless of its previous
    //       orientation.
    if (cvu.GetZ() > -MIN_VALUE)
    {
      u.SetX(hv.GetY());
      u.SetY(-hv.GetX());
    }
    else
    {
      u.SetX(-hv.GetY());
      u.SetY(hv.GetX());
    }
  }

  // Calculate approximate view-up vector
  temp = Cross(u, vdv);

  SetViewUp(temp);  // Set view-up vector
}

// Dolly viewing system along view direction vector
void ViewSys::Dolly( double dist_mult )
{
  Vector3 dolly_delta;  // Dolly delta vector

  // Calculate dolly delta vector
  dolly_delta = vdv * efd * dist_mult;

  // Update eye and focus positions
  eye_posn = eye_posn + dolly_delta;
  focus_posn = focus_posn + dolly_delta;

  // Update view system parameters
  BuildTransform();
}

// Orbit viewing system about focus position (where "horz"
// and "vert" rotation angles are in radians)
void ViewSys::Orbit( double horz, double vert )
{
  // Move eye position to focus position
  Dolly(1.0);

  // Rotate viewing system about focus position
  Rotate(-horz, -vert);

  // Update eye position
  Dolly(-1.0);
}

// Pan viewing system perpendicular to view direction vector
void ViewSys::Pan( double horz_mult, double vert_mult )
{
  Vector3 pan_delta;    // Pan delta vector

  // Calculate pan delta vector
  pan_delta = Cross(vdv, cvu) *  horz_mult * efd + cvu *
      vert_mult * efd;

  // Update eye and focus positions
  eye_posn = eye_posn + pan_delta;
  focus_posn = focus_posn + pan_delta;

  // Update view system parameters
  BuildTransform();
}

// Rotate viewing system about eye position (where "horz"
// and "vert" rotation angles are in radians)
void ViewSys::Rotate( double horz, double vert )
{
  double x, y;      // Horizontal plane coordinates
  Vector3 approx;   // Approximate view-up vector
  Vector3 u, v, n;  // Viewing system coordinate axes

  // Get viewing system cooordinate axes
  u = Cross(vdv, cvu);
  v = cvu;
  n = vdv;
  
  // Rotate view direction vector vertically through local
  // u-axis
  vdv = cos(vert) * n + sin(vert) * v;

  // Rotate view direction vector horizontally through
  // global z-axis
  x = vdv.GetX();
  y = vdv.GetY();
  vdv.SetX(cos(horz) * x + sin(horz) * y);
  vdv.SetY(cos(horz) * y - sin(horz) * x);
  vdv.Norm();

  // Rotate u-axis horizontally through global z-axis
  x = u.GetX();
  y = u.GetY();
  u.SetX(cos(horz) * x + sin(horz) * y);
  u.SetY(cos(horz) * y - sin(horz) * x);

  // Calculate approximate view-up vector
  approx = Cross(u, vdv);

  SetViewUp(approx);    // Set view-up vector

  // Update focus position
  focus_posn = eye_posn + vdv * efd;

  // Update view system parameters
  BuildTransform();
}

// Tilt viewing system view-up vector (where "tilt" angle is
// in radians)
void ViewSys::Tilt( double tilt )
{
  tilt_angle += tilt;   // Update tilt angle

  // Limit tilt angle range
  if (tilt_angle >= 2.0 * PI)
    tilt_angle -= 2.0 * PI;
  else if (tilt_angle < 0.0)
    tilt_angle += 2.0 * PI;

  // Update view system parameters
  BuildTransform();
}

// Zoom viewing system field of view
void ViewSys::Zoom( double vd_mult )
{
  double vd_delta;  // View distance delta

  // Calculate view distance delta
  vd_delta = -eye * (1.0 - vd_mult);

  // Update view distance
  eye *= vd_mult;

  // Update clipping plane distances
  fpd += vd_delta;
  bpd += vd_delta;

  // Update view system parameters
  BuildTransform();
}

