//
// C Color class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "C/Color.h"
#include "C/Except.h"
#include "Core/Color.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif

      


CAPI PTC_COLOR PTCAPI ptc_color_create()
{
    try
    {
        // create color object
        return (PTC_COLOR) new Color();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI PTC_COLOR PTCAPI ptc_color_create_indexed(int index)
{
    try
    {
        // create color object
        return (PTC_COLOR) new Color(index);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI PTC_COLOR PTCAPI ptc_color_create_direct(float r,float g,float b,float a)
{
    try
    {
        // create color object
        return (PTC_COLOR) new Color(r,g,b,a);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI void PTCAPI ptc_color_destroy(PTC_COLOR object)
{
    try
    {
        // destroy color object
        delete (Color*) object;
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI int PTCAPI ptc_color_index(PTC_COLOR object)
{
    try
    {
        // get index
        return ((Color*)object)->index();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI float PTCAPI ptc_color_r(PTC_COLOR object)
{
    try
    {
        // get r
        return ((Color*)object)->r();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI float PTCAPI ptc_color_g(PTC_COLOR object)
{
    try
    {
        // get g
        return ((Color*)object)->g();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI float PTCAPI ptc_color_b(PTC_COLOR object)
{
    try
    {
        // get b
        return ((Color*)object)->b();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI float PTCAPI ptc_color_a(PTC_COLOR object)
{
    try
    {
        // get a
        return ((Color*)object)->a();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI int PTCAPI ptc_color_direct(PTC_COLOR object)
{
    try
    {
        // get direct
        return ((Color*)object)->direct();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI int PTCAPI ptc_color_indexed(PTC_COLOR object)
{
    try
    {
        // get indexed
        return ((Color*)object)->indexed();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}




CAPI void PTCAPI ptc_color_assign(PTC_COLOR object,PTC_COLOR color)
{
    try
    {
        // assign object = color
        *((Color*)object) = *((Color*)color);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI int PTCAPI ptc_color_equals(PTC_COLOR object,PTC_COLOR color)
{
    try
    {
        // test object == color
        return *((Color*)object) == *((Color*)color);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}
