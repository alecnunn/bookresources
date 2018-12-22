////////////////////////////////////////////////////////////
//
//  SYN_CAM.CPP - Synthetic Camera Class
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/11/26 - Modified Preview and Shoot
//                         functions.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/03/21 - Added antialiasing support to
//                         Shoot function.
//                       - Added OpenBuffer, CloseBuffer,
//                         InstBartlettFilter, GetMaxPass,
//                         GetNumFilters and RenderPolygons
//                         functions.
//                       - Added BartlettParam array.
//              95/03/25 - Added gamma correction and color
//                         reduction to Shoot function.
//              95/06/17 - Added color conversion logic to
//                         Shoot function.
//              95/06/28 - Added Reset function.
//              95/07/21 - Version 1.02B release.
//              95/10/11 - Modified WinMeta::Record function
//                         call.
//              96/02/14 - Version 1.02C release.
//              96/02/21 - Added Abort function.
//              96/03/30 - Deleted SetViewDirVector and
//                         SetViewUpVector functions.
//                       - Deleted "spheric3.h" include
//                         directive.
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

#include "syn_cam.h"

// Static Bartlett filter parameters array
BartParam SynCamera::BartlettParam[] =
{
  { 0.0000, 1, "None", "None" },
  { 0.5000, 3, "3 x 3", "3 x 3 Bartlett" },
  { 0.3333, 5, "5 x 5", "5 x 5 Bartlett" },
  { 0.2500, 7, "7 x 7", "7 x 7 Bartlett" }
};

// Record wireframe display in metafile format
BOOL SynCamera::Preview( Environ *penv, WinMetaFile
    *pmeta )
{
  int i;                // Loop index
  int num_vert;         // Number of vertices
  Element3 *pelem;      // Element pointer
  Instance *pinst;      // Instance pointer
  OutPolygon out;       // Output polygon
  POINT vertex[8];      // Polygon vertex array
  Point3 posn;          // Point co-ordinates
  Patch3 *ppatch;       // Patch pointer
  Surface3 *psurf;      // Surface pointer

  // Start wireframe metafile recording
  if (pmeta->Record(TRUE) == FALSE)
    return FALSE;

  // Walk the instance list
  pinst = penv->GetInstPtr();
  while (pinst != NULL)
  {
    // Walk the surface list
    psurf = pinst->GetSurfPtr();
    while (psurf != NULL)
    {
      // Walk the patch list
      ppatch = psurf->GetPatchPtr();
      while (ppatch != NULL)
      {
        // Determine patch visibility
        if (BackFaceCull(ppatch) == FALSE)
        {
          // Walk the element list
          pelem = ppatch->GetElementPtr();
          while (pelem != NULL)
          {
            // Clip the 3-D element (polygon)
            num_vert = clipper.Clip(pelem, out,
                GetProjMatrix());
                
            // Initialize the 2-D polygon vertices array
            for (i = 0; i < num_vert; i++)
            {
              posn  = out.GetVertexPosn(i);

              // Convert normalized device co-ordinates to
              // screen space co-ordinates
              vertex[i].x = (int) (posn.GetX() * width);
              vertex[i].y = (int) (posn.GetY() * height);
            }

            // Add 2-D polygon draw command to metafile
            if (pmeta->Polygon(vertex, num_vert) == FALSE)
            {
              pmeta->Erase();   // Erase metafile recording
              return FALSE;
            }

            pelem = pelem->GetNext();
          }
        }
        ppatch = ppatch->GetNext();
      }
      psurf = psurf->GetNext();
    }
    pinst = pinst->GetNext();
  }
  return pmeta->Stop();         // Stop metafile recording
}

// Record rendered display as bitmap file
BOOL SynCamera::Shoot( Environ *penv, WinBitmap *pbmap, BOOL
    *pmore )
{
  int row;              // Row index
  int col;              // Column index
  WORD alpha;           // Subpixel blend coeff
  WORD beta;            // Pixel blend coefficient
  ColorRGB *ppixel;     // Pixel pointer
  ColorRGB new_rgb;     // New pixel color

  // Initialize polygon renderer
  if (fp_flag == TRUE)
  {
    if (renderer.Open(pbmap, penv->GetMaxReflect()) == FALSE)
      return FALSE;
  }
      
  if (ss_res != SC_None)    // Antialiasing required ?
  {
    if (fp_flag == TRUE)
    {
      // Open antialiasing buffer
      if (OpenBuffer(pbmap) == TRUE)
        fp_flag = FALSE;
      else
      {
        // Close the polygon renderer
        renderer.Close();

        return FALSE;
      }
    }

    // Get Bartlett filter blending info
    alpha = (WORD) pbart[pass].alpha;
    beta = (WORD) pbart[pass].beta;

    // Render subpixel bitmap
    RenderPolygons(penv, BartlettParam[ss_res].width,
        pbart[pass].x_offset, pbart[pass].y_offset);

    // Blend bitmaps
    for (row = 0; row < height; row++)
    {
      for (col = 0; col < width; col++)
      {
        // Get new subpixel value
        pbmap->GetPixel(col, row, &new_rgb);

        // Get current pixel value
        ppixel = &(pabuf[row][col]);

        // Blend pixel and subpixel values
        ppixel->SetRed((BYTE) (((WORD) new_rgb.GetRed() *
            alpha + (WORD) ppixel->GetRed() * beta) /
            256));
        ppixel->SetGreen((BYTE) (((WORD) new_rgb.GetGreen()
            * alpha + (WORD) ppixel->GetGreen() * beta) /
            256));
        ppixel->SetBlue((BYTE) (((WORD) new_rgb.GetBlue()
            * alpha + (WORD) ppixel->GetBlue() * beta) /
            256));
      }
    }

    renderer.Reset();       // Clear the subpixel bitmap
    pass++;                 // Increment the pass count

    if (pass < num_sub)     // More passes needed ?
    {
      *pmore = TRUE;
      return TRUE;
    }

    // Copy antialiasing buffer to bitmap
    for (row = 0; row < height; row++)
      for (col = 0; col < width; col++)
        pbmap->SetPixel(col, row, pabuf[row][col]);

    CloseBuffer();      // Close the antialiasing buffer
  }
  else
  {
    // Render bitmap (no antialiasing)
    RenderPolygons(penv, 1, 0.0, 0.0);
  }

  renderer.Close();     // Close the polygon renderer

  for (row = 0; row < height; row++)
    for (col = 0; col < width; col++)
    {
      // Get the bitmap pixel
      pbmap->GetPixel(col, row, &new_rgb);

      switch (color_type)
      {
        case SC_MONO:           // Convert to grayscale
          new_rgb.SetMono();
          break;
        case SC_PSEUDO:         // Convert to pseudocolor
          new_rgb.SetPseudo();
          break;
        default:
          break;
      }

      if (gamma_flag == TRUE)
      {
        // Perform gamma correction
        gamma.Correct(new_rgb);
      }

      if (jitter_flag == TRUE)
      {
        // Perform color reduction
        jitter.Reduce(&new_rgb, col, row);
      }

      // Set bitmap pixel
      pbmap->SetPixel(col, row, new_rgb);
    }

  pass = 0;         // Reset number of passes
  fp_flag = TRUE;   // Reset first pass flag
  *pmore = FALSE;   // Indicate completion

  return TRUE;
}

// Abort rendering
void SynCamera::Abort()
{
  CloseBuffer();        // Close the antialiasing buffer
  renderer.Close();     // Close the polygon renderer
  Reset();              // Reset the camera
}

// Reset camera
void SynCamera::Reset()
{
  pass = 0;
  fp_flag = TRUE;
}

// Set view system parameters
void SynCamera::UpdateViewSystem()
{
  aspect = (double) width / (double) height;
  BuildTransform();
}

// Get maximum number of passes
int SynCamera::GetMaxPass()
{
  int width;    // Array width

  width = BartlettParam[ss_res].width;
  return width * width;
}

// Get current antialiasing filter name
char *SynCamera::GetCurrFilterName()
{
  return BartlettParam[ss_res].pname;
}

// Get current antialiasing filter name
char *SynCamera::GetCurrFilterSize()
{
  return BartlettParam[ss_res].psize;
}

// Get antialiasing filter name
char *SynCamera::GetFilterSize( int i )
{
  return BartlettParam[i].psize;
}

// Get number of antialiasing filters
int SynCamera::GetNumFilters()
{
  return (sizeof(BartlettParam) / sizeof(BartParam));
}

// Open antialiasing buffer
BOOL SynCamera::OpenBuffer( WinBitmap *pbmap )
{
  int row, col;         // Loop indices
  ColorRGB *ppixel;     // Pixel pointer

  // Get bitmap dimensions
  height = pbmap->GetHeight();
  width = pbmap->GetWidth();

  // Instantiate Bartlett filter
  if (InstBartlettFilter() == FALSE)
    return FALSE;

  // Allocate subpixel bitmap buffer
  if ((pabuf = new (ColorRGB (*[height]))) != NULL)
  {
    for (row = 0; row < height; row++)
    {
      if ((pabuf[row] = new ColorRGB[width]) == NULL)
      {
        // Release partially allocated buffer
        row--;
        for ( ; row >= 0; row--)
          delete [] pabuf[row];
        delete [] pabuf;

        delete [] pbart;    // Release Bartlett filter
        return FALSE;
      }
    }

    // Clear antialiasing buffer
    for (row = 0; row < height; row++)
      for (col = 0; col < width; col++)
      {
        ppixel = &(pabuf[row][col]);

        ppixel->SetRed((BYTE) 0);
        ppixel->SetGreen((BYTE) 0);
        ppixel->SetBlue((BYTE) 0);
      }

    return TRUE;
  }
  else
  {
    delete [] pbart;    // Release Bartlett filter
    return FALSE;
  }
}

// Close antialiasing buffer
void SynCamera::CloseBuffer()
{
  int row;  // Loop index
  
  delete [] pbart;  // Release Bartlett filter

  // Release subpixel bitmap buffer
  for (row = 0; row < height; row++)
    delete [] pabuf[row];
  delete [] pabuf;
}

// Render polygons
void SynCamera::RenderPolygons( Environ *penv, int width,
    double x_offset, double y_offset )
{
  Element3 *pelem;      // Element pointer
  Instance *pinst;      // Instance pointer
  OutPolygon out;       // Output polygon
  Patch3 *ppatch;       // Patch pointer
  Surface3 *psurf;      // Surface pointer

  // Walk the instance list
  pinst = penv->GetInstPtr();
  while (pinst != NULL)
  {
    // Walk the surface list
    psurf = pinst->GetSurfPtr();
    while (psurf != NULL)
    {
      // Walk the patch list
      ppatch = psurf->GetPatchPtr();
      while (ppatch != NULL)
      {
        // Determine patch visibility
        if (BackFaceCull(ppatch) == FALSE)
        {
          // Walk the element list
          pelem = ppatch->GetElementPtr();
          while (pelem != NULL)
          {
            // Clip the 3-D polygon
            (void) clipper.Clip(pelem, out,
                GetProjMatrix());

            // Render the 2-D polygon
            renderer.Render(out, width, x_offset, y_offset);

            pelem = pelem->GetNext();
          }
        }
        ppatch = ppatch->GetNext();
      }
      psurf = psurf->GetNext();
    }
    pinst = pinst->GetNext();
  }
}

// Instantiate Bartlett filter
BOOL SynCamera::InstBartlettFilter()
{
  int i, j, m, n;       // Loop indices
  int sum = 0;          // Sum of previous subpixel weights
  int array_width;      // Array width
  double offset;        // Subpixel offset
  Bartlett *pbfe;       // Bartlett filter element pointer
  Bartlett *psbfe;      // Second filter element pointer

  // Get Bartlett filter parameters
  offset = BartlettParam[ss_res].offset;
  array_width = BartlettParam[ss_res].width;

  // Allocate blending info array
  num_sub = array_width * array_width;
  if ((pbart = new Bartlett[num_sub]) == NULL)
    return FALSE;

  // Calculate offsets and weights
  for (i = 0; i <= array_width / 2; i++)
  {
    m = array_width - i - 1;
    {
      for (j = 0; j <= array_width / 2; j++)
      {
        n = array_width - j - 1;
        pbfe = &(pbart[i * array_width + j]);

        pbfe->x_offset = (offset * (double) (j + 1) - 1.0);
        pbfe->y_offset = 1.0 - (offset * (double) (i + 1));
        pbfe->weight = (i + 1) * (j + 1);

        if (j != n)
        {
          psbfe = &(pbart[i * array_width + n]);
          psbfe->x_offset = -pbfe->x_offset;
          psbfe->y_offset = pbfe->y_offset;
          psbfe->weight = pbfe->weight;
        }

        if (i != m)
        {
          psbfe = &(pbart[m * array_width + j]);
          psbfe->x_offset = pbfe->x_offset;
          psbfe->y_offset = -pbfe->y_offset;
          psbfe->weight = pbfe->weight;

          if (j != n)
          {
            psbfe = &(pbart[m * array_width + n]);
            psbfe->x_offset = -pbfe->x_offset;
            psbfe->y_offset = -pbfe->y_offset;
            psbfe->weight = pbfe->weight;
          }
        }
      }
    }
  }

  // Calculate blending info
  for (i = 0; i < num_sub; i++)
  {
    pbfe = &(pbart[i]);
    sum += pbfe->weight;
    pbfe->alpha = (pbfe->weight * 256) / sum;
    pbfe->beta = 256 - pbfe->alpha;
  }

  return TRUE;
}

