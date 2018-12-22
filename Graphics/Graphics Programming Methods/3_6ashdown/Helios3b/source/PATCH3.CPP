////////////////////////////////////////////////////////////
//
//  PATCH3.CPP - 3-D Patch Classes
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/07/21 - Version 1.02B release.
//              96/02/14 - Version 1.02C release.
//              96/04/01 - Version 1.03A release.
//              97/03/13 - Modified Patch3::CalcCenter to
//                         avoid calling Vector3::+=
//                         operator.
//              97/04/01 - Version 1.03B release.
//
//  Compilers:  Microsoft Visual C/C++ Professional V5.0
//              Borland C++ Version 4.0
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

#include "patch3.h"

void Vertex3::CalcNormal()      // Calculate vertex normal
{
  ElemList *pelist = pelhd;     // Element list pointer

  // Sum element normals
  while (pelist != NULL)
  {
    normal += pelist->GetElemPtr()->GetNormal();
    pelist = pelist->GetNext();
  }

  normal.Norm();        // Normalize vector
}
 
void Element3::CalcArea()       // Calculate element area
{
  Vector3 temp;     // Temporary 3-D vector

  Vector3 va(pvertex[0]->GetPosn(), pvertex[1]->GetPosn());
  Vector3 vb(pvertex[0]->GetPosn(), pvertex[2]->GetPosn());

  temp = Cross(va, vb);
  area = (float) (temp.Length() / 2.0);

  if (IsQuad() == TRUE)
  {
    Vector3 vc(pvertex[3]->GetPosn(),
        pvertex[0]->GetPosn());

    temp = Cross(vb, vc);
    area += (float) (temp.Length() / 2.0);
  }
}

void Element3::CalcNormal()     // Calculate element normal
{
  Vector3 va(pvertex[0]->GetPosn(), pvertex[1]->GetPosn());
  Vector3 vb(pvertex[0]->GetPosn(), pvertex[2]->GetPosn());

  normal = Cross(va, vb);
  normal.Norm();
}

void Patch3::CalcCenter()       // Calculate patch centroid
{
  int i;            // Loop index
  int num_vert;     // Number of vertices
  Vector3 cv;       // Centroid vector

  num_vert = GetNumVert();

  // Initialize centroid vector to origin
  cv = Vector3(0.0, 0.0, 0.0);

  // Determine patch centroid
  for (i = 0; i < num_vert; i++)
    cv = cv + Vector3(pvertex[i]->GetPosn());

  cv /= (double) num_vert;

  // Convert centroid vector to 3-D point
  center.SetX(cv.GetX());
  center.SetY(cv.GetY());
  center.SetZ(cv.GetZ());
}

