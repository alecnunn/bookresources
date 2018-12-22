//
// DirectX Check function for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//                    

#ifndef __PTC_DIRECTX_CHECK_H
#define __PTC_DIRECTX_CHECK_H

// include files
#include <ddraw.h>
#include "config.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




// DirectX checking function
void DirectXCheck(HRESULT result);
void DirectXCheck(HRESULT result,const char message[]);




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
