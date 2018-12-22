/************************************************************************/
/*                                                                      */
/*                       Soft Body 3.0                                  */
/*                 by Maciej Matyka, 21.IX.2004                         */
/*                                                                      */
/*  http://panoramix.ift.uni.wroc.pl/~maq                               */
/*                                                                      */
/*  Made for purposes of Grame Programming Gems 5 article.              */
/*                                                                      */
/*  note: this code has been attached to article  only as               */
/*  an example of described method. No any guarrantee of anything is    */
/*  is given. Use at your own risk.                                     */
/*                                                                      */
/*  if you want to use any part of that code in your project,           */
/*  email Author at:                                                    */
/*                                                                      */
/*  maq@panoramix.ift.uni.wroc.pl                                       */
/*  http://panoramix.ift.uni.wroc.pl/~maq                               */
/************************************************************************/


#include "bitmap.h"

void *
LoadDIBitmap(char	*filename,	/* I - File to load */
             BITMAPINFO	**info)		/* O - Bitmap information */
{
  FILE			*fp;		/* Open file pointer */
  void			*bits;		/* Bitmap pixel bits */
  long			bitsize,	/* Size of bitmap */
			infosize;	/* Size of header information */
  BITMAPFILEHEADER	header;		/* File header */
 
  if ((fp = fopen(filename, "rb")) == NULL)
    return (NULL);

  if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1)
  {
    fclose(fp);
    return (NULL);
  }


  if (header.bfType != 'MB')	/* Check for BM reversed... */
  {
    fclose(fp);
    return (NULL);
  }

  infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
  if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL)
  {
 
    fclose(fp);
    return (NULL);
  }

  if (fread(*info, 1, infosize, fp) < infosize)
  {
 
    free(*info);
    fclose(fp);
    return (NULL);
  }

  if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
    bitsize = (*info)->bmiHeader.biWidth * 3 *
  	      abs((*info)->bmiHeader.biHeight);

  if ((bits = malloc(bitsize)) == NULL)
  {
 
    free(*info);
    fclose(fp);
    return (NULL);
  }

  if (fread(bits, 1, bitsize, fp) < bitsize)
  {
    free(*info);
    free(bits);
    fclose(fp);
    return (NULL);
  }


  fclose(fp);
  return (bits);
}

int
SaveDIBitmap(char       *filename,	/* I - File to save to */
	     BITMAPINFO *info,		/* I - Bitmap information */
             void       *bits)		/* I - Bitmap pixel bits */
{
  FILE			*fp;		/* Open file pointer */
  long			size,		/* Size of file */
			infosize,	/* Size of bitmap info */
			bitsize;	/* Size of bitmap pixels */
  BITMAPFILEHEADER	header;		/* File header */

  if ((fp = fopen(filename, "wb")) == NULL)
    return (-1);

  if (info->bmiHeader.biSizeImage == 0)	/* Figure out the bitmap size */
    bitsize = (info->bmiHeader.biWidth *
               info->bmiHeader.biBitCount + 7) / 8 *
	      abs(info->bmiHeader.biHeight);
  else
    bitsize = info->bmiHeader.biSizeImage;

  infosize = sizeof(BITMAPINFOHEADER);
  switch (info->bmiHeader.biCompression)
  {
    case BI_BITFIELDS :
        infosize += 12;			/* Add 3 RGB doubleword masks */
        if (info->bmiHeader.biClrUsed == 0)
	  break;
    case BI_RGB :
        if (info->bmiHeader.biBitCount > 8 &&
            info->bmiHeader.biClrUsed == 0)
	  break;
    case BI_RLE8 :
    case BI_RLE4 :
        if (info->bmiHeader.biClrUsed == 0)
          infosize += (1 << info->bmiHeader.biBitCount) * 4;
	else
          infosize += info->bmiHeader.biClrUsed * 4;
	break;
  };

  size = sizeof(BITMAPFILEHEADER) + infosize + bitsize;

 /*
  * Write the file header, bitmap information, and bitmap pixel data...
  */

  header.bfType      = 'MB';		/* Non-portable... sigh */
  header.bfSize      = size;
  header.bfReserved1 = 0;
  header.bfReserved2 = 0;
  header.bfOffBits   = sizeof(BITMAPFILEHEADER) + infosize;

  if (fwrite(&header, 1, sizeof(BITMAPFILEHEADER), fp) < sizeof(BITMAPFILEHEADER))
  {

    fclose(fp);
    return (-1);
  };

  if (fwrite(info, 1, infosize, fp) < infosize)
  {
    fclose(fp);
    return (-1);
  };

  if (fwrite(bits, 1, bitsize, fp) < bitsize)
  {

    fclose(fp);
    return (-1);
  };

  fclose(fp);
  return (0);
}

void *
ReadDIBitmap(BITMAPINFO **info)		/* O - Bitmap information */
{
  long		i, j,			/* Looping var */
  		bitsize,		/* Total size of bitmap */
		width;			/* Aligned width of a scanline */
  GLint		viewport[4];		/* Current viewport */
  void		*bits;			/* RGB bits */
  GLubyte	*rgb,			/* RGB looping var */
		temp;			/* Temporary var for swapping */

  glGetIntegerv(GL_VIEWPORT, viewport);

  if ((*info = (BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER))) == NULL)
  {

    return (NULL);
  };

  width   = viewport[2] * 3;		/* Real width of scanline */
  width   = (width + 3) & ~3;		/* Aligned to 4 bytes */
  bitsize = width * viewport[3];	/* Size of bitmap, aligned */

  if ((bits = calloc(bitsize, 1)) == NULL)
  {

    free(*info);
    return (NULL);
  };

  glFinish();				/* Finish all OpenGL commands */
  glPixelStorei(GL_PACK_ALIGNMENT, 4);	/* Force 4-byte alignment */
  glPixelStorei(GL_PACK_ROW_LENGTH, 0);
  glPixelStorei(GL_PACK_SKIP_ROWS, 0);
  glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

  glReadPixels(0, 0, viewport[2], viewport[3], GL_RGB, GL_UNSIGNED_BYTE,
               bits);

  for (i = 0; i < viewport[3]; i ++)
    for (j = 0, rgb = ((GLubyte *)bits) + i * width;
         j < viewport[2];
	 j ++, rgb += 3)
    {
      temp   = rgb[0];
      rgb[0] = rgb[2];
      rgb[2] = temp;
    };

  (*info)->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
  (*info)->bmiHeader.biWidth         = viewport[2];
  (*info)->bmiHeader.biHeight        = viewport[3];
  (*info)->bmiHeader.biPlanes        = 1;
  (*info)->bmiHeader.biBitCount      = 24;
  (*info)->bmiHeader.biCompression   = BI_RGB;
  (*info)->bmiHeader.biSizeImage     = bitsize;
  (*info)->bmiHeader.biXPelsPerMeter = 2952; /* 75 DPI */
  (*info)->bmiHeader.biYPelsPerMeter = 2952; /* 75 DPI */
  (*info)->bmiHeader.biClrUsed       = 0;
  (*info)->bmiHeader.biClrImportant  = 0;

  return (bits);
}


GLubyte *
ConvertRGB(BITMAPINFO *info,		/* I - Original bitmap information */
           void       *bits)		/* I - Original bitmap pixels */
{
  int		i, j,			/* Looping vars */
  		bitsize,		/* Total size of bitmap */
		width;			/* Aligned width of bitmap */
  GLubyte	*newbits;		/* New RGB bits */
  GLubyte	*from, *to;		/* RGB looping vars */

  width   = 3 * info->bmiHeader.biWidth;
  width   = (width + 3) & ~3;	
  bitsize = width * info->bmiHeader.biHeight;
  if ((newbits = (GLubyte *)calloc(bitsize, 1)) == NULL)
    return (NULL);

  switch (info->bmiHeader.biCompression)
  {
    case BI_RGB :
        if (info->bmiHeader.biBitCount == 24)
	{
 
          for (i = 0; i < info->bmiHeader.biHeight; i ++)
	    for (j = 0, from = ((GLubyte *)bits) + i * width,
	             to = newbits + i * width;
		 j < info->bmiHeader.biWidth;
		 j ++, from += 3, to += 3)
            {
              to[0] = from[2];
              to[1] = from[1];
              to[2] = from[0];
            };
	};
	break;
    case BI_RLE4 :
    case BI_RLE8 :
    case BI_BITFIELDS :
        break;
  };

  return (newbits);
}

// 8.1.4....8