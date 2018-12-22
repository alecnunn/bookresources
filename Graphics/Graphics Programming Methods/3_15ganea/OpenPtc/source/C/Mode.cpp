//
// C Mode class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "C/Mode.h"
#include "C/Except.h"
#include "Core/Mode.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




CAPI PTC_MODE PTCAPI ptc_mode_create(int width,int height,PTC_FORMAT format)
{
    try
    {
        // create mode object
        return (PTC_MODE) new Mode(width,height,*((Format*)format));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI PTC_MODE PTCAPI ptc_mode_create_invalid()
{
    try
    {
        // create invalid mode object
        return (PTC_MODE) new Mode();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI void PTCAPI ptc_mode_destroy(PTC_MODE object)
{
    try
    {
        // destroy mode object
        delete (Mode*) object;
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI int PTCAPI ptc_mode_valid(PTC_MODE object)
{
    try
    {
        // is mode valid?
        return ((Mode*)object)->valid();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}




CAPI int PTCAPI ptc_mode_width(PTC_MODE object)
{
    try
    {
        // get mode width
        return ((Mode*)object)->width();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI int PTCAPI ptc_mode_height(PTC_MODE object)
{
    try
    {
        // get mode height
        return ((Mode*)object)->height();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI PTC_FORMAT PTCAPI ptc_mode_format(PTC_MODE object)
{
    try
    {
        // get mode format
        return (PTC_FORMAT) & ((Mode*)object)->format();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}




CAPI void PTCAPI ptc_mode_assign(PTC_MODE object,PTC_MODE mode)
{
    try
    {
        // assign object = mode
        *((Mode*)object) = *((Mode*)mode);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI int PTCAPI ptc_mode_equals(PTC_MODE object,PTC_MODE mode)
{
    try
    {
        // test object == mode
        return *((Mode*)object) == *((Mode*)mode);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}
