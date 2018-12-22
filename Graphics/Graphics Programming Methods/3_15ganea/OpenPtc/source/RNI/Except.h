//
// RNI Exception routines for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __RNI_EXCEPT_H
#define __RNI_EXCEPT_H

// include files
#include "RNI/Library.h"
#include "ptc.h"




// throw an rni java exception
void rni_throw(const Error &error = Error("unknown rni exception"));




#endif
