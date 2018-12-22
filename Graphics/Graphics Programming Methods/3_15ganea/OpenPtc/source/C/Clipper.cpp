//
// C Clipper class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "C/Area.h"
#include "C/Except.h"
#include "C/Clipper.h"
#include "Core/Area.h"
#include "Core/Clipper.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




CAPI void PTCAPI ptc_clipper_clip(PTC_AREA area,PTC_AREA clip,PTC_AREA clipped)
{
    try
    {
        // clip area
        *((Area*)clipped) = Clipper::clip(*((Area*)area),*((Area*)clip));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void PTCAPI ptc_clipper_clip_complex(PTC_AREA source,PTC_AREA clip_source,PTC_AREA clipped_source,PTC_AREA destination,PTC_AREA clip_destination,PTC_AREA clipped_destination)
{
    try
    {
        // clip complex area
        Clipper::clip(*((Area*)source),*((Area*)clip_source),*((Area*)clipped_source),*((Area*)destination),*((Area*)clip_destination),*((Area*)clipped_destination));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}
