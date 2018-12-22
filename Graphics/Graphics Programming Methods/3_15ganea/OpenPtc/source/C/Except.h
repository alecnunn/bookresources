//
// C Exception handling routines for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_C_EXCEPTION_H
#define __PTC_C_EXCEPTION_H

// include files
#include "config.h"
#include "C/Error.h"
#include "Core/Error.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using ptc::Error;
#endif




// set exception handler
void ptc_exception_handler(PTC_ERROR_HANDLER handler);

// handle an exception
void ptc_exception_handle(Error &error);




#endif
