////////////////////////////////////////////////////////////
//
//  FF_CLIP.H - Form Factor Polygon Clipper Class
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

#ifndef _FF_CLIP_H
#define _FF_CLIP_H

#include "ff_poly.h"

// View normalization parameters
static const double FPD = -0.99;
static const double BPD = MAX_VALUE;
static const double EYE = -1.0;
static const double SN = (EYE - BPD) * (EYE - FPD) / (EYE *
    EYE * (BPD - FPD));
static const double RN = FPD * (EYE - BPD) / (EYE * (FPD -
    BPD));

class FormClipEdge      // Edge-plane clipper
{
  private:
    FormClipEdge *pnext;        // Next clipper pointer
    Vector4 normal;             // Plane normal
    Vector4 first;              // First vertex
    Vector4 start;              // Start vertex
    BOOL first_inside;          // First vertex inside flag
    BOOL start_inside;          // Start vertex inside flag
    BOOL first_flag;            // First vertex seen flag

    BOOL IsInside( Vector4 &v )
    { return (Dot(normal, v) >= 0.0); }
    Vector4 Intersect( Vector4 &, Vector4 & );
    void Output( Vector4 &, FormPoly & );

  public:
    FormClipEdge() { first_flag = FALSE; }

    void Add( FormClipEdge *pc ) { pnext = pc; }
    void Clip( Vector4 &, FormPoly & );
    void Close( FormPoly & );
    void SetNormal( Vector4 &n ) { normal = n; }
};

class FormClip  // Form factor polygon clipper
{
  protected:
    int num_vert;               // # of polygon vertices
    Vector3 u, v, n;            // View system co-ordinates
    double vtm[4][4];           // Transformation matrix
    FormClipEdge clipper[5];    // Clipper array
    FormClipEdge *pclip;        // Clipper list head pointer
    Point3 center;              // Polygon center

    Vector3 RandomVector()      // Generate random vector
    {
      Vector3 temp;     // Temporary vector

      temp.SetX(GetNormRand() * 2.0 - 1.0);
      temp.SetY(GetNormRand() * 2.0 - 1.0);
      temp.SetZ(GetNormRand() * 2.0 - 1.0);

      return temp;
    }

  public:
    BOOL BackFaceCull( Patch3 *ppatch )
    {
      Vector3 view;     // View vector
      
      // Calculate view vector
      view = Vector3(ppatch->GetVertexPtr(0)->GetPosn(),
          center);
          
      // Indicate whether patch is backface
      return (Dot(ppatch->GetNormal(), view) < MIN_VALUE) ?
          TRUE : FALSE;
    }

    int Clip( Element3 *, FormPoly &, WORD );
};

#endif

