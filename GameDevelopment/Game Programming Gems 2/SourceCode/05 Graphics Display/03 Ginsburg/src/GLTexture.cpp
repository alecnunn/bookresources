/* Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001"
 */
//
//
// GLTexture.cpp - OpenGL Texture Manager
//
//

#ifdef WIN32
#include <windows.h>
#endif

#ifdef MAC_OS
#include <Endian.h>
#endif

#include <stdio.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "glati.h"
#include "wglati.h"
#include "GLTexture.h"
#include "GLErr.h"


extern char    gszErrMsg[64];          // Global error message text


#ifdef WIN32
#define PRINTF wsprintf
#endif

#ifdef MAC_OS
#define PRINTF sprintf
#endif

ATI_GLTexture::ATI_GLTexture(char *filename, GLint name, GLboolean mipMap, GLenum format, GLint internalFormat)
{
   TGAHeaderInfo   TGAHeader;
   DWORD           imageSize;
   FILE            *fh;
   DWORD           bytesRead;
   GLubyte         *scratchMemory, *scratchWalker, *texDataWalker, 
      red, green, blue, alpha;
   GLint           texDepth;
#ifdef MAC_OS
   unsigned char   buf[18];
   unsigned char   *p1;
   unsigned short  *p2;
#endif
   
   //Open texture map file for binary data reading.
   if ((fh = fopen (filename, "rb")) == NULL)
   {
      PRINTF (gszErrMsg, "Could not open texture map %s", filename);
      return;
   }
   
   //Read TARGA header
#ifdef WIN32
   if ((bytesRead = fread (&TGAHeader, sizeof (unsigned char), sizeof (TGAHeader), fh)) != sizeof (TGAHeader))
#endif
#ifdef MAC_OS
   if( (bytesRead = fread(&buf, sizeof(unsigned char), 18, fh)) != 18 )
#endif
      {
         PRINTF (gszErrMsg, "Bad Targa header");
         return;
      }
   
#ifdef MAC_OS
   p1                    = buf;
   TGAHeader.idlen       = *p1++; //length of optional identification sequence
   TGAHeader.cmtype      = *p1++; //indicates whether a palette is present
   TGAHeader.imtype      = *p1++; //image data type (e.g., uncompressed RGB)
   
   p2                    = (unsigned short *)p1;
   
   TGAHeader.cmorg       = *p2++; //first palette index, if present
   TGAHeader.cmcnt       = *p2++; //number of palette entries, if present
   
   p1                    = (unsigned char *)p2;
   
   TGAHeader.cmsize      = *p1++; //number of bits per palette entry
   
   p2                    = (unsigned short *)p1;
   
   TGAHeader.imxorg      = *p2++; //horiz pixel coordinate of lower left of image
   TGAHeader.imyorg      = *p2++; //vert pixel coordinate of lower left of image
   TGAHeader.imwidth     = *p2++; //image width in pixels
   TGAHeader.imheight    = *p2++; //image height in pixels
   
   p1                    = (unsigned char *)p2;
   
   TGAHeader.imdepth     = *p1++; //image color depth (bits per pixel)
   TGAHeader.imdesc      = *p1;   //image attribute flags
   
   TGAHeader.cmorg       = Endian16_Swap(TGAHeader.cmorg); //first palette index, if present
   TGAHeader.cmcnt       = Endian16_Swap(TGAHeader.cmcnt); //number of palette entries, if present
   TGAHeader.imxorg      = Endian16_Swap(TGAHeader.imxorg); //horiz pixel coordinate of lower left of image
   TGAHeader.imyorg      = Endian16_Swap(TGAHeader.imyorg); //vert pixel coordinate of lower left of image 
   TGAHeader.imwidth     = Endian16_Swap(TGAHeader.imwidth); //image width in pixels 
   TGAHeader.imheight    = Endian16_Swap(TGAHeader.imheight); //image height in pixels 
#endif
   
   m_textureName = name;
   m_internalFormat = internalFormat;
   m_textureFormat = format;
   m_textureIsMipMap = mipMap;
   m_textureWidth = TGAHeader.imwidth;
   m_textureHeight = TGAHeader.imheight;
   m_rawBPP = TGAHeader.imdepth;
   
   //Skip descriptive bytes at end of header, idlen specifies the number.
   if (fseek (fh, TGAHeader.idlen, SEEK_CUR) != 0)
   {
      PRINTF (gszErrMsg, "Couldn't seek past Targa header");
      return;
   }
   
   // Allocate memory for bitmap data
   imageSize = TGAHeader.imheight * TGAHeader.imwidth * (int) (TGAHeader.imdepth / 8);
   m_textureData = (GLubyte *) malloc (imageSize * sizeof (GLubyte));
   
   if(!m_textureData)
   {
      PRINTF (gszErrMsg, "Couldn't allocate texture memory");
      return;
   }
   
   //Read bitmap data into system memory (textureData).
   if ((bytesRead = fread (m_textureData, sizeof (GLubyte), imageSize, fh)) != imageSize)
   {
      PRINTF (gszErrMsg, "Couldn't allocate scratch buffer");
      return;
   }
   
   fclose(fh); // close the file
   
   
   // determine desired texture depth
   switch(m_textureFormat)
   {
      case GL_RGBA:
         texDepth = 4;
         break;
      case GL_RGB:
         texDepth = 3;
         break;
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_LUMINANCE:
         texDepth = 1;
         break;
      case GL_LUMINANCE_ALPHA:
         texDepth = 2;
         break;
   }
   
   // Allocate scratch memory for the texture map data for feeding to OpenGL
   scratchMemory = (GLubyte *) malloc(m_textureWidth * m_textureHeight * texDepth * sizeof(GLubyte));
   
   if(!scratchMemory)
   {
      PRINTF (gszErrMsg, "Couldn't allocate texture memory");
      return;
   }
   
   scratchWalker = scratchMemory;
   texDataWalker = m_textureData;
   
   for(int i = 0; i < (m_textureWidth * m_textureHeight); i++)
   {
      blue = *texDataWalker++; // grab the blue channel
      green = *texDataWalker++; // grab the green channel
      red = *texDataWalker++; // grab the red channel
      
      if(m_rawBPP == 32)
         alpha = *texDataWalker++; // grab the alpha channel
      else
         alpha = 0x00;
      
      switch(m_textureFormat)
      {
         case GL_RGBA:
            *scratchWalker++ = red;
            *scratchWalker++ = green;
            *scratchWalker++ = blue;
            *scratchWalker++ = alpha;
            break;
         case GL_RGB:
            *scratchWalker++ = red;
            *scratchWalker++ = green;
            *scratchWalker++ = blue;
            break;
         case GL_RED:
            *scratchWalker++ = red;
            break;
         case GL_GREEN:
            *scratchWalker++ = green;
            break;
         case GL_BLUE:
            *scratchWalker++ = blue;
            break;
         case GL_ALPHA:
            *scratchWalker++ = alpha;
            break;
         case GL_LUMINANCE:
            *scratchWalker++ = alpha; // totally arbitrary hack - JasonM
            break;
         case GL_LUMINANCE_ALPHA:
            *scratchWalker++ = blue;  // totally arbitrary hack - JasonM
            *scratchWalker++ = alpha;
            break;
      }
      
   }
   
   // Make the texture object
   glBindTexture(GL_TEXTURE_2D, m_textureName);
   
   if(!m_textureIsMipMap)
   {
      glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_textureWidth, m_textureHeight, 0, m_textureFormat,
                   GL_UNSIGNED_BYTE, (GLvoid *)scratchMemory);
   }
   else
   {
      gluBuild2DMipmaps(GL_TEXTURE_2D, m_internalFormat, m_textureWidth, m_textureHeight, m_textureFormat,
                        GL_UNSIGNED_BYTE, (GLvoid *)scratchMemory);
   }
   
   free(scratchMemory);
}



ATI_GLTexture::~ATI_GLTexture()
{
   free(m_textureData);
   glDeleteTextures(1, &m_textureName);
}


void ATI_GLTexture::QueryTexel(GLfloat floatX, GLfloat floatY, GLubyte *red, GLubyte *green, GLubyte *blue, GLubyte *alpha)
{
   GLubyte x, y;
   
   x = (GLubyte) (m_textureWidth  * floatX);
   y = (GLubyte) (m_textureHeight * floatY);
   
   x = min(m_textureWidth, max(0, x));
   y = min(m_textureHeight, max(0, y));
   
   
   switch(m_textureFormat)
   {
      case GL_RGBA:
         *red    = *(m_textureData+y*4*m_textureWidth+x);
         *green  = *(m_textureData+y*4*m_textureWidth+x+1);
         *blue   = *(m_textureData+y*4*m_textureWidth+x+2);
         *alpha  = *(m_textureData+y*4*m_textureWidth+x+3);
         break;
      case GL_RGB:
         *red    = *(m_textureData+y*3*m_textureWidth+x);
         *green  = *(m_textureData+y*3*m_textureWidth+x+1);
         *blue   = *(m_textureData+y*3*m_textureWidth+x+2);
         *alpha  = 0x00;
         break;
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_LUMINANCE:
         *red = *green = *blue = *alpha = *(m_textureData+y*m_textureWidth+x);
         break;
      case GL_LUMINANCE_ALPHA:
         *red = *green = *blue = *(m_textureData+y*m_textureWidth+x);
         *alpha = *(m_textureData+y*m_textureWidth+x+1);
         break;
   }
}



void ATI_GLTexture::Reload(char *filename, GLboolean mipMap, GLenum format, GLenum internalFormat)
{
   
   TGAHeaderInfo  TGAHeader;
   DWORD          imageSize;
   FILE           *fh;
   DWORD          bytesRead;
   GLubyte        *scratchMemory, *scratchWalker, *texDataWalker, 
      red, green, blue, alpha;
   GLint          texDepth;
   
   //Open texture map file for binary data reading.
   if ((fh = fopen (filename, "rb")) == NULL)
   {
      PRINTF (gszErrMsg, "Could not open texture map %s", filename);
      return;
   }
   
   //Read TARGA header.
   if ((bytesRead = fread (&TGAHeader, sizeof (unsigned char), sizeof (TGAHeader), fh)) != sizeof (TGAHeader))
   {
      PRINTF (gszErrMsg, "Bad Targa header");
      return;
   }
   
   m_internalFormat = internalFormat;
   m_textureFormat = format;
   m_textureIsMipMap = mipMap;
   m_textureWidth = TGAHeader.imwidth;
   m_textureHeight = TGAHeader.imheight;
   m_rawBPP = TGAHeader.imdepth;
   
   //Skip descriptive bytes at end of header, idlen specifies the number.
   if (fseek (fh, TGAHeader.idlen, SEEK_CUR) != 0)
   {
      PRINTF (gszErrMsg, "Couldn't seek past Targa header");
      return;
   }
   
   // If the texture already has data, free it
   if(m_textureData)
      free(m_textureData);
   
   // Allocate memory for new bitmap data
   imageSize = TGAHeader.imheight * TGAHeader.imwidth * (int) (TGAHeader.imdepth / 8);
   m_textureData = (GLubyte *) malloc (imageSize * sizeof (GLubyte));
   
   if(!m_textureData)
   {
      PRINTF (gszErrMsg, "Couldn't allocate texture memory when Reloading texture");
      return;
   }
   
   //Read bitmap data into system memory (m_textureData).
   if ((bytesRead = fread (m_textureData, sizeof (GLubyte), imageSize, fh)) != imageSize)
   {
      PRINTF (gszErrMsg, "Couldn't allocate scratch buffer");
      return;
   }
   
   fclose(fh); // close the file
   
   
   // determine desired texture depth
   switch(m_textureFormat)
   {
      case GL_RGBA:
         texDepth = 4;
         break;
      case GL_RGB:
         texDepth = 3;
         break;
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_LUMINANCE:
         texDepth = 1;
         break;
      case GL_LUMINANCE_ALPHA:
         texDepth = 2;
         break;
   }
   
   // Allocate scratch memory for the texture map data for feeding to OpenGL
   scratchMemory = (GLubyte *) malloc(m_textureWidth * m_textureHeight * texDepth * sizeof(GLubyte));
   
   if(!scratchMemory)
   {
      PRINTF (gszErrMsg, "Couldn't allocate texture memory");
      return;
   }
   
   scratchWalker = scratchMemory;
   texDataWalker = m_textureData;
   
   for(int i = 0; i < (m_textureWidth * m_textureHeight); i++)
   {
      blue = *texDataWalker++; // grab the blue channel
      green = *texDataWalker++; // grab the green channel
      red = *texDataWalker++; // grab the red channel
      
      if(m_rawBPP == 32)
         alpha = *texDataWalker++; // grab the alpha channel
      else
         alpha = 0x00;
      
      switch(m_textureFormat)
      {
         case GL_RGBA:
            *scratchWalker++ = red;
            *scratchWalker++ = green;
            *scratchWalker++ = blue;
            *scratchWalker++ = alpha;
            break;
         case GL_RGB:
            *scratchWalker++ = red;
            *scratchWalker++ = green;
            *scratchWalker++ = blue;
            break;
         case GL_RED:
            *scratchWalker++ = red;
            break;
         case GL_GREEN:
            *scratchWalker++ = green;
            break;
         case GL_BLUE:
            *scratchWalker++ = blue;
            break;
         case GL_ALPHA:
            *scratchWalker++ = alpha;
            break;
         case GL_LUMINANCE:
            *scratchWalker++ = alpha;  // totally arbitrary hack - JasonM
            break;
         case GL_LUMINANCE_ALPHA:
            *scratchWalker++ = blue;   // totally arbitrary hack - JasonM
            *scratchWalker++ = alpha;
            break;
      }
      
   }
   
   // Make the texture object
   glBindTexture(GL_TEXTURE_2D, m_textureName);
   
   glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_textureWidth, m_textureHeight, 0, m_textureFormat,
                GL_UNSIGNED_BYTE, (GLvoid *)scratchMemory);
   
   free(scratchMemory);
   
}


void ATI_GLTexture::Reformat(GLenum format, GLenum internalFormat)
{
   GLubyte  *scratchMemory, *scratchWalker, *texDataWalker, 
      red, green, blue, alpha;
   GLint    texDepth;
   
   m_internalFormat = internalFormat;
   m_textureFormat = format;
   
   // determine desired texture depth
   switch(m_textureFormat)
   {
      case GL_RGBA:
         texDepth = 4;
         break;
      case GL_RGB:
         texDepth = 3;
         break;
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_LUMINANCE:
         texDepth = 1;
         break;
      case GL_LUMINANCE_ALPHA:
         texDepth = 2;
         break;
   }
   
   // Allocate scratch memory for the texture map data for feeding to OpenGL
   scratchMemory = (GLubyte *) malloc(m_textureWidth * m_textureHeight * texDepth * sizeof(GLubyte));
   
   if(!scratchMemory)
   {
      PRINTF (gszErrMsg, "Couldn't allocate texture memory");
      return;
   }
   
   scratchWalker = scratchMemory;
   texDataWalker = m_textureData;
   
   for(int i = 0; i < (m_textureWidth * m_textureHeight); i++)
   {
      blue = *texDataWalker++; // grab the blue channel
      green = *texDataWalker++; // grab the green channel
      red = *texDataWalker++; // grab the red channel
      
      if(m_rawBPP == 32)
         alpha = *texDataWalker++; // grab the alpha channel
      else
         alpha = 0x00;
      
      switch(m_textureFormat)
      {
         case GL_RGBA:
            *scratchWalker++ = red;
            *scratchWalker++ = green;
            *scratchWalker++ = blue;
            *scratchWalker++ = alpha;
            break;
         case GL_RGB:
            *scratchWalker++ = red;
            *scratchWalker++ = green;
            *scratchWalker++ = blue;
            break;
         case GL_RED:
            *scratchWalker++ = red;
            break;
         case GL_GREEN:
            *scratchWalker++ = green;
            break;
         case GL_BLUE:
            *scratchWalker++ = blue;
            break;
         case GL_ALPHA:
            *scratchWalker++ = alpha;
            break;
         case GL_LUMINANCE:
            *scratchWalker++ = alpha;  // totally arbitrary hack - JasonM
            break;
         case GL_LUMINANCE_ALPHA:
            *scratchWalker++ = blue;   // totally arbitrary hack - JasonM
            *scratchWalker++ = alpha;
            break;
      }
      
   }
   
   // Make the texture object
   glBindTexture(GL_TEXTURE_2D, m_textureName);
   
   glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_textureWidth, m_textureHeight, 0, m_textureFormat,
                GL_UNSIGNED_BYTE, (GLvoid *)scratchMemory);
   
   free(scratchMemory);
}

//////////////////////////
// Reads in a targa file and returns it's header and data.
//   filename -- the name of the file to open
//   TGAHeader -- location to store the targa header
//   textureData -- location to store the targa data
//////////////////////////
int
ATI_GLCubemap::ReadTGAFile (char *filename, TGAHeaderInfo *TGAHeader,
                                GLubyte **textureData)
{
   FILE            *fh;
   DWORD           imageSize;
   DWORD           bytesRead;
#ifdef MAC_OS
   unsigned char   buf[18];
   unsigned char   *p1;
   unsigned short  *p2;
#endif

   //Open texture map file for binary data reading.
   if ((fh = fopen (filename, "rb")) == NULL)
   {
      PRINTF (gszErrMsg, "Could not open texture map %s", filename);
      return 0;
   }
   
   //Read TARGA header
#ifdef WIN32
   if ((bytesRead = fread (TGAHeader, sizeof (unsigned char), sizeof (*TGAHeader), fh)) != sizeof (*TGAHeader))
#endif
#ifdef MAC_OS
   if( (bytesRead = fread (&buf, sizeof(unsigned char), 18, fh)) != 18 )
#endif
      {
         PRINTF (gszErrMsg, "Bad Targa header");
         return 0;
      }
   
#ifdef MAC_OS
   p1                     = buf;
   TGAHeader->idlen       = *p1++; //length of optional identification sequence
   TGAHeader->cmtype      = *p1++; //indicates whether a palette is present
   TGAHeader->imtype      = *p1++; //image data type (e.g., uncompressed RGB)
   
   p2                     = (unsigned short *)p1;
   
   TGAHeader->cmorg       = *p2++; //first palette index, if present
   TGAHeader->cmcnt       = *p2++; //number of palette entries, if present
   
   p1                     = (unsigned char *)p2;
   
   TGAHeader->cmsize      = *p1++; //number of bits per palette entry
   
   p2                     = (unsigned short *)p1;
   
   TGAHeader->imxorg      = *p2++; //horiz pixel coordinate of lower left of image
   TGAHeader->imyorg      = *p2++; //vert pixel coordinate of lower left of image
   TGAHeader->imwidth     = *p2++; //image width in pixels
   TGAHeader->imheight    = *p2++; //image height in pixels
   
   p1                     = (unsigned char *)p2;
   
   TGAHeader->imdepth     = *p1++; //image color depth (bits per pixel)
   TGAHeader->imdesc      = *p1;   //image attribute flags
   
   TGAHeader->cmorg       = Endian16_Swap(TGAHeader->cmorg); //first palette index, if present
   TGAHeader->cmcnt       = Endian16_Swap(TGAHeader->cmcnt); //number of palette entries, if present
   TGAHeader->imxorg      = Endian16_Swap(TGAHeader->imxorg); //horiz pixel coordinate of lower left of image
   TGAHeader->imyorg      = Endian16_Swap(TGAHeader->imyorg); //vert pixel coordinate of lower left of image 
   TGAHeader->imwidth     = Endian16_Swap(TGAHeader->imwidth); //image width in pixels 
   TGAHeader->imheight    = Endian16_Swap(TGAHeader->imheight); //image height in pixels 
#endif
      
   //Skip descriptive bytes at end of header, idlen specifies the number.
   if (fseek (fh, TGAHeader->idlen, SEEK_CUR) != 0)
   {
      PRINTF (gszErrMsg, "Couldn't seek past Targa header");
      return 0;
   }
   
   // Allocate memory for bitmap data
   imageSize = TGAHeader->imheight * TGAHeader->imwidth * (int) (TGAHeader->imdepth / 8);
   (*textureData) = (GLubyte *) malloc (imageSize * sizeof (GLubyte));
   
   if (!(*textureData))
   {
      PRINTF (gszErrMsg, "Couldn't allocate texture memory");
      return 0;
   }
   
   //Read bitmap data into system memory (textureData).
   if ((bytesRead = fread (*textureData, sizeof (GLubyte), imageSize, fh)) != imageSize)
   {
      PRINTF (gszErrMsg, "Couldn't allocate texture memory");
      return 0;
   }
   
   fclose(fh); // close the file
   return 1;
}

//////////////////////////
// Converts the raw texture data into the specified format.
//////////////////////////
int
ATI_GLCubemap::ReformatData (TGAHeaderInfo *TGAHeader, GLubyte *textureData,
                             GLenum format, GLubyte **outData)
{
   GLubyte  *scratchWalker, *texDataWalker, 
            red, green, blue, alpha;
   GLint    texDepth;

   // determine desired texture depth
   switch (format)
   {
      case GL_RGBA:
         texDepth = 4;
         break;
      case GL_RGB:
         texDepth = 3;
         break;
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_LUMINANCE:
         texDepth = 1;
         break;
      case GL_LUMINANCE_ALPHA:
         texDepth = 2;
         break;
   }
   
   // Allocate scratch memory for the texture map data for feeding to OpenGL
   (*outData) = (GLubyte *) malloc (TGAHeader->imwidth * TGAHeader->imheight *
                                    texDepth * sizeof(GLubyte));
   
   if(!(*outData))
   {
      PRINTF (gszErrMsg, "Couldn't allocate scratch memory");
      return 0;
   }
   
   scratchWalker = (*outData);
   texDataWalker = textureData;
   
   for(int i = 0; i < (TGAHeader->imwidth * TGAHeader->imheight); i++)
   {
      blue = *texDataWalker++; // grab the blue channel
      green = *texDataWalker++; // grab the green channel
      red = *texDataWalker++; // grab the red channel
      
      if (TGAHeader->imdepth == 32)
      {
         alpha = *texDataWalker++; // grab the alpha channel
      }
      else
      {
         alpha = 0x00;
      }
      
      switch (format)
      {
         case GL_RGBA:
            *scratchWalker++ = red;
            *scratchWalker++ = green;
            *scratchWalker++ = blue;
            *scratchWalker++ = alpha;
            break;
         case GL_RGB:
            *scratchWalker++ = red;
            *scratchWalker++ = green;
            *scratchWalker++ = blue;
            break;
         case GL_RED:
            *scratchWalker++ = red;
            break;
         case GL_GREEN:
            *scratchWalker++ = green;
            break;
         case GL_BLUE:
            *scratchWalker++ = blue;
            break;
         case GL_ALPHA:
            *scratchWalker++ = alpha;
            break;
         case GL_LUMINANCE:
            *scratchWalker++ = alpha; // totally arbitrary hack - JasonM
            break;
         case GL_LUMINANCE_ALPHA:
            *scratchWalker++ = blue;  // totally arbitrary hack - JasonM
            *scratchWalker++ = alpha;
            break;
      }
      
   }
   return 1;
}

//////////////////////////
// Constructor for a cubemap.
//   This adds 1-6 to the filename (pre extension) for each face of cube.
//////////////////////////
ATI_GLCubemap::ATI_GLCubemap(char *filename, GLint name, GLboolean mipMap, 
                             GLenum format, GLint internalFormat)
{
   // Save off the info about the texture.
   m_textureName = name;
   m_internalFormat = internalFormat;
   m_textureFormat = format;
   m_textureIsMipMap = mipMap;

   // Create the GL texture object
   glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, m_textureName);
   
   // Load up all 6 images
   for (int i = 0; i < 6; i++)
   {
      // Determine cube filename
      char cubefile[256];
      char *dotOld = strrchr (filename, '.');
      strcpy (cubefile, filename);
      char *dotNew = strrchr (cubefile, '.');
      sprintf(dotNew, "%d%s", i + 1, dotOld);

      // Read the cubemap face image from the file
      TGAHeaderInfo TGAHeader;
      if (!ReadTGAFile (cubefile, &TGAHeader, &m_textureData))
      {
         return;
      }
      m_textureWidth = TGAHeader.imwidth;
      m_textureHeight = TGAHeader.imheight;
      m_rawBPP = TGAHeader.imdepth;

      // Reformat as needed.
      GLubyte *scratch;
      if (!ReformatData (&TGAHeader, m_textureData, format, &scratch))
      {
         return;
      }

      // Load the cube face
      glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + i, 
                    0, m_internalFormat, m_textureWidth, m_textureHeight, 
                    0, m_textureFormat, GL_UNSIGNED_BYTE,
                    (GLvoid *)scratch);
      free (scratch);
   }
   

}
