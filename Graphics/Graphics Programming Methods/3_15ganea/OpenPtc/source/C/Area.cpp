//
// C Area class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "C/Area.h"
#include "C/Except.h"
#include "Core/Area.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




CAPI PTC_AREA PTCAPI ptc_area_create(int left,int top,int right,int bottom)
{
    try
    {
        // create area object
        return (PTC_AREA) new Area(left,top,right,bottom);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI void PTCAPI ptc_area_destroy(PTC_AREA object)
{
    try
    {
        // destroy area object
        delete (Area*) object;
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI int PTCAPI ptc_area_left(PTC_AREA object)
{
    try
    {
        // get left
        return ((Area*)object)->left();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI int PTCAPI ptc_area_right(PTC_AREA object)
{
    try
    {
        // get right
        return ((Area*)object)->right();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI int PTCAPI ptc_area_top(PTC_AREA object)
{
    try
    {
        // get top
        return ((Area*)object)->top();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI int PTCAPI ptc_area_bottom(PTC_AREA object)
{
    try
    {
        // get bottom
        return ((Area*)object)->bottom();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI int PTCAPI ptc_area_width(PTC_AREA object)
{
    try
    {
        // get width
        return ((Area*)object)->width();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI int PTCAPI ptc_area_height(PTC_AREA object)
{
    try
    {
        // get height
        return ((Area*)object)->height();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}




CAPI void PTCAPI ptc_area_assign(PTC_AREA object,PTC_AREA area)
{
    try
    {
        // assign object = area
        *((Area*)object) = *((Area*)area);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI int PTCAPI ptc_area_equals(PTC_AREA object,PTC_AREA area)
{
    try
    {
        // test object == area
        return *((Area*)object) == *((Area*)area);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}
