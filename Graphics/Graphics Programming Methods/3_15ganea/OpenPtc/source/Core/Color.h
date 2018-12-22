//
// Color class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_COLOR_H
#define __PTC_COLOR_H

// include files
#include "config.h"
#include "Core/Types.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class Color
{
    public:

        // setup
        PTCAPI Color();
        PTCAPI Color(int index);
        PTCAPI Color(float r,float g,float b,float a=1.0f);
        PTCAPI Color(const Color &color);
        PTCAPI ~Color();

        // data access
        int PTCAPI index() const;
        float PTCAPI r() const;
        float PTCAPI g() const;
        float PTCAPI b() const;
        float PTCAPI a() const;
        bool PTCAPI direct() const;
        bool PTCAPI indexed() const;

        // operators
        Color& PTCAPI operator =(const Color &color);
        bool PTCAPI operator ==(const Color &color) const;
        bool PTCAPI operator !=(const Color &color) const;

    private:

        // data
        int m_index;
        float m_r;
        float m_g;
        float m_b;
        float m_a;
        bool m_direct;
        bool m_indexed;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
