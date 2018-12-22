//
// C Extra routines for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "C/Extra.h"
#include "C/Error.h"
#include "Dynamic/Error.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




void PTC_CALLBACK ptc_use_exceptions_handler(PTC_ERROR error)
{
    // throw dynamic error exception
    throw DynamicError(ptc_error_message(error));
}




CAPI void PTCAPI ptc_use_exceptions()
{
    // set use exceptions handler
    ptc_error_handler(ptc_use_exceptions_handler);
}
