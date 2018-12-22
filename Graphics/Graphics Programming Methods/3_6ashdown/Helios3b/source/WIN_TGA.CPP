////////////////////////////////////////////////////////////
//
//  WIN_TGA.CPP - MS-Windows TARGA Bitmap Class
//
//  Version:    1.03B
//
//  History:    94/12/17 - Created.
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

#include "win_tga.h"

// Write TARGA-format file
BOOL WinTarga::Write( BYTE __huge *pbm, ColorRGB *ppal,
    int width, int height, int scan_width, int num_colors,
    char *fname )
{
  int i;                        // Loop index
  BYTE blue;                    // Blue color value
  BYTE green;                   // Green color value
  BYTE red;                     // Red color value
  BYTE __huge *pbuff = pbm;     // Buffer pointer
  HFILE hfile;                  // File handle
  WORD num_bytes;               // Number of scan line bytes

  // Check for existing bitmap
  if (pbm == NULL)
    return FALSE;

  // Initialize file header
  if (num_colors == 0)
  {
    // No palette
    header.cmap_type = (BYTE) 0;
    header.image_type = (BYTE) 2;
    header.cmap_num = (BYTE) 0;
    header.cmap_size = (BYTE) 0;
    header.bpp = (BYTE) 24;
  }
  else
  {
    // Palette
    header.cmap_type = (BYTE) 1;
    header.image_type = (BYTE) 1;
    header.cmap_size = (BYTE) 24;
    header.cmap_num = (BYTE) num_colors;
    header.bpp = (BYTE) 8;
  }

  header.width = (WORD) width;
  header.height = (WORD) height;

  // Open the file
  if ((hfile = _lcreat(fname, 0)) == HFILE_ERROR)
    return FALSE;

  // Write the file header (member-by-member to avoid
  // structure alignment problems with Win32)
  _lwrite(hfile, (LPSTR) &(header.id_len),
      sizeof(header.id_len));
  _lwrite(hfile, (LPSTR) &(header.cmap_type),
      sizeof(header.cmap_type));
  _lwrite(hfile, (LPSTR) &(header.image_type),
      sizeof(header.image_type));
  _lwrite(hfile, (LPSTR) &(header.cmap_start),
      sizeof(header.cmap_start));
  _lwrite(hfile, (LPSTR) &(header.cmap_num),
      sizeof(header.cmap_num));
  _lwrite(hfile, (LPSTR) &(header.cmap_size),
      sizeof(header.cmap_size));
  _lwrite(hfile, (LPSTR) &(header.horz_org),
      sizeof(header.horz_org));
  _lwrite(hfile, (LPSTR) &(header.vert_org),
      sizeof(header.vert_org));
  _lwrite(hfile, (LPSTR) &(header.width),
      sizeof(header.width));
  _lwrite(hfile, (LPSTR) &(header.height),
      sizeof(header.height));
  _lwrite(hfile, (LPSTR) &(header.bpp),
      sizeof(header.bpp));
  _lwrite(hfile, (LPSTR) &(header.desc),
      sizeof(header.desc));

  if (num_colors > 0)
  {
    num_bytes = (WORD) width;

    // Write palette
    for (i = 0; i < num_colors; i++)
    {
      // Get palette entry color components
      blue = ppal[i].GetBlue();
      green = ppal[i].GetGreen();
      red = ppal[i].GetRed();

      // Write palette entry (NOTE REVERSED ORDER!)
      _lwrite(hfile, (LPSTR) &blue, sizeof(BYTE));
      _lwrite(hfile, (LPSTR) &green, sizeof(BYTE));
      _lwrite(hfile, (LPSTR) &red, sizeof(BYTE));
    }
  }
  else
    num_bytes = (WORD) width * 3;

  // Write bitmap to file
  //
  // NOTES:  1. TARGA file format stores RGB triples in
  //            reverse order (same as BMP file format).
  //         2. TARGA file format does NOT pad scan lines
  //            to DWORD boundaries.
  //
  for (i = 0; i < height; i++)
  {
    // Write scan line
    if (_lwrite(hfile, (LPSTR) pbuff, num_bytes) !=
        num_bytes)
      return FALSE;

    // Advance buffer pointer to next scan line
    pbuff += scan_width;
  }

  _lclose(hfile);       // Close the file

  return TRUE;
}

