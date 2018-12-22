//
// Type definitions for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_TYPES_H
#define __PTC_TYPES_H

// include files
#include "config.h"



// use hermes types
#include "Hermes/H_Types.h"


#if defined(__COMPILER_SUPPORTS_NAMESPACES__) && defined(__cplusplus)

    // alias types
    namespace ptc
    {
        typedef ::hermes_int32 int32;
        typedef ::hermes_short16 short16;
        typedef ::hermes_char8 char8;
    }

#else

    // alias types
    typedef hermes_int32 int32;
    typedef hermes_short16 short16;
    typedef hermes_char8 char8;

#endif




#endif
