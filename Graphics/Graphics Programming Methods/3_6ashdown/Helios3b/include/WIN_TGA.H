////////////////////////////////////////////////////////////
//
//  WIN_TGA.H - MS-Windows TARGA Bitmap Class Include File
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

#ifndef _WIN_TGA_H
#define _WIN_TGA_H

#include "color.h"

struct TargaHeader      // TARGA file header
{
  BYTE id_len;          // Identifier field length
  BYTE cmap_type;       // Color map type
  BYTE image_type;      // Image type
  WORD cmap_start;      // First color map entry index
  WORD cmap_num;        // Number of color map entries
  BYTE cmap_size;       // Color map entry size (in bits)
  WORD horz_org;        // Image horizontal origin
  WORD vert_org;        // Image vertical origin
  WORD width;           // Image width
  WORD height;          // Image height
  BYTE bpp;             // Bits per pixel
  BYTE desc;            // Image descriptor
};

class WinTarga
{
  private:
    TargaHeader header;     // File header

  public:
    WinTarga()
    {
      header.id_len = (BYTE) 0;
      header.cmap_start = (WORD) 0;
      header.horz_org = (WORD) 0;
      header.vert_org = (WORD) 0;
      header.desc = (BYTE) 0;
    }

    ~WinTarga() { }

    BOOL Write( BYTE __huge *, ColorRGB *, int, int, int,
        int, char * );
};

#endif

