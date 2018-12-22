///////////////////////////////////////////////////////////////////////////////
//
// BMP.cpp : Bitmap BMP file loader and utilities
//
//	Copyright (c) Darwin 3D, LLC, 2003
//	For use in source accompanying the book Graphics Programming Methods
//
///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

static int GetColorCount(int nbits)
{
    int ncolors = 0;

    if (nbits < 24)
    {
 	    ncolors = 1 << nbits;
    }

    return(ncolors);
}


static void BMPLineNone(FILE *f, char *sline, int pixbytes, int width)
{
    int nbytes, i, k, j;

	switch (pixbytes)
	{
	    case 1 :
            nbytes = (width + 3) / 4;
            nbytes *= 4;

            fread(sline, width, 1, f);
            nbytes -= width;

            while (nbytes-- > 0) fgetc(f);
			return;

		case 3 :
			nbytes = ((width * 3) + 3) / 4;
			nbytes *= 4;

			fread(sline, width, 3, f);
			nbytes -= width * 3;

			while (nbytes-- > 0) fgetc(f);

			// reorder bgr to rgb
			for (i = 0, j = 0; i < width; i++, j += 3)
			{
				k = sline[j];
				sline[j] = sline[j+2];
				sline[j+2] = k;
			}

			return;
	}

	::MessageBox(NULL,"BMP Load Error","BMPLineNone failed.",MB_OK);
}


static void BMPLineRLE8(FILE *f, char *sline, int pixbytes, int width)
{
	::MessageBox(NULL,"BMP Load Error","RLE8 not yet supported.",MB_OK);
}


static void BMPLineRLE4(FILE *f, char *sline, int pixbytes, int width)
{
	::MessageBox(NULL,"BMP Load Error","RLE4 not yet supported.",MB_OK);
}


static void BMPLine(FILE *f, char *scanline, int pixbytes, int width, int rle)
{
    switch (rle)
    {
	    case xBI_NONE : BMPLineNone(f, scanline, pixbytes, width); return;
	    case xBI_RLE8 : BMPLineRLE8(f, scanline, pixbytes, width); return;
	    case xBI_RLE4 : BMPLineRLE4(f, scanline, pixbytes, width); return;
    }

	::MessageBox(NULL,"BMP Load Error","Unknown compression type.",MB_OK);
}

void LoadBMP(char *filename, bitmap_t *bit)
{
    FILE    *f;
    bmphd_t  bhd;
    binfo_t  info;
    int      pxlsize = 1;
    int      rowbytes, i, pixbytes;
    char    *scanline;

    // open file
    if ((f = fopen(filename, "rb")) == NULL)
    {
		::MessageBox(NULL,"BMP Load Error","Unable to open file.",MB_OK);
		return;
    }

    // read in bitmap header
    if (fread(&bhd, sizeof(bhd), 1, f) != 1)
    {
		fclose(f);
		::MessageBox(NULL,"BMP Load Error","Unable to read file.",MB_OK);
		return;
    }

    // make sure we have a valid bitmap file
    if (bhd.bfType != BMP_SIGNATURE_WORD)
    {
		fclose(f);
		::MessageBox(NULL,"BMP Load Error","Invalid BMP file.",MB_OK);
    }

    // load in info header
    if (fread(&info, sizeof(info), 1, f) != 1)
    {
		fclose(f);
		::MessageBox(NULL,"BMP Load Error","Unable to read bitmap info header.",MB_OK);
    }

    // make sure this is an info type of bitmap
    if (info.biSize != sizeof(binfo_t))
    {
		fclose(f);
		::MessageBox(NULL,"BMP Load Error","We only support the info bitmap type.",MB_OK);
    }

    // PrintHeader(&info);

	bit->bpp      = info.biBitCount;
	bit->width    = info.biWidth;
    bit->height   = info.biHeight;
    bit->data     = NULL;
    bit->palette  = NULL;

    //currently we only read in 8 and 24 bit bmp files
	if      (info.biBitCount == 8)  pixbytes = 1;
	else if (info.biBitCount == 24) pixbytes = 3;
	else
    {
		::MessageBox(NULL,"BMP Load Error","Bit depth not supported.",MB_OK);
    }

    // if this is an eight bit image load palette
	if (pixbytes == 1)
    {
	    drgb_t q;

	    bit->palette = (rgb_t *)malloc(sizeof(rgb_t) * 256);

	    for (i = 0; i < 256; i++)
	    {
	        if (fread(&q, sizeof(drgb_t), 1, f) != 1)
	        {
				fclose(f); free(bit->palette);
				::MessageBox(NULL,"BMP Load Error","Unable to read palette.",MB_OK);
			}

	        bit->palette[i].r   = q.red;
	        bit->palette[i].g   = q.green;
	        bit->palette[i].b   = q.blue;
		}
    }

    // position to start of bitmap
    fseek(f, bhd.bfOffBits, SEEK_SET);

    // create scanline to read data into
    rowbytes = ((info.biWidth * pixbytes) + 3) / 4;
    rowbytes *= 4;

    scanline = (char *)malloc(rowbytes);

    // alloc space for new bitmap
    bit->data = (unsigned char *)malloc(info.biWidth * pixbytes * info.biHeight);

    // read in image
    for (i = 0; i < info.biHeight; i++)
    {
	    BMPLine(f, scanline, pixbytes, info.biWidth, info.biCompression);

	    // store line
	    memcpy(&bit->data[info.biWidth * pixbytes * (info.biHeight - i - 1)], scanline, info.biWidth * pixbytes);
    }

    free(scanline);
    fclose(f);
}

void __cdecl FreeBMP(bitmap_t *bitmap)
{
    if (bitmap->palette)
    {
	    free(bitmap->palette);
	    bitmap->palette = NULL;
    }

    if (bitmap->data)
    {
	    free(bitmap->data);
	    bitmap->data = NULL;
    }
}
