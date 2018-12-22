//
// Dynamic Clipper class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_DYNAMIC_CLIPPER_H
#define __PTC_DYNAMIC_CLIPPER_H

// include files
#include "config.h"
#include "C/Extra.h"
#include "C/Clipper.h"
#include "Dynamic/Index.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class DynamicClipper
{
    public:

        static DynamicArea PTCAPI clip(const DynamicArea &area,const DynamicArea &clip)
        {
            // clipped area
            DynamicArea clipped;

            // clip
            ptc_clipper_clip(area.object,clip.object,clipped.object);

            // return clipped
            return clipped;
        }

        static void PTCAPI clip(const DynamicArea &source,const DynamicArea &clip_source,DynamicArea &clipped_source,const DynamicArea &destination,const DynamicArea &clip_destination,DynamicArea &clipped_destination)
        {
            // clip complex
            ptc_clipper_clip_complex(source.object,clip_source.object,clipped_source.object,destination.object,clip_destination.object,clipped_destination.object);
        }
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
