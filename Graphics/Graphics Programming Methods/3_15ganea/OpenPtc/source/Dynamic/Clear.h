//
// Dynamic Clear class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_DYNAMIC_CLEAR_H
#define __PTC_DYNAMIC_CLEAR_H

// include files
#include "config.h"
#include "C/Clear.h"
#include "C/Extra.h"
#include "Core/Types.h"
#include "Dynamic/Index.h"
#include "Dynamic/Color.h"
#include "Dynamic/Format.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class DynamicClear
{
    public:

        DynamicClear()
        {
            // use exceptions
            ptc_use_exceptions();

            // create clear object
            object = ptc_clear_create();
        }

        ~DynamicClear()
        {
            // destroy clear object
            ptc_clear_destroy(object);
        }

        void request(const DynamicFormat &format)
        {
            // request clear
            ptc_clear_request(object,format.object);
        }

        void clear(void *pixels,int x,int y,int width,int height,int pitch,const DynamicColor &color)
        {
            // clear pixels
            ptc_clear_clear(object,pixels,x,y,width,height,pitch,color.object);
        }

    private:

        // disallow copy construction
        DynamicClear(const DynamicClear &clear);

        // disallow assignment
        DynamicClear& operator =(const DynamicClear &clear);

        // clear object
        PTC_CLEAR object;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
