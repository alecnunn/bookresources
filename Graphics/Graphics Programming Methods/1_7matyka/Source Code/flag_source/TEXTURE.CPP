#include "texture.h"

int TextureLoadBitmap(char *filename)		/* I - Bitmap file to load */
{
  BITMAPINFO	*info;				/* Bitmap information */
  void		*bits;				/* Bitmap pixel bits */
  GLubyte	*rgb;				/* Bitmap RGB pixels */

  bits = LoadDIBitmap(filename, &info);
  if (bits == NULL)
    return (-1);

  rgb = ConvertRGB(info, bits);
  if (rgb == NULL)
  {
    free(info);
    free(bits);

    return (-1);
  };

  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);	/* Force 4-byte alignment */
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

  if (info->bmiHeader.biHeight == 1)
  {
    glTexImage1D(GL_TEXTURE_1D, 0, 3, info->bmiHeader.biWidth, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }
  else if (info->bmiHeader.biWidth == 1)
  {
    glTexImage1D(GL_TEXTURE_1D, 0, 3, info->bmiHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }
  else
  {
    glTexImage2D(GL_TEXTURE_2D, 0, 3, info->bmiHeader.biWidth, info->bmiHeader.biHeight, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, rgb);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  };


  free(rgb);
  free(info);
  free(bits);

  return (0);
}

int
TextureLoadMipmap(char *filename)		/* I - Bitmap file to load */
{
  BITMAPINFO	*info;				/* Bitmap information */
  void		*bits;				/* Bitmap pixel bits */
  GLubyte	*rgb;				/* Bitmap RGB pixels */

  bits = LoadDIBitmap(filename, &info);
  if (bits == NULL)
    return (-1);

  rgb = ConvertRGB(info, bits);
  if (rgb == NULL)
  {
    free(info);
    free(bits);

    return (-1);
  };

  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);	/* Force 4-byte alignment */
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

  if (info->bmiHeader.biHeight == 1)
  {
    gluBuild1DMipmaps(GL_TEXTURE_1D, 3, info->bmiHeader.biWidth, GL_RGB, GL_UNSIGNED_BYTE, rgb);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  }
  else if (info->bmiHeader.biWidth == 1)
  {
    gluBuild1DMipmaps(GL_TEXTURE_1D, 3, info->bmiHeader.biHeight, GL_RGB, GL_UNSIGNED_BYTE, rgb);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  }
  else
  {
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, info->bmiHeader.biWidth, info->bmiHeader.biHeight,
                      GL_RGB, GL_UNSIGNED_BYTE, rgb);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  };

  free(rgb);
  free(info);
  free(bits);

  return (0);
}

// komorka // 8148