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
// GLTexture.h - Texture manager for OpenGL
//
//

#ifndef ATI_GLTEXTURE_H
#define ATI_GLTEXTURE_H

#ifdef MAC_OS
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
#endif

//Targa header info
#pragma pack (push)
#pragma pack (1)	//dont pad the following struct

#ifdef MAC_OS
#pragma options align=mac68k
#endif

typedef struct _TGAHeaderInfo
{
	BYTE	idlen;		//length of optional identification sequence
	BYTE	cmtype;		//indicates whether a palette is present
	BYTE	imtype;		//image data type (e.g., uncompressed RGB)
	WORD 	cmorg;		//first palette index, if present
	WORD	cmcnt;		//number of palette entries, if present
	BYTE	cmsize;		//number of bits per palette entry
	WORD	imxorg;		//horiz pixel coordinate of lower left of image
	WORD	imyorg;		//vert pixel coordinate of lower left of image
	WORD	imwidth;	//image width in pixels
	WORD	imheight;	//image height in pixels
	BYTE	imdepth;	//image color depth (bits per pixel)
	BYTE	imdesc;		//image attribute flags
}TGAHeaderInfo;

class ATI_GLTexture
{
	GLubyte		*m_textureData; // raw RGBA or RGB bits from (TGA) file...
	GLint		 m_rawBPP;
	GLenum		 m_textureFormat;
	GLint		 m_internalFormat;
	GLboolean	 m_textureIsMipMap;
	GLuint		 m_textureName;
public:
	GLint		 m_textureWidth;
	GLint		 m_textureHeight;
	ATI_GLTexture(char *filename, GLint name, GLboolean mipMap, GLenum format, GLint internalFormat);
	~ATI_GLTexture(void);
	void Reload(char *filename, GLboolean mipMap, GLenum format, GLenum internalFormat);
	void Reformat(GLenum format, GLenum internalFormat);
	void QueryTexel(GLfloat floatX, GLfloat floatY, GLubyte *red, GLubyte *green, GLubyte *blue, GLubyte *alpha);

};


class ATI_GLCubemap
{
	GLubyte		*m_textureData; // raw RGBA or RGB bits from (TGA) file...
	GLint		 m_rawBPP;
	GLenum		 m_textureFormat;
	GLint		 m_internalFormat;
	GLboolean	 m_textureIsMipMap;
	GLuint		 m_textureName;
public:
	GLint		 m_textureWidth;
	GLint		 m_textureHeight;
	ATI_GLCubemap(char *filename, GLint name, GLboolean mipMap, GLenum format, GLint internalFormat);
	~ATI_GLCubemap(void);

        int ReadTGAFile (char *filename, TGAHeaderInfo *TGAHeader, 
                         GLubyte **textureData);
        int ReformatData (TGAHeaderInfo *TGAHeader, GLubyte *textureData,
                          GLenum format, GLubyte **outData);

        /*
	void Reload(char *filename, GLboolean mipMap, GLenum format, GLenum internalFormat);
	void Reformat(GLenum format, GLenum internalFormat);
	void QueryTexel(GLfloat floatX, GLfloat floatY, GLubyte *red, GLubyte *green, GLubyte *blue, GLubyte *alpha);
        */
};

#pragma pack (pop)

#ifdef MAC_OS
#pragma options align=reset
#endif


#endif // ATI_GLTEXTURE_H

