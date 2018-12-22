////////////////////////////////////////////////////////////
//
//  WIN_BMAP.CPP - MS-Windows Bitmap Class
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/12/02 - General rewrite.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/03/23 - Added FlushBitmap function.
//                       - Added string.h include directive.
//              95/06/22 - Added rgb_flag and hpal checks to
//                         Redisplay function.
//              95/06/24 - Deleted GetPixel, SetPalPixel and
//                         SetPixel functions.
//              95/07/21 - Version 1.02B release.
//              96/02/14 - Version 1.02C release.
//              96/04/01 - Version 1.03A release.
//              96/11/23 - Added WORD typecast in num_colors
//                         assignment in AllocPallette.
//              97/03/11 - Corrected calculations of
//                         bmOffBits member of DIB file
//                         header in InitDIB.
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

#include <string.h>
#include "win_bmap.h"

// Open bitmap
BOOL WinBitmap::Open( int w, int h )
{
  FreeBitmap();     // Release current bitmap (if any)
  FreePalette();    // Release logical palette (if any)

  width = w;
  height = h;

  // Bitmap width must be multiple of DWORD (4 bytes) to
  // avoid segmentation arithmetic problems with __huge
  // pointers on 80x86 CPU
  pal_scan = (DWORD) BM_WidthBytes(width);
  rgb_scan = (DWORD) BM_WidthBytes(width * BM_RGB_BPP);

  // Calculate bitmap size
  pal_size = pal_scan * (DWORD) height;
  rgb_size = rgb_scan * (DWORD) height;

  if (AllocBitmap() == FALSE)   // Allocate bitmap
  {
    width = height = 0;
    pal_scan = rgb_scan = (DWORD) 0L;
    pal_size = rgb_size = (DWORD) 0L;

    return FALSE;
  }

  InitDIB();    // Initialize DIB palette

  return TRUE;
}

// Perform color quantization
BOOL WinBitmap::QuantizeColors()
{
  if (Quantize() == FALSE)      // Quantize bitmap colors
    return FALSE;

  if (AllocPalette() == FALSE)  // Create logical palette
    return FALSE;

  InitDIB();    // Reinitialize DIB palette
  
  return TRUE;
}

// Display device-independent bitmap (DIB)
BOOL WinBitmap::Display( HDC hdc, POINT &pos, RECT &rect )
{
  int i;                // Loop index
  WORD *ppal_index;     // Palette-relative index pointer

  if (hddb == NULL)
  {
    // Convert DIB to device-dependent bitmap
    if (rgb_flag == FALSE)
    {
      // Select logical palette into device context
      SelectPalette(hdc, hpal, FALSE);

      // Realize logical palette
      RealizePalette(hdc);

      // Convert DIB color palette to logical palette-
      // relative indices
      ppal_index = (WORD *) bm_colors;
      for (i = 0; i < num_colors; i++)
        *ppal_index++ = (WORD) i;

      // Display palette-mapped bitmap
      if ((hddb = CreateDIBitmap(hdc, &(bm_iheader),
          CBM_INIT, (LPSTR) pbm, (BITMAPINFO *) &bm_iheader,
          DIB_PAL_COLORS)) == NULL)
        return FALSE;
    }
    else
    {
      // Display 24-bit RGB bitmap
      if ((hddb = CreateDIBitmap(hdc, &(bm_iheader),
          CBM_INIT, (LPSTR) pbm, (BITMAPINFO *) &bm_iheader,
          DIB_RGB_COLORS)) == NULL)
        return FALSE;
    }
  }

  // Display device-dependent bitmap
  return DisplayDDB(hdc, pos, rect);
}

// Repaint client area
void WinBitmap::Redisplay( HWND hwnd )
{
  HDC hdc;              // Device context
  HPALETTE holdpal;     // Previous palette handle

  if (rgb_flag == FALSE)        // 256-color display ?
  {
    if (hpal != NULL)           // Valid bitmap palette ?
    {
      hdc = GetDC(hwnd);        // Get device context handle

      // Select logical palette into device context
      holdpal = SelectPalette(hdc, hpal, FALSE);

      RealizePalette(hdc);      // Update system palette

      // Restore previous logical palette
      SelectPalette(hdc, holdpal, FALSE);

      ReleaseDC(hwnd, hdc);     // Release device context

      // Repaint client area
      InvalidateRect(hwnd, NULL, TRUE);
    }
  }
}

BOOL WinBitmap::Write( char *fname )
{
  HFILE hfile;  // File handle

  // Check for existing bitmap
  if (pbm == NULL)
    return FALSE;

  // Open the file
  if ((hfile = _lcreat(fname, 0)) == HFILE_ERROR)
    return FALSE;

  // Write the file header (member-by-member to avoid
  // structure alignment problems with Win32)
  _lwrite(hfile, (LPSTR) &(bm_file.bfType),
      sizeof(bm_file.bfType));
  _lwrite(hfile, (LPSTR) &(bm_file.bfSize),
      sizeof(bm_file.bfSize));
  _lwrite(hfile, (LPSTR) &(bm_file.bfReserved1),
      sizeof(bm_file.bfReserved1));
  _lwrite(hfile, (LPSTR) &(bm_file.bfReserved2),
      sizeof(bm_file.bfReserved2));
  _lwrite(hfile, (LPSTR) &(bm_file.bfOffBits),
      sizeof(bm_file.bfOffBits));

  // Write the information header
  _lwrite(hfile, (LPSTR) &(bm_iheader),
      sizeof(BITMAPINFOHEADER));

  if (rgb_flag == FALSE)
  {
    SetDIBPalette();    // Initialize the DIB palette

    // Write the DIB palette
    _lwrite(hfile, (LPSTR) bm_colors, sizeof(RGBQUAD) *
        num_colors);

    // Write the palette-mapped bitmap
    WriteBitmap(hfile, pal_size);
  }
  else
  {
    // Write the RGB bitmap
    WriteBitmap(hfile, rgb_size);
  }

  _lclose(hfile);       // Close the file

  return TRUE;
}

void WinBitmap::Close()     // Close bitmap
{
  FreeBitmap();     // Release bitmap
  FreePalette();    // Release logical palette

  width = height = 0;
  pal_scan = rgb_scan = (DWORD) 0L;
  pal_size = rgb_size = (DWORD) 0L;
}

// Initialize DIB data
void WinBitmap::InitDIB()
{
  // Initialize DIB file header
  bm_file.bfOffBits = (DWORD) (sizeof(BITMAPFILEHEADER)
      + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) *
      num_colors);
  if (rgb_flag == TRUE)
    bm_file.bfSize = (DWORD) (bm_file.bfOffBits + rgb_size);
  else
    bm_file.bfSize = (DWORD) (bm_file.bfOffBits + pal_size);

  // Initialize DIB information header
  bm_iheader.biWidth = width;
  bm_iheader.biHeight = height;
  bm_iheader.biClrUsed = num_colors;
  bm_iheader.biClrImportant = num_colors;

  if (rgb_flag == TRUE)
    bm_iheader.biBitCount = 24;
  else
    bm_iheader.biBitCount = 8;
}

// Display device-dependent bitmap (DDB)
BOOL WinBitmap::DisplayDDB( HDC hdc, POINT &pos, RECT
    &rect )
{
  BOOL status = FALSE;  // Return status
  HBITMAP holddb;       // Previous DDB bitmap handle
  HDC hmemdc;           // Memory device context handle
  
  // Create memory device context
  if ((hmemdc = CreateCompatibleDC(hdc)) != NULL)
  {
    // Select bitmap
    if ((holddb = (HBITMAP) SelectObject(hmemdc, hddb)) !=
        NULL)
    {
      // Copy bitmap from memory to display device
      BitBlt(hdc, rect.left, rect.top, rect.right,
          rect.bottom, hmemdc, pos.x, pos.y, SRCCOPY);

      // Select previous bitmap
      SelectObject(hmemdc, holddb);

      status = TRUE;
    }

    // Delete memory device context
    DeleteDC(hmemdc);
  }

  return status;
}

// Set device-independent bitmap (DIB) palette
void WinBitmap::SetDIBPalette()
{
  int i;    // Loop index

  for (i = 0; i < num_colors; i++)
  {
    bm_colors[i].rgbRed = palette[i].GetRed();
    bm_colors[i].rgbGreen = palette[i].GetGreen();
    bm_colors[i].rgbBlue = palette[i].GetBlue();
    bm_colors[i].rgbReserved = (BYTE) 0;
  }
}

// Write bitmap to file
BOOL WinBitmap::WriteBitmap( int hfile, DWORD size )
{
  DWORD remain = size;          // Bytes remaining
  BYTE __huge *pbuff = pbm;     // Buffer pointer

  // Write buffer to file in blocks
  while (remain > (DWORD) BM_BlockSize)
  {
    if (_lwrite(hfile, (LPSTR) pbuff, BM_BlockSize) !=
        BM_BlockSize)
      return FALSE;
    remain -= BM_BlockSize;
    pbuff += BM_BlockSize;
  }

  // Write last block to file
  if ((DWORD) _lwrite(hfile, (LPSTR) pbuff, (WORD) remain)
      == remain)
    return TRUE;
  else
    return FALSE;
}

void WinBitmap::FlushBitmap()   // Clear bitmap
{
  int i;                // Loop index
  BYTE __huge *prow;    // Row pointer

  // Initialize row pointer
  prow = pbm;

  for (i = 0; i < height; i++)
  {
    memset(prow, '\0', (size_t) rgb_scan);
    prow += rgb_scan;
  }
}

BOOL WinBitmap::AllocBitmap()   // Allocate bitmap
{
  // Allocate global memory for bitmap
  if ((hdib = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
      rgb_size)) != NULL)
  {
    // Lock bitmap memory
    pbm = (BYTE __huge *) GlobalLock(hdib);
    return TRUE;
  }
  else
    return FALSE;
}

BOOL WinBitmap::AllocPalette()  // Allocate logical palette
{
  int i;                // Loop index
  LOGPALETTE *ppal;     // Logical palette pointer

  // Allocate palette
  if ((ppal = (LOGPALETTE *) new BYTE[sizeof(LOGPALETTE)
      + num_colors * sizeof(PALETTEENTRY)]) == NULL)
    return FALSE;

  // Initialize palette information
  ppal->palVersion = 0x300;
  ppal->palNumEntries = (WORD) num_colors;

  // Copy DIB palette
  for (i = 0; i < num_colors; i++)
  {
    ppal->palPalEntry[i].peRed = palette[i].GetRed();
    ppal->palPalEntry[i].peGreen = palette[i].GetGreen();
    ppal->palPalEntry[i].peBlue = palette[i].GetBlue();
    ppal->palPalEntry[i].peFlags = (BYTE) 0;
  }

  // Create logical color palette
  hpal = CreatePalette(ppal);

  delete [] ppal;       // Release palette memory

  return TRUE;
}

void WinBitmap::FreeBitmap()    // Release bitmap
{
  if (hdib != NULL)     // Release DIB memory
  {
    GlobalUnlock(hdib);
    GlobalFree(hdib);
    pbm = NULL;
    hdib = NULL;
  }

  if (hddb != NULL)     // Release DDB memory
  {
    DeleteObject(hddb);
    hddb = NULL;
  }
}

void WinBitmap::FreePalette()   // Release logical palette
{
  if (hpal != NULL)
  {
    DeleteObject(hpal);
    hpal = NULL;
  }
}

