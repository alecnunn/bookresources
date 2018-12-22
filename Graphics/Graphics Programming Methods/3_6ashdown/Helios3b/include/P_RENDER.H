////////////////////////////////////////////////////////////
//
//  P_RENDER.H - Polygon Renderer Class Include File
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/10/12 - Modified PolyRender constructor
//                         to initialize intensity and clear
//                         intensity_flag.
//                       - Added intensity and
//                         intensity_flag data members.
//                       - Added IntensityFlag,
//                         GetIntensity, DisableIntensity,
//                         EnableIntensity and SetIntensity
//                         functions.
//              94/10/14 - Modified PolyRender constructor
//                         to initialize contrast and clear
//                         contrast_flag.
//                       - Added contrast and contrast_flag
//                         data members.
//                       - Added ContrastFlag, GetContrast,
//                         DisableContrast, EnableContrast
//                         and SetContrast functions.
//              94/11/26 - Added color_scale and
//                         intensity_adj data members.
//                       - Added CalcColorScale function.
//                       - Modified Open function prototype.
//                       - Added P_MaxColorBand definition.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/03/21 - Modified Render and GetVertexInfo
//                         function prototypes.
//              95/03/22 - Modified CalcColorScale function
//                         to prevent divide-by-zero error.
//                       - Added Reset function prototype.
//              95/03/25 - Removed "c_jitter.h" and
//                         "gamma.h" include directives.
//                       - Deleted "jitter_flag" and
//                         "gamma_flag" data members.
//                       - Deleted "gamma" and "jitter"
//                         members.
//                       - Removed "gamma_flag" and
//                         "jitter_flag" initialization from
//                         PolyRender constructor.
//                       - Removed DisableGamma,
//                         DisableJitter, EnableGamma,
//                         EnableJitter, GammaFlag,
//                         GetGamma, GetNoiseLevel,
//                         GetStatus, JitterFlag, SetGamma
//                         and SetNoiseLevel functions.
//              95/06/05 - Changed PolyRender data member
//                         z_buffer from float to long.
//                       - Changed ScanInfo data member x
//                         from double to int data type.
//                       - Changed ScanInfo data member z
//                         from double to long data type.
//                       - Changed ScanInfo data member
//                         color from Spectra to IntRGB
//                         data type.
//                       - Changed PR_Infinity from float
//                         to long data type.
//                       - Added ScaleZ function.
//                       - Added DDA_Info structure.
//                       - Added pi, si, r, inc, and dec
//                         data members.
//                       - Added LongFloorDiv and
//                         IntFloorDiv functions.
//                       - Added EdgeSetup, EdgeScan,
//                         SpanSetup and SpanScan function
//                         prototypes.
//              95/06/17 - Deleted color_type data member
//                         from PolyRender.
//                       - Removed color_type initialization
//                         from PolyRender constructor.
//                       - Deleted GetColorType and
//                         SetColorType functions.
//                       - Deleted PR_RGB, PR_MONO and
//                         PR_PSEUDO defintions.
//              95/07/05 - Added ss_rate, ss_ymax and
//                         ss_ymin data members to
//                         PolyRender class.
//                       - Added ss_screen data member to
//                         VertexInfo class.
//                       - Modified Render function
//                         prototype.
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

#ifndef _P_RENDER_H
#define _P_RENDER_H

#include <limits.h>
#include "out_poly.h"
#include "win_bmap.h"

// Maximum reflected color band value
static const double P_MaxColorBand = ((double) 254 /
    (double) 255);

// Maximum Z-buffer value
static const long PR_Infinity = LONG_MAX;

struct DDA_Info     // Edge/span DDA info
{
  int x;            // x-axis value (edge only)
  long z;           // z-axis value
  IntRGB color;     // RGB color values
};

struct VertexInfo       // Vertex information
{
  POINT ss_screen;      // Supersampling screen co-ords
  POINT screen;         // Integer screen co-ordinates
  Point3 posn;          // Scaled position
  Spectra color;        // Spectral radiant exitance
};

struct ScanInfo         // Scan line intersection info
{
  int x;                // X-axis co-ordinate
  long z;               // Pseudodepth
  IntRGB color;         // RGB color
};

struct EdgeInfo         // Edge information
{
  BOOL first;           // First intersection flag
  ScanInfo isect[2];    // Scan line intersection array
};

class PolyRender        // Polygon renderer
{
  private:
    BOOL contrast_flag;         // Contrast flag
    BOOL intensity_flag;        // Intensity flag
    int ymin;                   // Minimum y-axis co-ord
    int ymax;                   // Maximum y-axis co-ord
    int ss_ymin;                // Min supersampling co-ord
    int ss_ymax;                // Max supersampling co-ord
    int width;                  // Display width
    int height;                 // Display height
    int num_vert;               // Number of vertices
    int ss_rate;                // Supersampling rate
    double contrast;            // Contrast scaling factor
    double intensity;           // Intensity scaling factor
    double color_scale;         // Color scaling factor
    long **z_buffer;            // Depth buffer pointer
    EdgeInfo *edge_list;        // Edge list pointer
    DDA_Info pi;                // DDA pixel parameters
    DDA_Info si;                // DDA delta values
    DDA_Info r;                 // DDA offset error terms
    DDA_Info inc;               // DDA increment values
    DDA_Info dec;               // DDA decrement values
    Spectra intensity_adj;      // Intensity adjustment
    VertexInfo v_info[8];       // Vertex info table
    WinBitmap *pbmap;           // Bitmap object pointer

    // Calculate integer floor(a / b) - assumes b > 0
    int IntFloorDiv( int a, int b )
    {
      if (a >= 0)
        return a / b;
      else
        return (a / b) + ((a % b) == 0 ? 0 : - 1);
    }

    // Calculate long floor(a / b) - assumes b > 0
    long LongFloorDiv( long a, long b )
    {
      if (a >= 0)
        return a / b;
      else
        return (a / b) + ((a % b) == 0 ? 0 : - 1);
    }

    // Scale z-axis value from float to long (assumes
    // range is 0.0 to 1.0)
    long ScaleZ( double x )
    { return ((long) (x * (double) LONG_MAX)); }

    void EdgeSetup( VertexInfo *, VertexInfo * );
    void EdgeScan( VertexInfo *, VertexInfo * );
    void GetVertexInfo( OutPolygon &, double, double );
    void ScanEdges();
    void DrawEdgeList();
    void SpanSetup( ScanInfo *, ScanInfo * );
    void SpanScan( ScanInfo *, ScanInfo *, int );

  public:
    PolyRender()
    {
      contrast_flag = FALSE;
      intensity_flag = FALSE;
      contrast = 1.0;
      intensity = 0.0;
    }

    BOOL ContrastFlag() { return contrast_flag; }
    BOOL IntensityFlag() { return intensity_flag; }
    BOOL Open( WinBitmap *, double );
    double GetContrast() { return contrast; }
    double GetIntensity() { return intensity; }
    void CalcColorScale( double rmax )
    {
      if (rmax > MIN_VALUE)
        color_scale = P_MaxColorBand / rmax;
      else
        color_scale = 1.0;
    }
    void Close();
    void DisableContrast() { contrast_flag = FALSE; }
    void DisableIntensity() { intensity_flag = FALSE; }
    void EnableContrast() { contrast_flag = TRUE; }
    void EnableIntensity() { intensity_flag = TRUE; }
    void Render( OutPolygon &, int, double, double );
    void Reset();
    void SetContrast( double c ) { contrast = c; }
    void SetIntensity( double i )
    {
      intensity = i;
      intensity_adj.SetRedBand((float) i);
      intensity_adj.SetGreenBand((float) i);
      intensity_adj.SetBlueBand((float) i);
    }
};

#endif

