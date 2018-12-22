////////////////////////////////////////////////////////////
//
//  INSTANCE.H - Instance Class
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

#ifndef _INSTANCE_H
#define _INSTANCE_H

#include "surface3.h"

class Instance          // Instance (also entity)
{
  private:
    Surface3 *pshead;   // Surface list head pointer
    Vertex3 *pvhead;    // Vertex list head pointer
    Instance *pnext;    // Next instance pointer

  public:
    Instance( Vertex3 *pv, Surface3 *ps )
    {
      pvhead = pv;
      pshead = ps;
      pnext = NULL;
    }

    ~Instance()
    {
      Surface3 *psnext;
      Surface3 *ps = pshead;
      Vertex3 *pvnext;
      Vertex3 *pv = pvhead;

      // Delete surfaces
      while (ps != NULL)
      {
        psnext = ps->GetNext();
        delete ps;
        ps = psnext;
      }

      // Delete vertices
      while (pv != NULL)
      {
        pvnext = pv->GetNext();
        delete pv;
        pv = pvnext;
      }
    }

    Instance *GetNext() { return pnext; }
    Surface3 *GetSurfPtr() { return pshead; }
    Vertex3 *GetVertPtr() { return pvhead; }
    void SetNext( Instance *pn ) { pnext = pn; }
    void SetSurfPtr( Surface3 *ps ) { pshead = ps; }
    void SetVertPtr( Vertex3 *pv ) { pvhead = pv; }
};

#endif

