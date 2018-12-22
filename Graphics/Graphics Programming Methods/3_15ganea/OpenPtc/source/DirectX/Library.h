//
// DirectX Library class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_DIRECTX_LIBRARY_H
#define __PTC_DIRECTX_LIBRARY_H

// include files
#include <ddraw.h>
#include "config.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class DirectXLibrary
{
    public:

        // setup
        DirectXLibrary();
        ~DirectXLibrary();

        // data access
        LPDIRECTDRAW lpDD() const;
        LPDIRECTDRAW2 lpDD2() const;

        // close
        void close();

    private:

        // library data
        HMODULE m_library;
        LPDIRECTDRAW m_lpDD;
        LPDIRECTDRAW2 m_lpDD2;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
