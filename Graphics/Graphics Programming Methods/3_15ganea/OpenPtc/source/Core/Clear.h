//
// Clear class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_CLEAR_H
#define __PTC_CLEAR_H

// include files
#include "config.h"
#include "Core/Index.h"
#include "Core/Types.h"
#include "Core/Hermes.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class Clear
{
    public:

        // setup
        PTCAPI Clear();
        PTCAPI ~Clear();

        // request clear
        void PTCAPI request(const Format &format);

        // clear pixels
        void PTCAPI clear(void *pixels,int x,int y,int width,int height,int pitch,const Color &color);

    private:

        // disallow copy construction
        Clear(const Clear &clear);

        // disallow assignment
        Clear& operator =(const Clear &clear);

        // hermes clear handle
        HermesHandle m_handle;

        // current format
        Format *m_format;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
