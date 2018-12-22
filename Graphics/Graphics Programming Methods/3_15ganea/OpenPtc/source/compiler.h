/*
 * Compiler specifics for OpenPTC 1.0 C++ Implementation
 * Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
 * This source code is licensed under the GNU Lesser GPL
 * See http://www.gnu.org/copyleft/lesser.html for details
 */

#ifndef __PTC_COMPILER_H
#define __PTC_COMPILER_H




#if (_MSC_VER>=1100)


    /* Visual C++ 5.0 and greater */

    /* compiler has boolean type support */
    #define __COMPILER_SUPPORTS_BOOL__

    /* compiler has const_cast support */
    #define __COMPILER_SUPPORTS_CONST_CAST__

    /* compiler has namespace support */
    #define __COMPILER_SUPPORTS_NAMESPACES__

    /* compiler supports ddraw */
    #define __COMPILER_SUPPORTS_DDRAW__

    /* ptc api convention */
    #define PTCAPI __stdcall

    /* callback convention */
    #define PTC_CALLBACK __cdecl


#elif (_MSC_VER>=1000)


    /* Visual C++ 4.x */

    /* compiler supports ddraw */
    #define __COMPILER_SUPPORTS_DDRAW__

    /* ptc api convention */
    #define PTCAPI __stdcall

    /* callback convention */
    #define PTC_CALLBACK __cdecl


#elif (__WATCOMC__>=1100)


    /* Watcom C++ 11.0 and greater

    /* compiler has boolean type support */
    #define __COMPILER_SUPPORTS_BOOL__

    /* compiler supports ddraw */
    #define __COMPILER_SUPPORTS_DDRAW__


#elif (__WATCOMC__>=900)


    /* Watcom C++ 10.0, 10.5 and 10.6 */


#elif defined (__CYGWIN32__)


    /*  Cygnus GNU-Win32 (Cygwin) */

    /* compiler is gnu */
    #define __COMPILER_IS_GNU__

    /* compiler has boolean type support */
    #define __COMPILER_SUPPORTS_BOOL__

    /* c api convention */
    #ifdef __cplusplus
    #define CAPI extern "C" __stdcall
    #else
    #define CAPI __stdcall
    #endif

    /* ptc api convention */
    #define PTCAPI

    /* callback convention */
    #define PTC_CALLBACK __cdecl


#else


    /* Unknown Compiler */

    /* define this if the compiler is based on gnu c/c++ */
    #define __COMPILER_IS_GNU__

    /* define this if the compiler has boolean type support */
    #define __COMPILER_SUPPORTS_BOOL__

    /* define this if the compiler has const_cast support */
    #define __COMPILER_SUPPORTS_CONST_CAST__

    /* define this if the compiler has namespace support */
    #define __COMPILER_SUPPORTS_NAMESPACES__

    /* define this if the compiler supports ddraw */
    #define __COMPILER_SUPPORTS_DDRAW__


#endif




/* check compiler bool type support */
#ifndef __COMPILER_SUPPORTS_BOOL__

    /* emulate 'bool' */
    typedef int bool;
    #define true 1
    #define false 0

#endif


/* check compiler ddraw support */
#ifndef __COMPILER_SUPPORTS_DDRAW__

    /* emulate ddraw types */
    #define LPDIRECTDRAW void*
    #define LPDIRECTDRAW2 void*
    #define LPDIRECTDRAWSURFACE void*

#endif

        


/* ptc api convention */
#ifndef PTCAPI
#include <windows.h>
#define PTCAPI WINAPI
#endif


/* c api convention */
#ifndef CAPI
#ifdef __cplusplus
#define CAPI extern "C"
#else
#define CAPI
#endif
#endif

        
/* callback convention */
#ifndef PTC_CALLBACK
#define PTC_CALLBACK
#endif


/* debug build */
#ifndef NDEBUG
#define __DEBUG__
#endif




#endif
