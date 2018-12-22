////////////////////////////////////////////////////////////
//
//  VIEW_SYS.H - Viewing System Class Include File
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/12/16 - Version 1.01A release.
//              94/12/24 - Added par_flag and scale data
//                         members.
//                       - Added GetParProjFlag,
//                         GetWindowScale, SetParProjFlag,
//                         and SetWindowScale functions.
//              95/02/05 - Version 1.02A release.
//              95/07/21 - Version 1.02B release.
//              96/02/14 - Version 1.02C release.
//              96/03/30 - Added InitViewSystem function
//                         prototype.
//                       - Added "environ.h" include
//                         directive.
//                       - Added VS_FILL_FACTOR definition.
//                       - Added efd, tilt_angle, cvu and
//                         focus_posn data members.
//                       - Added GetFocusPosn, SetFocusPosn,
//                         GetTiltAngle and SetTiltAngle
//                         functions.
//                       - Added Dolly, Orbit, Pan, Rotate,
//                         Tilt, Zoom and SetEyeFocusPosn
//                         function prototypes.
//                       - Modified ViewSys constructor to
//                         initialize focus_posn, cvu, efd
//                         and tilt_angle.
//                       - Deleted GetViewDir, GetViewUp,
//                         SetViewDir and SetEyePosn
//                         functions.
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

#ifndef _VIEW_SYS_H
#define _VIEW_SYS_H

#include "patch3.h"
#include "environ.h"

// Extents fill factor
static const double VS_FILL_FACTOR = 0.9;

class ViewSys           // Viewing system
{
  private:
    BOOL par_flag;      // Parallel projection flag
    double bpd;         // Back plane distance
    double eye;         // View distance
    double efd;         // Eye-focus distance
    double fpd;         // Front plane distance
    double scale;       // Window scale
    double tilt_angle;  // Tilt angle (degrees)
    Point3 origin;      // View space origin
    Point3 eye_posn;    // Eye position
    Point3 focus_posn;  // Focus position
    Vector3 cvu;        // Canonical view-up vector
    Vector3 vdv;        // View direction vector
    Vector3 vuv;        // View-up vector
    double ptm[4][4];   // Projective transformation matrix

  protected:
    double aspect;      // Aspect ratio

    BOOL BackFaceCull( Patch3 *);
    double (*GetProjMatrix())[4];
    void BuildTransform();

  public:
    ViewSys()
    {
      par_flag = FALSE;
      aspect = 1.0;
      fpd = -0.99;
      bpd = 10000.0;
      eye = -1.0;
      efd = 1.0;
      scale = 1.0;
      tilt_angle = 0.0;
      eye_posn = Point3(-1.0, 0.0, 0.0);
      focus_posn = Point3(0.0, 0.0, 0.0);
      origin = Point3(0.0, 0.0, 0.0);
      cvu = Vector3(0.0, 0.0, 1.0);
      vdv = Vector3(-1.0, 0.0, 0.0);
      vuv = Vector3(0.0, 0.0, 1.0);

      BuildTransform();         // Initialize matrix
    }

    BOOL GetParProjFlag() { return par_flag; }
    double GetBackDist() { return bpd; }
    double GetFrontDist() { return fpd; }
    double GetTiltAngle() { return tilt_angle; }
    double GetWindowScale() { return scale; }
    double GetViewDist() { return -eye; }
    Point3 &GetEyePosn() { return eye_posn; }
    Point3 &GetFocusPosn() { return focus_posn; }
    Point3 &GetOrigin() { return origin; }
    void Dolly( double );
    void InitViewSystem( Environ * );
    void Orbit( double, double );
    void Pan( double, double );
    void Rotate( double, double );
    void SetBackDist( double b ) { bpd = b; }
    void SetEyeFocusPosn( Point3 &, Point3 & );
    void SetFrontDist( double f ) { fpd = f; }
    void SetOrigin( Point3 &o ) { origin = o; }
    void SetParProjFlag( BOOL f ) { par_flag = f; }
    void SetTiltAngle( double t ) { tilt_angle = t; }
    void SetViewDist( double e ) { eye = -e; }
    void SetViewUp( Vector3 & );
    void SetWindowScale( double s ) { scale = s; }
    void Tilt( double );
    void Zoom( double );
};

// Return projective transformation matrix pointer
inline double (*ViewSys::GetProjMatrix())[4]
{ return ptm; }

#endif

