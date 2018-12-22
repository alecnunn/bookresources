//
// Dynamic Format class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_DYNAMIC_FORMAT_H
#define __PTC_DYNAMIC_FORMAT_H

// include files
#include "config.h"
#include "C/Extra.h"
#include "C/Format.h"
#include "Core/Types.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class DynamicFormat
{
    public:

        DynamicFormat()
        {
            // use exceptions
            ptc_use_exceptions();

            // create format object
            object = ptc_format_create();
        }

        DynamicFormat(int bits)
        {
            // use exceptions
            ptc_use_exceptions();

            // create format object
            object = ptc_format_create_indexed(bits);
        }

        DynamicFormat(int bits,int32 r,int32 g,int32 b,int32 a=0)
        {
            // use exceptions
            ptc_use_exceptions();

            // create format object
            object = ptc_format_create_direct(bits,r,g,b,a);
        }

        DynamicFormat(const DynamicFormat &format)
        {
            // use exceptions
            ptc_use_exceptions();

            // create format object
            object = ptc_format_create();

            // assign format data
            ptc_format_assign(object,format.object);
        }

        ~DynamicFormat()
        {
            // destroy format object
            ptc_format_destroy(object);
        }

        int32 r() const
        {
            // get r
            return ptc_format_r(object);
        }

        int32 g() const
        {
            // get g
            return ptc_format_g(object);
        }

        int32 b() const
        {
            // get b
            return ptc_format_b(object);
        }

        int32 a() const
        {
            // get a
            return ptc_format_a(object);
        }

        int bits() const
        {
            // get bits
            return ptc_format_bits(object);
        }

        int bytes() const
        {
            // get bytes
            return ptc_format_bytes(object);
        }

        bool direct() const
        {
            // is direct
            return (ptc_format_direct(object)==1);
        }

        bool indexed() const
        {
            // is indexed
            return (ptc_format_indexed(object)==1);
        }

        DynamicFormat& operator =(const DynamicFormat &format)
        {
            // assignment operator
            ptc_format_assign(object,format.object);
            return *this;
        }

        bool operator ==(const DynamicFormat &format) const
        {
            // is equal to
            return (ptc_format_equals(object,format.object)==1);
        }

        bool operator !=(const DynamicFormat &format) const
        {
            // not equal to
            return (ptc_format_equals(object,format.object)==0);
        }

    private:

        // format object
        PTC_FORMAT object;

        // friend classes
        friend class DynamicCopy;
        friend class DynamicMode;
        friend class DynamicClear;
        friend class DynamicSurface;
        friend class DynamicConsole;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
