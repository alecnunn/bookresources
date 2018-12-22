/*

	Visual Terrain Grid Determination Using Frustrum
	written by Christopher Tremblay
	Last revision (26 Jan / 2003)

	This source code is a complement to the book:
	"Graphics Programming Methods" by 
	Charles River Media

	All rights reserved
*/
#include "GLImage.hpp"


// Generic function to load an image file given by File
// It can easilly be link with SDL_Image by to minimize
// dependency, it only supports BMP
SDL_Surface *LoadSDLImage(char *File)
{
	SDL_Surface		*New, *Surf = SDL_LoadBMP(File);

	if (!Surf) {
		char txt[1024];

		sprintf(txt, "Error: Can't load %s (%s)\n", File, SDL_GetError());
		REPORT(txt);

		return NULL;
	}

	// With Alpha...
	if (Surf->format->Amask) {
		SDL_PixelFormat  Format = 
		{
			NULL,							// Palette
			32,								// Bits per Pel
			4,								// Bytes per Pel
			0,								// R loss
			0,								// G loss
			0,								// B loss
			0,								// A loss
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			24,
			16,
			8,
			0,
			0xff000000,
			0x00ff0000,
			0x0000ff00,
			0x000000ff,
	#else
			0,
			8,
			16,
			24,
			0x000000ff,
			0x0000ff00,
			0x00ff0000,
			0xff000000,
	#endif
			0,								// Color Key
			255,							// Alpha bias
		};

		New = SDL_ConvertSurface(Surf, &Format, 0);

	// Without Alpha...
	} else {
		SDL_PixelFormat  Format = 
		{
			NULL,							// Palette
			24,								// Bits per Pel
			3,								// Bytes per Pel
			0,								// R loss
			0,								// G loss
			0,								// B loss
			8,								// A loss
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			0,
			16,
			8,
			0,
			0x00000000,
			0x00ff0000,
			0x0000ff00,
			0x000000ff,
	#else
			0,
			8,
			16,
			0,
			0x000000ff,
			0x0000ff00,
			0x00ff0000,
			0x00000000,
	#endif
			0,								// Color Key
			255,							// Alpha bias
		};

		New = SDL_ConvertSurface(Surf, &Format, 0);
	}

	SDL_FreeSurface(Surf);

	return New;
}



// Function to load an image and return an OpenGL TextureID
GLuint LoadGLImage(char *File)
{
	SDL_Surface *Image	= LoadSDLImage(File);
	GLuint ID			= -1;

	if (!Image)
		return -1;

	// Initialize the OpenGL image
	glGenTextures		(1, &ID);
	glBindTexture		(GL_TEXTURE_2D, ID);
	glTexParameteri		(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri		(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	if (Image->format->Amask)
		gluBuild2DMipmaps	(GL_TEXTURE_2D, Image->format->BytesPerPixel, Image->w, 
							 Image->h, GL_RGBA, GL_UNSIGNED_BYTE, Image->pixels);
	else
		gluBuild2DMipmaps	(GL_TEXTURE_2D, Image->format->BytesPerPixel, Image->w, 
							 Image->h, GL_RGB, GL_UNSIGNED_BYTE, Image->pixels);

	SDL_FreeSurface		(Image);

	return ID;
}
