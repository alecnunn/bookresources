//
// Dynamic Mode class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_DYNAMIC_MODE_H
#define __PTC_DYNAMIC_MODE_H

// include files
#include "config.h"
#include "C/Mode.h"
#include "C/Extra.h"
#include "Core/Types.h"
#include "Dynamic/Format.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class DynamicMode
{
    public:

        DynamicMode()
        {
            // use exceptions
            ptc_use_exceptions();

            // create mode object
            object = ptc_mode_create_invalid();
        }

        DynamicMode(int width,int height,const DynamicFormat &format)
        {
            // use exceptions
            ptc_use_exceptions();

            // create mode object
            object = ptc_mode_create(width,height,format.object);
        }

        DynamicMode(const DynamicMode &mode)
        {
            // use exceptions
            ptc_use_exceptions();

            // create mode object
            object = ptc_mode_create(ptc_mode_width(mode.object),ptc_mode_height(mode.object),ptc_mode_format(mode.object));
        }

        ~DynamicMode()
        {
            // destroy mode object
            ptc_mode_destroy(object);
        }

        bool valid() const
        {
            // is valid
            return (ptc_mode_valid(object)==1);
        }

        int width() const
        {
            // get width
            return ptc_mode_width(object);
        }

        int height() const
        {
            // get width
            return ptc_mode_height(object);
        }

        const DynamicFormat& format() const
        {
            // get format
            ptc_format_assign(kludge_format.object,ptc_mode_format(object));
            return kludge_format;
        }

        DynamicMode& operator =(const DynamicMode &mode)
        {
            // assignment operator
            ptc_mode_assign(object,mode.object);
            return *this;
        }

        bool operator ==(const DynamicMode &mode) const
        {
            // is equal to
            return (ptc_mode_equals(object,mode.object)==1);
        }

        bool operator !=(const DynamicMode &mode) const
        {
            // not equal to
            return (ptc_mode_equals(object,mode.object)==0);
        }
        
    private:

        // mode object
        PTC_MODE object;

        // kludge objects
        DynamicFormat kludge_format;

        // friend classes
        friend class DynamicConsole;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
