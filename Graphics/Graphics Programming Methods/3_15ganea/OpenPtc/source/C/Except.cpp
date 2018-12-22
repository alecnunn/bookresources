//
// C Exception handling routines for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "C/Except.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




void PTC_CALLBACK ptc_error_handler_default(PTC_ERROR error)
{
    // default exception handler
    ((Error*)error)->report();
}


// ptc error handler function
static PTC_ERROR_HANDLER ptc_error_handler_function = ptc_error_handler_default;


void ptc_exception_handler(PTC_ERROR_HANDLER handler)
{
    // check handler
    if (handler==0)
    {
        // set to default handler
        ptc_error_handler_function = ptc_error_handler_default;
    }
    else
    {
        // set to user specified handler
        ptc_error_handler_function = handler;
    }
}


void ptc_exception_handle(Error &error)
{
    // handle an exception
    ptc_error_handler_function((PTC_ERROR)&error);
}
