//
// Format class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_FORMAT_H
#define __PTC_FORMAT_H

// include files
#include "config.h"
#include "Core/Copy.h"
#include "Core/Types.h"
#include "Core/Clear.h"
#include "Core/Hermes.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class Format
{
    public:

        // setup
        PTCAPI Format();
        PTCAPI Format(int bits);
        PTCAPI Format(int bits,int32 r,int32 g,int32 b,int32 a=0);
        PTCAPI Format(const Format &format);
        PTCAPI ~Format();

        // data access
        int32 PTCAPI r() const;
        int32 PTCAPI g() const;
        int32 PTCAPI b() const;
        int32 PTCAPI a() const;
        int PTCAPI bits() const;
        int PTCAPI bytes() const;
        bool PTCAPI direct() const;
        bool PTCAPI indexed() const;

        // operators
        Format& PTCAPI operator =(const Format &format);
        bool PTCAPI operator ==(const Format &format) const;
        bool PTCAPI operator !=(const Format &format) const;

    private:

        // hermes format
        HermesFormat m_format;

        // friend functions
        friend void Clear::request(const Format &format);
        friend void Copy::request(const Format &source,const Format &destination);
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
