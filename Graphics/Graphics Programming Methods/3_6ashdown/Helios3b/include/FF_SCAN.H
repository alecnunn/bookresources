////////////////////////////////////////////////////////////
//
//  FF_SCAN.H - Form Factor Scan Conversion Class Include
//              File
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/07/21 - Version 1.02B release.
//              96/02/14 - Version 1.02C release.
//              96/04/01 - Version 1.03A release.
//              96/11/17 - Added float typecast to
//                         FF_Infinity constant.
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

#ifndef _FF_SCAN_H
#define _FF_SCAN_H

#include "ff_scan.h"
#include "ff_poly.h"

static const float FF_Infinity = (float) MAX_VALUE;
static const WORD FF_None = 0;

struct FormCellInfo     // Face cell information
{
  float depth;          // Polygon cell depth
  WORD id;              // Polygon identifier
};

struct FormVertexInfo   // Vertex information
{
  struct                // Face cell array offsets
  {
    int x;              // Width offset
    int y;              // Height offset
  }
  face;
  Point3 posn;          // Scaled position
};

struct FormScanInfo     // Scan line intersection info
{
  double x;             // X-axis co-ordinate
  double z;             // Pseudodepth
};

struct FormEdgeInfo     // Edge information
{
  BOOL first;               // First intersection flag
  FormScanInfo isect[2];    // Scan line intersection array
};

// Form factor polygon scan conversion (abstract class)
class FormScan
{
  protected:
    BOOL status;                // Object status
    int ymin;                   // Minimum y-axis co-ord
    int ymax;                   // Maximum y-axis co-ord
    int num_vert;               // Number of vertices
    FormCellInfo **cell_buffer; // Cell info buffer ptr
    FormEdgeInfo *edge_list;    // Edge list pointer
    FormVertexInfo v_info[8];   // Vertex info table
    WORD poly_id;               // Polygon identifier

    virtual void DrawEdgeList() = 0;
    void GetVertexInfo( FormPoly & );
    void ScanEdges();

  public:
    virtual ~FormScan() { };

    BOOL GetStatus() { return status; }
    void Scan( FormPoly & );
};

#endif

