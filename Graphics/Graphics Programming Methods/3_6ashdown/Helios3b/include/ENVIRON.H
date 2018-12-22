////////////////////////////////////////////////////////////
//
//  ENVIRON.H - Environment Class Include File
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/11/26 - Added max_reflect data member.
//                       - Added GetMaxReflect and
//                         SetMaxReflect functions.
//                       - Modified Environ to initialize
//                         max_reflect.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/07/21 - Version 1.02B release.
//              96/02/14 - Version 1.02C release.
//              96/03/30 - Added max_x, max_y, max_z, min_x,
//                         min_y and min_z data members.
//                       - Added GetMax_X, GetMax_Y,
//                         GetMax_Z, GetMin_X, GetMin_Y and
//                         GetMin_Z functions.
//                       - Added CalcExtents function
//                         prototype.
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

#ifndef _ENVIRON_H
#define _ENVIRON_H

#include "instance.h"

class Environ   // Environment
{
  private:
    double max_reflect;     // Maximum reflected color
    double max_x;           // Maximum x-axis extent
    double max_y;           // Maximum y-axis extent
    double max_z;           // Maximum z-axis extent
    double min_x;           // Minimum x-axis extent
    double min_y;           // Minimum y-axis extent
    double min_z;           // Minimum z-axis extent
    Instance *pinsthd;      // Instance list pointer
    WORD num_inst;          // Number of instances
    WORD num_surf;          // Number of surfaces
    WORD num_patch;         // Number of patches
    WORD num_elem;          // Number of elements
    WORD num_vert;          // Number of vertices

    friend class Parse;

  public:
    Environ()
    {
      pinsthd = NULL;
      max_reflect = 0.0;
    }

    ~Environ() { DeleteEnv(); }

    double GetMaxReflect() { return max_reflect; }
    double GetMax_X() { return max_x; }
    double GetMax_Y() { return max_y; }
    double GetMax_Z() { return max_z; }
    double GetMin_X() { return min_x; }
    double GetMin_Y() { return min_y; }
    double GetMin_Z() { return min_z; }
    Instance *GetInstPtr() { return pinsthd; }
    WORD GetNumInst() { return num_inst; }
    WORD GetNumSurf() { return num_surf; }
    WORD GetNumPatch() { return num_patch; }
    WORD GetNumElem() { return num_elem; }
    WORD GetNumVert() { return num_vert; }
    void CalcExtents();
    void DeleteEnv()
    {
      Instance *pinst;  // Instance pointer
      Instance *pnext;  // Next instance pointer
  
      pinst = pinsthd;
      while (pinst != NULL)
      {
        pnext = pinst->GetNext();
        delete pinst;
        pinst= pnext;
      }
      pinsthd = NULL;
    }
    void SetMaxReflect( double mr ) { max_reflect = mr; }
};

#endif

