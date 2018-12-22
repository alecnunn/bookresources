//
// Perspective Shadow Maps
// accompanying example code for Game Programming Gems 4
// (w)(c) 2003, Carsten Dachsbacher (dachsbacher@cs.fau.de), Marc Stamminger (stamminger@cs.fau.de)
//
#include <windows.h>
#include <gl\gl.h>          
#include <gl\glu.h>
            
#include "wglext.h"
#include "p_buffer.h"
#include "oglExtension.h"

CPBuffer::CPBuffer( int _x, int _y, HDC _hDC ) : sizeX(_x), sizeY(_y), hPBuffer(NULL), exists(0)
{
    // create texture id, to which the p-buffer is bound
    glGenTextures( 1, &textureID );
    glBindTexture( GL_TEXTURE_2D, textureID );
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_LUMINANCE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL );

    // pixel format attributes for the p-buffers
    //
    int pfAttribute[] =
    {
        WGL_SUPPORT_OPENGL_ARB,       TRUE,
        WGL_DRAW_TO_PBUFFER_ARB,      TRUE,
        WGL_BIND_TO_TEXTURE_RGBA_ARB, TRUE,
        WGL_DOUBLE_BUFFER_ARB,        FALSE, 
        0
    };

    // look for format
    //
    unsigned int nFormat = 0;
      signed int pixelFormat;
    wglChoosePixelFormatARB( _hDC, pfAttribute, NULL, 1, &pixelFormat, &nFormat );

    if ( nFormat == 0 )
    {
        MessageBox( NULL, "No matching pixel format found !", "PSM", MB_OK );
        return;
    }

    // p-buffer attributes
    //
    int pbAttribute[] =
    {
        WGL_DEPTH_TEXTURE_FORMAT_NV, WGL_TEXTURE_DEPTH_COMPONENT_NV,
        WGL_TEXTURE_FORMAT_ARB,      WGL_TEXTURE_RGBA_ARB, 
        WGL_TEXTURE_TARGET_ARB,      WGL_TEXTURE_2D_ARB,
        0

    };

    // and create one
    //
    hPBuffer = wglCreatePbufferARB( _hDC, pixelFormat, sizeX, sizeY, pbAttribute );
    hDC      = wglGetPbufferDCARB( hPBuffer );
    hRC      = wglCreateContext( hDC );

    if( !hPBuffer )
    {
        MessageBox( NULL, "Could not create p-buffer !", "PSM", MB_OK );
        return;
    }

    int __x, __y;

    wglQueryPbufferARB( hPBuffer, WGL_PBUFFER_WIDTH_ARB,  &__x );
    wglQueryPbufferARB( hPBuffer, WGL_PBUFFER_HEIGHT_ARB, &__y );

    if( !( __x == sizeX && __y == sizeY ) )
    {
        MessageBox( NULL, "Size of p-buffer is not as demanded !", "PSM", MB_OK );
        return;
    }

    exists = 1;
}

CPBuffer::~CPBuffer()
{
    // release binding and destroy
    release();

    wglDeleteContext( hRC );
    wglReleasePbufferDCARB( hPBuffer, hDC );
    wglDestroyPbufferARB( hPBuffer );
}


int CPBuffer::bind( GLenum drawBuffer )
{
    if ( !exists ) return 0;

    if( !wglBindTexImageARB( hPBuffer, drawBuffer ) )
        return 0;

    bound = drawBuffer;

    return 1;
}

int CPBuffer::release()
{
    if ( !exists ) return 0;

    if( !wglReleaseTexImageARB( hPBuffer, bound ) )
        return 0;

    return 1;
}

int CPBuffer::makeCurrent()
{
    if ( !exists ) return 0;

    if( !wglMakeCurrent( hDC, hRC ) )
        return 0;

    return 1;
}



