//
// Mode class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "Core/Mode.h"
#include "Core/Error.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




PTCAPI Mode::Mode()
{
    // defaults
    m_valid = false;
    m_width = 0;
    m_height = 0;
}


PTCAPI Mode::Mode(int width,int height,const Format &format)
{
    // setup data
    m_valid = true;
    m_width = width;
    m_height = height;
    m_format = format;
}


PTCAPI Mode::Mode(const Mode &mode)
{
    // assignment
    *this = mode;
}


PTCAPI Mode::~Mode()
{
    // destructor
}




bool PTCAPI Mode::valid() const
{
    // get valid flag
    return m_valid;
}




int PTCAPI Mode::width() const
{
    // check
    check();

    // get width
    return m_width;
}


int PTCAPI Mode::height() const
{
    // check
    check();

    // get height
    return m_height;
}


const Format& PTCAPI Mode::format() const
{
    // check
    check();

    // get format
    return m_format;
}




Mode& PTCAPI Mode::operator =(const Mode &mode)
{
    // check for self assignment
    if (this==&mode) throw Error("self assignment is not allowed");
        
    // assignment
    m_valid = mode.m_valid;
    m_width = mode.m_width;
    m_height = mode.m_height;
    m_format = mode.m_format;
    return *this;
}


bool PTCAPI Mode::operator ==(const Mode &mode) const
{
    // equal to
    if (m_valid==mode.m_valid && 
        m_width==mode.m_width && 
        m_height==mode.m_height && 
        m_format==mode.m_format) return true;

    // failure
    return false;
}


bool PTCAPI Mode::operator !=(const Mode &mode) const
{
    // not equal to
    return !(*this==mode);
}




void Mode::check() const
{
    #ifdef __DEBUG__

        // check that mode is valid
        if (!m_valid) throw Error("mode is not valid");

    #else

        // no checking in release build

    #endif
}
