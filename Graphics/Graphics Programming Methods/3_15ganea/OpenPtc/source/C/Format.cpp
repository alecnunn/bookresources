//
// C Format class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "C/Format.h"
#include "C/Except.h"
#include "Core/Format.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




CAPI PTC_FORMAT PTCAPI ptc_format_create()
{
    try
    {
        // create format object
        return (PTC_FORMAT) new Format();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI PTC_FORMAT PTCAPI ptc_format_create_indexed(int bits)
{
    try
    {
        // create format object
        return (PTC_FORMAT) new Format(bits);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI PTC_FORMAT PTCAPI ptc_format_create_direct(int bits,ptc_int32 r,ptc_int32 g,ptc_int32 b,ptc_int32 a)
{
    try
    {
        // create format object
        return (PTC_FORMAT) new Format(bits,r,g,b,a);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI void PTCAPI ptc_format_destroy(PTC_FORMAT object)
{
    try
    {
        // destroy format object
        delete (Format*) object;
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI ptc_int32 PTCAPI ptc_format_r(PTC_FORMAT object)
{
    try
    {
        // get r
        return ((Format*)object)->r();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI ptc_int32 PTCAPI ptc_format_g(PTC_FORMAT object)
{
    try
    {
        // get g
        return ((Format*)object)->g();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI ptc_int32 PTCAPI ptc_format_b(PTC_FORMAT object)
{
    try
    {
        // get b
        return ((Format*)object)->b();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI ptc_int32 PTCAPI ptc_format_a(PTC_FORMAT object)
{
    try
    {
        // get a
        return ((Format*)object)->a();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI int PTCAPI ptc_format_bits(PTC_FORMAT object)
{
    try
    {
        // get bits
        return ((Format*)object)->bits();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI int PTCAPI ptc_format_bytes(PTC_FORMAT object)
{
    try
    {
        // get bytes
        return ((Format*)object)->bytes();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI int PTCAPI ptc_format_direct(PTC_FORMAT object)
{
    try
    {
        // get direct
        return (int) ((Format*)object)->direct();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI int PTCAPI ptc_format_indexed(PTC_FORMAT object)
{
    try
    {
        // get indexed
        return (int) ((Format*)object)->indexed();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}




CAPI void PTCAPI ptc_format_assign(PTC_FORMAT object,PTC_FORMAT format)
{
    try
    {
        // assign object = format
        *((Format*)object) = *((Format*)format);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI int PTCAPI ptc_format_equals(PTC_FORMAT object,PTC_FORMAT format)
{
    try
    {
        // test object == color
        return *((Format*)object) == *((Format*)format);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}
