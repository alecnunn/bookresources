////////////////////////////////////////////////////////////
//
//  SYN_CAM.H - Synthetic Camera Class Include File
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/10/12 - Removed EnableGamma function call
//                         from SynCamera constructor.
//                       - Added IntensityFlag,
//                         GetIntensity, DisableIntensity,
//                         EnableIntensity and SetIntensity
//                         functions.
//              94/10/14 - Added ContrastFlag, GetContrast,
//                         DisableContrast, EnableContrast
//                         and SetContrast functions.
//              94/11/26 - Modified Preview and Shoot
//                         function prototypes.
//                       - Changed "instance.h" include
//                         directive to "environ.h".
//                       - Added Shade function prototype.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/03/21 - Added OpenBuffer, CloseBuffer,
//                         InstBartlettFilter, GetMaxPass,
//                         GetNumFilters, GetCurrFilterName,
//                         GetCurrFilterSize, GetFilterName
//                         and RenderPolygons function
//                         prototypes.
//                       - Added AntiAliasFlag, GetNumPass
//                         and SetFilterType functions.
//                       - Added Bartlett and BartParam
//                         data structures.
//                       - Added pabuf, pbart, num_sub,
//                         pass, fp_flag and BartlettParam
//                         data members.
//              95/03/25 - Added "c_jitter.h" and "gamma.h"
//                         include directives.
//                       - Added jitter_flag and gamma_flag
//                         gamma_flag data members.
//                       - Added gamma and jitter members.
//                       - Added gamma_flag, jitter_flag,
//                         pass and fp_flag initialization
//                         to SynCamera constructor.
//                       - Modified DisableGamma,
//                         DisableJitter, EnableGamma,
//                         EnableJitter, GammaFlag,
//                         GetGamma, GetNoiseLevel,
//                         GetStatus, JitterFlag, SetGamma
//                         and SetNoiseLevel functions.
//                       - Added SC_SuperRes enum constants.
//                       - Modified Shoot function
//                         prototype.
//              95/06/17 - Added color_type data member to
//                         SynCamera.
//                       - Modified GetColorType and
//                         SetColorType functions.
//                       - Added color_type initialization
//                         to SynCamera constructor.
//                       - Added SC_RGB, SC_MONO and
//                         SC_PSEUDO definitions.
//              95/06/28 - Added Reset function prototype.
//              95/07/21 - Version 1.02B release.
//              96/02/14 - Version 1.02C release.
//              96/02/21 - Added Abort function prototype.
//              96/03/30 - Deleted vdv_horz, vdv_vert,
//                         vup_horz and vup_vert data
//                         members from SynCamera.
//                       - Removed view director vector and
//                         view-up vector initialization
//                         from SynCamera constructor.
//                       - Deleted GetViewDirHorz,
//                         GetViewDirVert, GetViewUpHorz,
//                         and GetViewUpVert functions from
//                         SynCamera.
//                       - Deleted SetViewDirVector and
//                         and SetViewUpVector functions
//                         from SynCamera.
//                       - Deleted "environ.h" include
//                         directive.
//              96/04/01 - Version 1.03A release.
//              96/11/29 - Deleted unreferenced parameters
//                         in SynCamera constructor.
//              97/03/01 - Added default initialization
//                         values to SynCamera constructor.
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

#ifndef _SYN_CAM_H
#define _SYN_CAM_H

#include "p_clip4.h"
#include "gamma.h"
#include "c_jitter.h"
#include "p_render.h"
#include "win_meta.h"
#include "win_bmap.h"
#include "view_sys.h"

// Supersampling resolution identifiers
enum SC_SuperRes
{ SC_None, SC_Bartlett_3, SC_Bartlett_5, SC_Bartlett_7 };

// Color type definitions
static const int SC_RGB = 0;       // RGB color
static const int SC_MONO = 1;      // Grayscale
static const int SC_PSEUDO = 2;    // Pseudocolor

// Extents fill factor
static const double SC_FILL_FACTOR = 0.9;

#ifdef _NOT_WINAPP
struct POINT    // Raster display point
{
  int x;        // X-axis co-ordinate
  int y;        // Y-axis co-ordinate
};
#endif

struct Bartlett         // Bartlett filter element
{
  double x_offset;      // x-axis subpixel offset
  double y_offset;      // y-axis subpixel offset
  int alpha;            // Subpixel blend coefficient
  int beta;             // Pixel blend coefficient
  int weight;           // Subpixel weight
};

struct BartParam        // Bartlett filter parameters
{
  double offset;        // Subpixel offset
  int width;            // Filter array width
  char *psize;          // Filter size pointer
  char *pname;          // Filter name pointer
};

class SynCamera : public ViewSys        // Synthetic camera
{
  private:
    int color_type;         // Display color type
    int height;             // Window height
    int width;              // Window width
    int num_sub;            // Number of subpixels
    int pass;               // Number of passes
    int ss_res;             // Supersampling resolution
    BOOL fp_flag;           // First pass flag
    BOOL gamma_flag;        // Gamma correction flag
    BOOL jitter_flag;       // Color reduction flag
    Bartlett *pbart;        // Bartlett filter pointer
    ColorJitter jitter;     // Color reduction filter
    ColorRGB **pabuf;       // Antialiasing buffer pointer
    Gamma gamma;            // Gamma correction object
    PolyClip4 clipper;      // Polygon clipper
    PolyRender renderer;    // Polygon renderer

    // Bartlett filter parameters array
    static BartParam BartlettParam[];

    BOOL InstBartlettFilter();
    BOOL OpenBuffer( WinBitmap * );
    void CloseBuffer();
    void RenderPolygons( Environ *, int, double, double );

  public:
    SynCamera( int w = 640, int h = 480 ) : ViewSys()
    {
      width = w; height = h;
      aspect = (double) w / (double) h;
      fp_flag = TRUE;
      color_type = SC_RGB;
      pass = 0;
      ss_res = SC_None;
      gamma_flag = TRUE;
      jitter_flag = FALSE;
    }

    BOOL AntiAliasFlag()
    { return ss_res != SC_None ? TRUE: FALSE; }
    BOOL ContrastFlag() { return renderer.ContrastFlag(); }
    BOOL GammaFlag() { return gamma_flag; }
    BOOL GetStatus() { return jitter.GetStatus(); }
    BOOL IntensityFlag()
    { return renderer.IntensityFlag(); }
    BOOL JitterFlag() { return jitter_flag; }
    BOOL Preview( Environ *, WinMetaFile * );
    BOOL Shade( Environ *, WinBitmap * );
    BOOL Shoot( Environ *, WinBitmap *, BOOL * );
    char *GetCurrFilterName();
    char *GetCurrFilterSize();
    char *GetFilterSize( int );
    double GetContrast() { return renderer.GetContrast(); }
    double GetGamma() { return gamma.GetGamma(); }
    double GetIntensity()
    { return renderer.GetIntensity(); }
    int GetColorType() { return color_type; }
    int GetHeight() { return height; }
    int GetWidth() { return width; }
    int GetNoiseLevel() { return jitter.GetNoiseLevel(); }
    int GetMaxPass();
    int GetNumFilters();
    int GetNumPass() { return pass; }
    void Abort();
    void DisableContrast() { renderer.DisableContrast(); }
    void DisableGamma() { gamma_flag = FALSE; }
    void DisableIntensity() { renderer.DisableIntensity(); }
    void DisableJitter() { jitter_flag = FALSE; }
    void EnableContrast() { renderer.EnableContrast(); }
    void EnableGamma() { gamma_flag = TRUE; }
    void EnableIntensity() { renderer.EnableIntensity(); }
    void EnableJitter() { jitter_flag = TRUE; }
    void Reset();
    void SetFilterType( int t ) { ss_res = t; }
    void SetColorType( int t ) { color_type = t; }
    void SetContrast( double c )
    { renderer.SetContrast(c); }
    void SetGamma( double g ) { gamma.SetGamma(g); }
    void SetHeight( int h ) { height = h; }
    void SetIntensity( double i )
    { renderer.SetIntensity(i); }
    void SetNoiseLevel( int n ) { jitter.SetNoiseLevel(n); }
    void SetWidth( int w ) { width = w; }
    void UpdateViewSystem();
};

#endif

