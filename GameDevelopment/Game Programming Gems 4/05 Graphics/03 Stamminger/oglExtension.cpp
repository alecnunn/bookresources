//
// Perspective Shadow Maps
// accompanying example code for Game Programming Gems 4
// (w)(c) 2003, Carsten Dachsbacher (dachsbacher@cs.fau.de), Marc Stamminger (stamminger@cs.fau.de)
//
#include <windows.h>
#include <stdio.h>

#include "oglextension.h"

#define MAX_EXTENSION_SPACE     10240
#define MAX_EXTENSION_LENGTH    256

bool    supportARB_vertex_program       = false;
bool    supportARB_fragment_program     = false;
bool    supportWGL_ARB_pbuffer          = false;
bool    supportWGL_ARB_pixel_format     = false;
bool    supportWGL_ARB_render_texture   = false;

// ARB vertex program & ARB fragment program
// 
PFNGLGENPROGRAMSARBPROC                glGenProgramsARB           = NULL;
PFNGLDELETEPROGRAMSARBPROC             glDeleteProgramsARB        = NULL;
PFNGLBINDPROGRAMARBPROC                glBindProgramARB           = NULL;
PFNGLPROGRAMSTRINGARBPROC              glProgramStringARB         = NULL;
PFNGLPROGRAMENVPARAMETER4FARBPROC      glProgramEnvParameter4fARB = NULL;

// WGL_ARB_pbuffer
//
PFNWGLCREATEPBUFFERARBPROC              wglCreatePbufferARB    = NULL;
PFNWGLGETPBUFFERDCARBPROC               wglGetPbufferDCARB     = NULL;
PFNWGLRELEASEPBUFFERDCARBPROC           wglReleasePbufferDCARB = NULL;
PFNWGLDESTROYPBUFFERARBPROC             wglDestroyPbufferARB   = NULL;
PFNWGLQUERYPBUFFERARBPROC               wglQueryPbufferARB     = NULL;

// WGL_ARB_pixel_format
//
PFNWGLGETPIXELFORMATATTRIBIVARBPROC     wglGetPixelFormatAttribivARB = NULL;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC     wglGetPixelFormatAttribfvARB = NULL;
PFNWGLCHOOSEPIXELFORMATARBPROC          wglChoosePixelFormatARB      = NULL;

// WGL_ARB_render_texture
//
PFNWGLBINDTEXIMAGEARBPROC               wglBindTexImageARB     = NULL;
PFNWGLRELEASETEXIMAGEARBPROC            wglReleaseTexImageARB  = NULL;
PFNWGLSETPBUFFERATTRIBARBPROC           wglSetPbufferAttribARB = NULL;

// WGL_ARB_extensions_string
//
PFNWGLGETEXTENSIONSSTRINGARBPROC        wglGetExtensionsStringARB = NULL;


#ifdef _WIN32
#define GETEXT( function, name ) \
    *(void**)&function = (void *)wglGetProcAddress( name );
#else
#define GETEXT( function, name ) \
    *(void**)&function = (void *)glXGetProcAddressARB( (byte*)name );
#endif


void getOpenGLExtensions() 
{
    GETEXT( wglGetExtensionsStringARB, "wglGetExtensionsStringARB" );

    char *vendor        = (char*)glGetString( GL_VENDOR );
    char *version       = (char*)glGetString( GL_VERSION );
    char *renderer      = (char*)glGetString( GL_RENDERER );
    char *extensions    = (char*)glGetString( GL_EXTENSIONS );
    char *wglextensions = NULL;

    if ( wglGetExtensionsStringARB )
        wglextensions = (char*)wglGetExtensionsStringARB( wglGetCurrentDC() );

    if ( strstr( extensions, "GL_ARB_vertex_program" ) )
    {
        supportARB_vertex_program = true;

        GETEXT( glGenProgramsARB,           "glGenProgramsARB" );
        GETEXT( glDeleteProgramsARB,        "glDeleteProgramsARB" );
        GETEXT( glBindProgramARB,           "glBindProgramARB" );
        GETEXT( glProgramStringARB,         "glProgramStringARB" );
        GETEXT( glProgramEnvParameter4fARB, "glProgramEnvParameter4fARB" );
    }

    if ( strstr( extensions, "GL_ARB_fragment_program" ) )
    {
        supportARB_fragment_program = true;
    }

    if ( strstr( wglextensions, "WGL_ARB_pbuffer" ) )
    {
        supportWGL_ARB_pbuffer = true;
        GETEXT( wglCreatePbufferARB,        "wglCreatePbufferARB" );
        GETEXT( wglGetPbufferDCARB,         "wglGetPbufferDCARB" );
        GETEXT( wglReleasePbufferDCARB,     "wglReleasePbufferDCARB" );
        GETEXT( wglDestroyPbufferARB,       "wglDestroyPbufferARB" );
        GETEXT( wglQueryPbufferARB,         "wglQueryPbufferARB" );
    }

    if ( strstr( wglextensions, "WGL_ARB_pixel_format" ) )
    {
        supportWGL_ARB_pixel_format = true;
        GETEXT( wglGetPixelFormatAttribivARB,   "wglGetPixelFormatAttribivARB" );
        GETEXT( wglGetPixelFormatAttribfvARB,   "wglGetPixelFormatAttribfvARB" );
        GETEXT( wglChoosePixelFormatARB,        "wglChoosePixelFormatARB" );
    }
    
    if ( strstr( wglextensions, "WGL_ARB_render_texture" ) )
    {
        supportWGL_ARB_render_texture = true;
        GETEXT( wglBindTexImageARB,         "wglBindTexImageARB" );
        GETEXT( wglReleaseTexImageARB,      "wglReleaseTexImageARB" );
        GETEXT( wglSetPbufferAttribARB,     "wglSetPbufferAttribARB" );
    }
}

