#ifndef __wgl_ATI_h_
#define __wgl_ATI_h_
//
// Copyright (C) 1998-2000 ATI Technologies Inc.
//

#ifdef __cplusplus
extern "C" {
#endif

#ifndef APIENTRY
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

/*
** Notes:
**
**  Listed support is for current drivers and should really only be used
**  as a guideline.  ISV should still use glGetString() and 
**  wglGetExtensionsString() to determine the exact set of supported
**  GL and WGL extensions.
**
*/

/*
** WGL_ARB_extensions_string
**
**  Support:
**   Rage 128 * based : Supported
**   Radeon   * based : Supported
*/
#ifndef WGL_ARB_extensions_string
#define WGL_ARB_extensions_string 1

typedef const char * (WINAPI * PFNWGLGETEXTENSIONSSTRINGARBPROC) (HDC hDC);

#endif /* WGL_ARB_extensions_string */

/*
** WGL_ARB_pixel_format
**
**  Support:
**   Rage 128 * based : Supported
**   Radeon   * based : Supported
*/
#ifndef WGL_ARB_pixel_format
#define WGL_ARB_pixel_format 1

#define WGL_NUMBER_PIXEL_FORMATS_ARB		0x2000
#define WGL_DRAW_TO_WINDOW_ARB				0x2001
#define WGL_DRAW_TO_BITMAP_ARB				0x2002
#define WGL_ACCELERATION_ARB				0x2003
#define WGL_NEED_PALETTE_ARB				0x2004
#define	WGL_NEED_SYSTEM_PALETTE_ARB			0x2005
#define	WGL_SWAP_LAYER_BUFFERS_ARB			0x2006
#define	WGL_SWAP_METHOD_ARB					0x2007
#define	WGL_NUMBER_OVERLAYS_ARB				0x2008
#define	WGL_NUMBER_UNDERLAYS_ARB			0x2009
#define WGL_TRANSPARENT_ARB					0x200A
#define WGL_TRANSPARENT_RED_VALUE_ARB		0x2037
#define WGL_TRANSPARENT_GREEN_VALUE_ARB		0x2038
#define WGL_TRANSPARENT_BLUE_VALUE_ARB		0x2039
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB		0x203A
#define WGL_TRANSPARENT_INDEX_VALUE_ARB		0x203B
#define WGL_SHARE_DEPTH_ARB					0x200C
#define WGL_SHARE_STENCIL_ARB				0x200D
#define WGL_SHARE_ACCUM_ARB					0x200E
#define WGL_SUPPORT_GDI_ARB					0x200F
#define WGL_SUPPORT_OPENGL_ARB				0x2010
#define WGL_DOUBLE_BUFFER_ARB				0x2011
#define WGL_STEREO_ARB						0x2012
#define WGL_PIXEL_TYPE_ARB					0x2013
#define WGL_COLOR_BITS_ARB					0x2014
#define WGL_RED_BITS_ARB					0x2015
#define WGL_RED_SHIFT_ARB					0x2016
#define WGL_GREEN_BITS_ARB					0x2017
#define WGL_GREEN_SHIFT_ARB					0x2018
#define WGL_BLUE_BITS_ARB					0x2019
#define WGL_BLUE_SHIFT_ARB					0x201A
#define WGL_ALPHA_BITS_ARB					0x201B
#define WGL_ALPHA_SHIFT_ARB					0x201C
#define WGL_ACCUM_BITS_ARB					0x201D
#define WGL_ACCUM_RED_BITS_ARB				0x201E
#define WGL_ACCUM_GREEN_BITS_ARB			0x201F
#define WGL_ACCUM_BLUE_BITS_ARB				0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB			0x2021
#define WGL_DEPTH_BITS_ARB					0x2022
#define WGL_STENCIL_BITS_ARB				0x2023
#define WGL_AUX_BUFFERS_ARB					0x2024
#define WGL_NO_ACCELERATION_ARB				0x2025
#define WGL_GENERIC_ACCELERATION_ARB		0x2026
#define WGL_FULL_ACCELERATION_ARB			0x2027
#define WGL_SWAP_EXCHANGE_ARB				0x2028
#define WGL_SWAP_COPY_ARB					0x2029
#define WGL_SWAP_UNDEFINED_ARB				0x202A
#define WGL_TYPE_RGBA_ARB					0x202B
#define WGL_TYPE_COLORINDEX_ARB				0x202C


typedef BOOL (WINAPI * PFNWGLGETPIXELFORMATATTRIBIVARBPROC) (
                                        HDC hDC,
                                        int iPixelFormat,
                                        int iLayerPlane,
                                        UINT nAttributes,
                                        const int *piAttributes,
                                        int *piValues);
typedef BOOL (WINAPI * PFNWGLGETPIXELFORMATATTRIBFVARBPROC) (
                                        HDC hDC,
                                        int iPixelFormat,
                                        int iLayerPlane,
                                        UINT nAttributes,
                                        const int *piAttributes,
                                        FLOAT *pfValues);
typedef BOOL (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (
                                        HDC hDC,
                                        const int *piAttribIList,
                                        const FLOAT *pfAttribFList,
                                        UINT nMaxFormats,
                                        int *piFormats,
                                        UINT *nNumFormats);

#endif /* WGL_ARB_pixel_format */

/*
** WGL_ARB_make_current_read
**
**  Support:
**   Rage 128 * based : Supported
**   Radeon   * based : Supported
*/
#ifndef WGL_ARB_make_current_read
typedef BOOL (WINAPI * PFNWGLMAKECONTEXTCURRENTARBPROC) (
#define WGL_ARB_make_current_read 1

                                        HDC hDrawDC,
                                        HDC hReadDC,
                                        HGLRC hGLRC);
typedef HDC (WINAPI * PFNWGLGETCURRENTREADDCARBPROC) (VOID);

#endif /* WGL_ARB_make_current_read */

/*
** WGL_ARB_pbuffer
**
**  Support:
**   Rage 128 * based : Unsupported
**   Radeon   * based : Unsupported
*/
#ifndef WGL_ARB_pbuffer
#define WGL_ARB_pbuffer 1

#define WGL_DRAW_TO_PBUFFER_ARB              0x202D
#define WGL_MAX_PBUFFER_PIXELS_ARB           0x202E
#define WGL_MAX_PBUFFER_WIDTH_ARB            0x202F
#define WGL_MAX_PBUFFER_HEIGHT_ARB           0x2030
#define WGL_PBUFFER_LARGEST_ARB              0x2033
#define WGL_PBUFFER_WIDTH_ARB                0x2034
#define WGL_PBUFFER_HEIGHT_ARB               0x2035
#define WGL_PBUFFER_LOST_ARB				 0x2036

DECLARE_HANDLE(HPBUFFERARB);

typedef HPBUFFERARB (WINAPI * PFNWGLCREATEPBUFFERARBPROC) (
                                        HDC hDC,
                                        int iPixelFormat,
                                        int iWidth,
                                        int iHeight,
                                        const int *piAttribList);
typedef HDC (WINAPI * PFNWGLGETPBUFFERDCARBPROC) (HPBUFFERARB hPbuffer);
typedef int (WINAPI * PFNWGLRELEASEPBUFFERDCARBPROC) (
                                        HPBUFFERARB hPbuffer,
                                        HDC hDC);
typedef BOOL (WINAPI * PFNWGLDESTROYPBUFFERARBPROC) (HPBUFFERARB hPbuffer);
typedef BOOL (WINAPI * PFNWGLQUERYPBUFFERARBPROC) (
                                        HPBUFFERARB hPbuffer,
                                        int iAttribute,
                                        int *piValue);

#endif /* WGL_ARB_pbuffer */


/*
** WGL_EXT_extensions_string
**
**  Support:
**   Rage 128 * based : Supported
**   Radeon   * based : Supported
*/
#ifndef WGL_EXT_extensions_string
#define WGL_EXT_extensions_string 1

typedef const char * (WINAPI * PFNWGLGETEXTENSIONSSTRINGEXTPROC) (VOID);

#endif /* WGL_EXT_extensions_string */

/*
** WGL_EXT_swap_control
**
**  Support:
**   Rage 128 * based : Supported
**   Radeon   * based : Supported
*/
#ifndef WGL_EXT_swap_control
#define WGL_EXT_swap_control 1

typedef BOOL (WINAPI * PFNWGLSWAPINTERVALEXTPROC) (int interval);
typedef int (WINAPI * PFNWGLGETSWAPINTERVALEXTPROC) (VOID);

#endif /* WGL_EXT_swap_control */

#ifdef __cplusplus
}
#endif

#endif /* __wgl_ATI_h_ */
