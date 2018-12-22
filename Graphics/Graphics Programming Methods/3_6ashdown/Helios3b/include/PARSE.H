////////////////////////////////////////////////////////////
//
/// PARSE.H - Environment Data File Parser Class
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

#ifndef _PARSE_H
#define _PARSE_H

#include "environ.h"
#include "transfm3.h"
#include "win_text.h"

#define MaxLine 256     // Maximum line length

typedef Patch3 *PatchPtr;       // Patch pointer data type
typedef Surface3 *SurfacePtr;   // Surface pointer data type
typedef Vertex3 *VertexPtr;     // Vertex pointer data type

class Parse     // Environment data file parser
{
  private:
    WORD elem_cnt;              // Instance element count
    WORD patch_cnt;             // Instance patch count
    WORD surf_cnt;              // Instance surface count
    WORD vert_cnt;              // Instance vertex count
    char ent_buff[MaxLine];     // Entity file name buffer
    char line_buff[MaxLine];    // Line buffer
    char msg_buff[MaxLine];     // Message buffer
    Environ *penv;              // Environment pointer
    PatchPtr *pp_array;         // Patch pointer array ptr
    SurfacePtr *ps_array;       // Surface pointer array ptr
    Transform3 tm;              // 3-D transformation matrix
    VertexPtr *pv_array;        // Vertex pointer array ptr
    WinText efile;              // Entity file
    WinText ifile;              // Instance file

    BOOL ParseElements();
    BOOL ParsePatches();
    BOOL ReadVector( WinText &, double *, double *,
       double * );
    Instance *ParseEntityFile();
    Surface3 *ParseSurfaces();
    Surface3 *ReadSurface();
    Vertex3 *ParseVertices();
    Vertex3 *ReadVertex();
    void ReadLine( WinText & );
    void ReadTransform();
    void TransformInstance( Instance * );

  public:
    BOOL ParseFile( char *, char *, Environ * );
};

#endif

