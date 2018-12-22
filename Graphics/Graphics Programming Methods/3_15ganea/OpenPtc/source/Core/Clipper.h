//
// Clipper class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_CLIPPER_H
#define __PTC_CLIPPER_H

// include files
#include "config.h"
#include "Core/Index.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class Clipper
{
    public:

        // clip a single area against clip area
        static Area PTCAPI clip(const Area &area,const Area &clip);

        // clip source and destination areas against source and destination clip areas
        static void PTCAPI clip(const Area &source,const Area &clip_source,Area &clipped_source,
                                       const Area &destination,const Area &clip_destination,Area &clipped_destination);

    private:

        // clip floating point area against clip area
        static inline void clip(float &left,float &top,float &right,float &bottom,const Area &clip);

        // clip floating point area against a floating point clip area
        static inline void clip(float &left,float &top,float &right,float &bottom,
                                float left_clip,float top_clip,float right_clip,float bottom_clip);

        // snap a floating point area to integer coordinates
        static inline void round(float &left,float &top,float &right,float &bottom);
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
