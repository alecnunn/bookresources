//
// Perspective Shadow Maps
// accompanying example code for Game Programming Gems 4
// (w)(c) 2003, Carsten Dachsbacher (dachsbacher@cs.fau.de), Marc Stamminger (stamminger@cs.fau.de)
//
#ifndef __OGLEXTENSION
#define __OGLEXTENSION

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "glext.h"
#include "wglext.h"

extern bool supportARB_vertex_program;
extern bool supportARB_fragment_program;
extern bool supportWGL_ARB_pbuffer;
extern bool supportWGL_ARB_pixel_format;
extern bool supportWGL_ARB_render_texture;

// ARB vertex program
extern PFNGLGENPROGRAMSARBPROC                glGenProgramsARB;
extern PFNGLDELETEPROGRAMSARBPROC             glDeleteProgramsARB;
extern PFNGLBINDPROGRAMARBPROC                glBindProgramARB;
extern PFNGLPROGRAMSTRINGARBPROC              glProgramStringARB;
extern PFNGLPROGRAMENVPARAMETER4FARBPROC      glProgramEnvParameter4fARB;

// WGL_ARB_pbuffer
extern PFNWGLCREATEPBUFFERARBPROC             wglCreatePbufferARB;
extern PFNWGLGETPBUFFERDCARBPROC              wglGetPbufferDCARB;
extern PFNWGLRELEASEPBUFFERDCARBPROC          wglReleasePbufferDCARB;
extern PFNWGLDESTROYPBUFFERARBPROC            wglDestroyPbufferARB;
extern PFNWGLQUERYPBUFFERARBPROC              wglQueryPbufferARB;

// WGL_ARB_pixel_format
extern PFNWGLGETPIXELFORMATATTRIBIVARBPROC    wglGetPixelFormatAttribivARB;
extern PFNWGLGETPIXELFORMATATTRIBFVARBPROC    wglGetPixelFormatAttribfvARB;
extern PFNWGLCHOOSEPIXELFORMATARBPROC         wglChoosePixelFormatARB;

// WGL_ARB_render_texture
extern PFNWGLBINDTEXIMAGEARBPROC              wglBindTexImageARB;
extern PFNWGLRELEASETEXIMAGEARBPROC           wglReleaseTexImageARB;
extern PFNWGLSETPBUFFERATTRIBARBPROC          wglSetPbufferAttribARB;

extern void getOpenGLExtensions();

#endif