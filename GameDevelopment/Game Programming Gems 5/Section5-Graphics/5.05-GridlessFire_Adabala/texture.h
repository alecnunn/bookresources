/***********************************************
                                                                                
Demo for chapter "Gridless Controllable Fire"
in Games Programming Gems 5.
                                                                                
Author: Neeharika Adabala
                                                                                
Date: August 2004
                                                                                
************************************************/

#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/gl.h>
#include <GL/glu.h>

class Texture {
public:
	enum {
		TRILINEAR = 1 << 1,
		LINEAR = 1 << 2,
		MIPMAP_SGIS = 1 << 3,
		CLAMP = 1 << 4,
		CLAMP_TO_EDGE = 1 << 5
	};
	enum {
		TEXTURE_1D = GL_TEXTURE_1D,
		TEXTURE_2D = GL_TEXTURE_2D
	};

	Texture();
	Texture(const char *name,int flag = TRILINEAR | MIPMAP_SGIS,int texture_target = TEXTURE_2D);
	~Texture();
	
	void load(const char *name,int flag = TRILINEAR | MIPMAP_SGIS,int texture_target = TEXTURE_2D);
	
	void enable();
	void disable();
	void bind();
	
	static unsigned char *load(const char *name,int *width,int *height);
	static unsigned char *load_png(const char *name,int *width,int *height);
	static unsigned char *load_jpeg(const char *name,int *width,int *height);
	
	GLenum target;
	GLuint id;
        unsigned char *data;
};

#endif 
