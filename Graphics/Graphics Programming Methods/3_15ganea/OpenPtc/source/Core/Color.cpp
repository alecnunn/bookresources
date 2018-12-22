//
// Color class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "Core/Color.h"
#include "Core/Error.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




PTCAPI Color::Color()
{
    // defaults
    m_index = 0;
    m_r = 0.0f;
    m_g = 0.0f;
    m_b = 0.0f;
    m_a = 1.0f;
    m_direct = false;
    m_indexed = false;
}


PTCAPI Color::Color(int index)
{
    // indexed color
    m_index = index;
    m_r = 0.0f;
    m_g = 0.0f;
    m_b = 0.0f;
    m_a = 1.0f;
    m_direct = false;
    m_indexed = true;
}


PTCAPI Color::Color(float r,float g,float b,float a)
{
    // direct color
    m_index = 0;
    m_r = r;
    m_g = g;
    m_b = b;
    m_a = a;
    m_direct = true;
    m_indexed = false;
}


PTCAPI Color::Color(const Color &color)
{
    // copy constructor
    *this = color;
}


PTCAPI Color::~Color()
{
    // destructor
}




int PTCAPI Color::index() const
{
    // get index
    return m_index;
}


float PTCAPI Color::r() const
{
    // get r
    return m_r;
}


float PTCAPI Color::g() const
{
    // get g
    return m_g;
}


float PTCAPI Color::b() const
{
    // get b
    return m_b;
}


float PTCAPI Color::a() const
{
    // get a
    return m_a;
}


bool PTCAPI Color::direct() const
{
    // get direct
    return m_direct;
}


bool PTCAPI Color::indexed() const
{
    // get indexed
    return m_indexed;
}




Color& PTCAPI Color::operator =(const Color &color)
{
    // check for self assignment
    if (this==&color) throw Error("self assignment is not allowed");
        
    // assignment
    m_index = color.m_index;
    m_r = color.m_r;
    m_g = color.m_g;
    m_b = color.m_b;
    m_a = color.m_a;
    m_direct = color.m_direct;
    m_indexed = color.m_indexed;
    return *this;
}


bool PTCAPI Color::operator ==(const Color &color) const
{
    // equal to
    if (m_direct && color.m_direct)
    {
        // direct color
        if (m_r==color.m_r && 
            m_g==color.m_g && 
            m_b==color.m_b && 
            m_a==color.m_a) return true;
    }
    else if (m_indexed && color.m_indexed)
    {
        // indexed color
        if (m_index==color.m_index) return true;
    }

    // failure
    return false;
}


bool PTCAPI Color::operator !=(const Color &color) const
{
    // not equal to
    return !(*this==color);
}
