//
// Dynamic Color class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_DYNAMIC_COLOR_H
#define __PTC_DYNAMIC_COLOR_H

// include files
#include "config.h"
#include "C/Color.h"
#include "C/Extra.h"
#include "Core/Types.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class DynamicColor
{
    public:

        DynamicColor()
        {
            // use exceptions
            ptc_use_exceptions();

            // create color object
            object = ptc_color_create();
        }

        DynamicColor(int index)
        {
            // use exceptions
            ptc_use_exceptions();

            // create color object
            object = ptc_color_create_indexed(index);
        }

        DynamicColor(float r,float g,float b,float a=1.0f)
        {
            // use exceptions
            ptc_use_exceptions();

            // create color object
            object = ptc_color_create_direct(r,g,b,a);
        }

        DynamicColor(const DynamicColor &color)
        {
            // use exceptions
            ptc_use_exceptions();

            // create color object
            object = ptc_color_create();

            // assign color data
            ptc_color_assign(object,color.object);
        }

        ~DynamicColor()
        {
            // destroy color object
            ptc_color_destroy(object);
        }

        int index() const
        {
            // get index
            return ptc_color_index(object);
        }

        float r() const
        {
            // get r
            return ptc_color_r(object);
        }

        float g() const
        {
            // get g
            return ptc_color_g(object);
        }

        float b() const
        {
            // get b
            return ptc_color_b(object);
        }

        float a() const
        {
            // get a
            return ptc_color_a(object);
        }

        bool direct() const
        {
            // is direct
            return (ptc_color_direct(object)==1);
        }

        bool indexed() const
        {
            // is indexed
            return (ptc_color_indexed(object)==1);
        }

        DynamicColor& operator =(const DynamicColor &color)
        {
            // assignment operator
            ptc_color_assign(object,color.object);
            return *this;
        }

        bool operator ==(const DynamicColor &color) const
        {
            // is equal to
            return (ptc_color_equals(object,color.object)==1);
        }

        bool operator !=(const DynamicColor &color) const
        {
            // not equal to
            return (ptc_color_equals(object,color.object)==0);
        }

    private:

        // color object
        PTC_COLOR object;

        // friend classes
        friend class DynamicClear;
        friend class DynamicSurface;
        friend class DynamicConsole;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
