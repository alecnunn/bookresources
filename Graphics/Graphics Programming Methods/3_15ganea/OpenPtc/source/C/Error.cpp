//
// C Error class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "C/Error.h"
#include "C/Except.h"
#include "Core/Error.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




CAPI PTC_ERROR PTCAPI ptc_error_create(char *message)
{
    try
    {
        // create error object
        return (PTC_ERROR) new Error(message);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI PTC_ERROR PTCAPI ptc_error_create_composite(char *message,PTC_ERROR error)
{
    try
    {
        // create composite error object
        return (PTC_ERROR) new Error(message,*((Error*)error));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI void PTCAPI ptc_error_destroy(PTC_ERROR object)
{
    try
    {
        // destroy error object
        delete (Error*) object;
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void PTCAPI ptc_error_report(PTC_ERROR object)
{
    try
    {
        // report error
        ((Error*)object)->report();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI char* PTCAPI ptc_error_message(PTC_ERROR object)
{
    try
    {
        // get error message
        return (char*) ((Error*)object)->message();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}




CAPI void PTCAPI ptc_error_assign(PTC_ERROR object,PTC_ERROR error)
{
    try
    {
        // assign object = error
        *((Error*)object) = *((Error*)error);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI int PTCAPI ptc_error_equals(PTC_ERROR object,PTC_ERROR error)
{
    try
    {
        // test object == color
        return *((Error*)object) == *((Error*)error);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}




void PTCAPI ptc_error_handler(PTC_ERROR_HANDLER handler)
{
    // set exception handler
    ptc_exception_handler(handler);
}
