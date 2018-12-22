//
// DirectX Translate functions for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//                    

#ifndef __PTC_DIRECTX_TRANSLATE_H
#define __PTC_DIRECTX_TRANSLATE_H

// include files
#include <ddraw.h>
#include "config.h"
#include "Core/Format.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




// DirectX translate functions
Format DirectXTranslate(DDPIXELFORMAT const &ddpf);




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
