/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_GL_HEADER_H_
#define _BB_GL_HEADER_H_

#ifdef _WIN32
   #include<windows.h>
   #include<gl/gl.h>
   #include<gl/glu.h>
   #include<gl/glext.h>
   #include<OpenGLExtensions.h>
#else
#ifdef _MAC
   #include<Carbon/Carbon.h>
   #include<agl/agl.h>
   #include<OpenGL/gl.h>
   #include<OpenGL/glu.h>
   #include<OpenGL/glext.h>
#else
   #include<GL/glx.h>
   #include<GL/gl.h>
   #include<GL/glu.h>
   #include<GL/glext.h>
   #include<GL/glxext.h>
   #include<OpenGLExtensions.h>
   #include<X11/extensions/xf86vmode.h>
   #include<X11/keysym.h>
#endif
#endif

#include<GLTexture.h>

#endif
