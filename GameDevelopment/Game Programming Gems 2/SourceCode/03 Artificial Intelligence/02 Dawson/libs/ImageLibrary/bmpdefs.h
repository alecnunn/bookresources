/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
#ifndef	BMPDEFS_H
#define	BMPDEFS_H

// These structures are defined here to make the BMP loading and saving code
// more portable.

#pragma pack(push, 1)

#ifdef	RENAME_WINDOWS_STRUCTS
struct PortableBITMAPFILEHEADER {
#else
struct BITMAPFILEHEADER {
#endif
	uint16_t   bfType;
	uint32_t   bfSize;
	uint16_t   bfReserved1;
	uint16_t   bfReserved2;
	uint32_t   bfOffBits;
};

#ifdef	RENAME_WINDOWS_STRUCTS
struct PortableBITMAPINFOHEADER
#else
struct BITMAPINFOHEADER
#endif
{
	uint32_t      biSize;
	int32_t       biWidth;
	int32_t       biHeight;
	uint16_t      biPlanes;
	uint16_t      biBitCount;
	uint32_t      biCompression;
	uint32_t      biSizeImage;
	int32_t       biXPelsPerMeter;
	int32_t       biYPelsPerMeter;
	uint32_t      biClrUsed;
	uint32_t      biClrImportant;
};

#define BI_RGB        0L
#define BI_RLE8       1L

#pragma pack(pop)

#endif
