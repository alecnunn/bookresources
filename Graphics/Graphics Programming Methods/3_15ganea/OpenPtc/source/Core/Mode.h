//
// Mode class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_MODE_H
#define __PTC_MODE_H

// include files
#include "config.h"
#include "Core/Types.h"
#include "Core/Format.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class Mode
{
    public:

        // setup
        PTCAPI Mode();
        PTCAPI Mode(int width,int height,const Format &format);
        PTCAPI Mode(const Mode &mode);
        PTCAPI ~Mode();

        // valid mode flag
        bool PTCAPI valid() const;

        // data access
        int PTCAPI width() const;
        int PTCAPI height() const;
        const Format& PTCAPI format() const;

        // operators
        Mode& PTCAPI operator =(const Mode &mode);
        bool PTCAPI operator ==(const Mode &mode) const;
        bool PTCAPI operator !=(const Mode &mode) const;

    private:

        // debug check
        void check() const;

        // data
        bool m_valid;
        int m_width;
        int m_height;
        Format m_format;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
