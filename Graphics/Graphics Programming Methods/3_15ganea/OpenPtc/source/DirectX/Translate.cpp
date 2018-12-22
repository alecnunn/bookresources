//
// DirectX Translate functions for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//                    

// include files
#include "Core/Error.h"
#include "DirectX/Translate.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




Format DirectXTranslate(DDPIXELFORMAT const &ddpf)
{
    // pixel format
    Format format;

    // check rgb data
    if (ddpf.dwFlags & DDPF_PALETTEINDEXED8)
    {
        // indexed color
        format = Format(8);
    }
    else if (ddpf.dwFlags & DDPF_RGB)
    {
        // direct color
        format = Format(ddpf.dwRGBBitCount,ddpf.dwRBitMask,ddpf.dwGBitMask,ddpf.dwBBitMask);
    }
    else
    {
        // error message
        throw Error("invalid pixel format");
    }

    // note: i should probably add alpha component support...

    // return format
    return format;
}




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif
