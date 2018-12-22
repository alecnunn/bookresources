/*
 * Include file for OpenPTC 1.0 C++ Implementation
 * Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
 * This source code is licensed under the GNU Lesser GPL
 * See http://www.gnu.org/copyleft/lesser.html for details
 */

#ifndef __PTC_H
#define __PTC_H




/* configuration */
#include "config.h"


#ifdef __cplusplus
    

    #ifdef __COMPILER_SUPPORTS_NAMESPACES__

        // include ptc
        #include "ptc"

        // using directive
        using namespace ptc;

    #else

        // integral types
        #include "Core/Types.h"

        #ifdef __PTC_DYNAMIC__

            // dynamic classes
            #include "Dynamic/Key.h"
            #include "Dynamic/Area.h"
            #include "Dynamic/Base.h"
            #include "Dynamic/Copy.h"
            #include "Dynamic/Mode.h"
            #include "Dynamic/Clear.h"
            #include "Dynamic/Color.h"
            #include "Dynamic/Error.h"
            #include "Dynamic/Timer.h"
            #include "Dynamic/Format.h"
            #include "Dynamic/Clipper.h"
            #include "Dynamic/Palette.h"
            #include "Dynamic/Surface.h"
            #include "Dynamic/Console.h"

            // alias classes
            typedef DynamicKey Key;
            typedef DynamicArea Area;
            typedef DynamicCopy Copy;
            typedef DynamicMode Mode;
            typedef DynamicClear Clear;
            typedef DynamicColor Color;
            typedef DynamicError Error;
            typedef DynamicTimer Timer;
            typedef DynamicFormat Format;
            typedef DynamicClipper Clipper;
            typedef DynamicPalette Palette;
            typedef DynamicSurface Surface;
            typedef DynamicConsole Console;
            typedef DynamicBaseSurface BaseSurface;
            typedef DynamicBaseConsole BaseConsole;
    
        #else

            // core classes
            #include "Core/Key.h"
            #include "Core/Area.h"
            #include "Core/Base.h"
            #include "Core/Copy.h"
            #include "Core/Mode.h"
            #include "Core/Clear.h"
            #include "Core/Color.h"
            #include "Core/Error.h"
            #include "Core/Timer.h"
            #include "Core/Format.h"
            #include "Core/Clipper.h"
            #include "Core/Palette.h"
            #include "Core/Surface.h"
            #include "Core/Console.h"

        #endif

    #endif

#else

    /* integral types */
    #include "C/Types.h"

    /* public classes */
    #include "C/Key.h"
    #include "C/Area.h"
    #include "C/Copy.h"
    #include "C/Mode.h"
    #include "C/Clear.h"
    #include "C/Color.h"
    #include "C/Error.h"
    #include "C/Timer.h"
    #include "C/Format.h"
    #include "C/Clipper.h"
    #include "C/Palette.h"
    #include "C/Surface.h"
    #include "C/Console.h"

    /* extra functions */
    #include "C/Extra.h"

#endif




#endif
