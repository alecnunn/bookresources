////////////////////////////////////////////////////////////
//
//  WIN_BMAP.H - MS-Windows Bitmap Class Include File
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/12/02 - General rewrite.
//              94/12/16 - Version 1.01A release.
//              94/12/17 - Deleted __huge conditional
//                         directive.
//              94/12/19 - Merged GetPalScanWidth and
//                         GetRGBScanWidth functions into
//                         GetScanWidth.
//              95/02/05 - Version 1.02A release.
//              95/03/21 - Made GetPixel function public.
//              95/03/23 - Added FlushBitmap function
//                         prototype.
//              95/06/24 - Made GetPixel, SetPalPixel and
//                         SetPixel functions inline.
//              95/06/30 - Added ppix data member.
//                       - Added IncPixelPtr, SetPixelPtr
//                         and SetCurrPixel functions.
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

#ifndef _WIN_BMAP_H
#define _WIN_BMAP_H

#include "oct_quan.h"

// Round number upwards to next multiple of four
#define BM_WidthBytes(i) (((i + 3) / 4) * 4)

// Number of bytes per pixel (24-bit RGB)
static const int BM_RGB_BPP = 3;

// File read/write block size
static const WORD BM_BlockSize = 0x8000;

// Number of palette elements
//
// NOTE: MS-Windows reserves 20 static colors for its
//       default palette. Limiting the BMP file color
//       palette to 236 colors ensures that these colors
//       will not be affected when the BMP file is
//       displayed.

static const int BM_PaletteSize = 236;

class WinBitmap : public OctQuant       // MS-Windows bitmap
{
  private:
    BITMAPFILEHEADER bm_file;   // DIB file header

    // NOTE: the following data structures MUST be
    //       contiguous and aligned on WORD boundaries

#pragma pack(2)     // Enable WORD alignment
    // DIB information header pointer
    BITMAPINFOHEADER bm_iheader;

    // DIB bitmap palette (for palette-mapped DIBs only)
    RGBQUAD bm_colors[BM_PaletteSize];
#pragma pack()      // Revert to default compiler alignment

    BOOL pal_flag;      // Display palette flag
    BOOL rgb_flag;      // 24-bit RGB color flag
    BYTE __huge *pbm;   // Bitmap pointer
    BYTE __huge *ppix;  // Pixel pointer
    DWORD pal_scan;     // Palette-mapped scanline width
    DWORD pal_size;     // Palette-mapped bitmap size
    DWORD rgb_scan;     // RGB scanline width
    DWORD rgb_size;     // RGB bitmap size
    HBITMAP hddb;       // DDB bitmap handle
    HGLOBAL hdib;       // DIB bitmap handle
    HPALETTE hpal;      // Logical palette handle
                            
    BOOL AllocBitmap();
    BOOL AllocPalette();
    BOOL DisplayDDB( HDC, POINT &, RECT & );
    BOOL WriteBitmap( int, DWORD );
    void FreeBitmap();
    void InitDIB();
    void SetDIBPalette();

    // Write 256-color pixel to bitmap
    void SetPalPixel( int x, int y, BYTE &c )
    {
      BYTE __huge *ppixel;  // Pixel pointer

      // Get pixel pointer
      ppixel = pbm + (y * pal_scan) + x;

      *ppixel = c;  // Set pixel color palette index
    }

  public:
    WinBitmap()
    {
      pbm = NULL;
      hddb = NULL;
      hdib = NULL;
      hpal = NULL;
      width = height = 0;
      pal_scan = rgb_scan = (DWORD) 0L;
      pal_size = rgb_size = (DWORD) 0L;
      max_colors = BM_PaletteSize;

      bm_file.bfType = 0x4d42;  // 'BM' signature
      bm_file.bfSize = 0L;
      bm_file.bfReserved1 = 0;
      bm_file.bfReserved2 = 0;

      bm_iheader.biSize = (DWORD) sizeof(BITMAPINFOHEADER);
      bm_iheader.biWidth = 0L;
      bm_iheader.biHeight = 0L;
      bm_iheader.biPlanes = 1;
      bm_iheader.biCompression = BI_RGB;
      bm_iheader.biSizeImage = 0L;
      bm_iheader.biXPelsPerMeter = 0L;
      bm_iheader.biYPelsPerMeter = 0L;

      pal_flag = FALSE;
      rgb_flag = TRUE;
    }

    ~WinBitmap()
    {
      FreeBitmap();
      FreePalette();
    }

    BOOL Display( HDC, POINT &, RECT & );
    BOOL GetPaletteFlag() { return pal_flag; }
    BOOL Open( int, int );
    BOOL QuantizeColors();
    BOOL Write( char * );
    BYTE __huge *GetBitmapPtr() { return pbm; }
    ColorRGB *GetPalettePtr() { return palette; }
    int GetHeight() { return height; }
    int GetRGBFlag() { return rgb_flag; }
    int GetScanWidth()
    {
      if (rgb_flag == TRUE)
        return (int) rgb_scan;
      else
        return (int) pal_scan;
    }
    int GetWidth() { return width; }
    void Close();
    void FlushBitmap();
    void FreePalette();

    // Read 24-bit RGB pixel from bitmap
    void GetPixel( int x, int y, ColorRGB *pc )
    {
      BYTE __huge *ppixel;  // Pixel pointer

      // Get pixel pointer
      ppixel = pbm + (y * rgb_scan) + (x * BM_RGB_BPP);

      // Get pixel colors (NOTE REVERSED ORDER!)
      pc->SetBlue(ppixel[0]);
      pc->SetGreen(ppixel[1]);
      pc->SetRed(ppixel[2]);
    }

    void IncPixelPtr() { ppix += BM_RGB_BPP; }
    void Redisplay( HWND );

    // Write current 24-bit RGB pixel to bitmap
    void SetCurrPixel( ColorRGB &c )
    {
      // Set pixel colors (NOTE REVERSED ORDER!)
      ppix[0] = c.GetBlue();
      ppix[1] = c.GetGreen();
      ppix[2] = c.GetRed();
    }

    void SetPaletteFlag( BOOL f ) { pal_flag = f; }

    // Write 24-bit RGB pixel to bitmap
    void SetPixel( int x, int y, ColorRGB &c )
    {
      BYTE __huge *ppixel;  // Pixel pointer

      // Get pixel pointer
      ppixel = pbm + (y * rgb_scan) + (x * BM_RGB_BPP);

      // Set pixel colors (NOTE REVERSED ORDER!)
      ppixel[0] = c.GetBlue();
      ppixel[1] = c.GetGreen();
      ppixel[2] = c.GetRed();
    }

    void SetPixelPtr( int x, int y )
    { ppix = pbm + (y * rgb_scan) + (x * BM_RGB_BPP); }
    void SetRGBFlag( int r ) { rgb_flag = r; }
};

#endif

