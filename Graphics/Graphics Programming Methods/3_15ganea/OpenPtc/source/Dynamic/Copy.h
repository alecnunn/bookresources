//
// Dynamic Copy class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org) 
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_DYNAMIC_COPY_H
#define __PTC_DYNAMIC_COPY_H

// include files
#include "config.h"
#include "C/Copy.h"
#include "C/Extra.h"
#include "Core/Types.h"
#include "Dynamic/Index.h"
#include "Dynamic/Format.h"
#include "Dynamic/Palette.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class DynamicCopy
{ 
    public:

        DynamicCopy()
        {
            // use exceptions
            ptc_use_exceptions();

            // create copy object
            object = ptc_copy_create();
        }

        ~DynamicCopy()
        {
            // destroy copy object
            ptc_copy_destroy(object);
        }
        
        void request(const DynamicFormat &source,const DynamicFormat &destination)
        {
            // request formats
            ptc_copy_request(object,source.object,destination.object);
        }
        
        void palette(const DynamicPalette &source,const DynamicPalette &destination)
        {
            // set palettes
            ptc_copy_palette(object,source.object,destination.object);
        }
        
        void copy(const void *source_pixels,int source_x,int source_y,int source_width,int source_height,int source_pitch,void *destination_pixels,int destination_x,int destination_y,int destination_width,int destination_height,int destination_pitch)
        {
            // copy pixels
            ptc_copy_copy(object,(void*)source_pixels,source_x,source_y,source_width,source_height,source_pitch,(void*)destination_pixels,destination_x,destination_y,destination_width,destination_height,destination_pitch);
        }
        
        bool option(const char option[])
        {
            // set copy option
            return (ptc_copy_option(object,(char*)option)==1);
        }
        
    private:

        // disallow copy construction
        DynamicCopy(const DynamicCopy &copy);

        // disallow assignment
        DynamicCopy& operator =(const DynamicCopy &copy);

        // copy object
        PTC_COPY object;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
