//
// Copy class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org) 
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_COPY_H
#define __PTC_COPY_H

// include files
#include "config.h"
#include "Core/Types.h"
#include "Core/Index.h"
#include "Core/Hermes.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class Copy
{ 
    public:

        // setup
        PTCAPI Copy();
        PTCAPI ~Copy();

        // set source and destination formats
        void PTCAPI request(const Format &source,const Format &destination);

        // set source and destination palettes
        void PTCAPI palette(const Palette &source,const Palette &destination);

        // copy pixels
        void PTCAPI copy(const void *source_pixels,int source_x,int source_y,int source_width,int source_height,int source_pitch,
                         void *destination_pixels,int destination_x,int destination_y,int destination_width,int destination_height,int destination_pitch);

        // copy option string
        bool PTCAPI option(const char option[]);

    private:

        // update handle
        void update();

        // disallow copy construction
        Copy(const Copy &copy);

        // disallow assignment
        Copy& operator =(const Copy &copy);

        // hermes handle
        HermesHandle m_handle;

        // internal flags
        long m_flags;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
