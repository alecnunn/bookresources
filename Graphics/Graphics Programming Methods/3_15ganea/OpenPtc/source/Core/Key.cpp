//
// Key class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "Core/Key.h"
#include "Core/Error.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




PTCAPI Key::Key()
{
    // defaults
    m_code = UNDEFINED;
    m_alt = false;
    m_shift = false;
    m_control = false;
}


PTCAPI Key::Key(int code)
{
    // setup data
    m_code = code;
    m_alt = false;
    m_shift = false;
    m_control = false;
}


PTCAPI Key::Key(int code,bool alt,bool shift,bool control)
{
    // setup data
    m_code = code;
    m_alt = alt;
    m_shift = shift;
    m_control = control;
}


PTCAPI Key::Key(const Key &key)
{
    // assignment
    *this = key;
}


PTCAPI Key::~Key()
{
    // destructor
}




int PTCAPI Key::code() const
{
    // get key code
    return m_code;
}




bool PTCAPI Key::alt() const
{
    // get alt flag
    return m_alt;
}


bool PTCAPI Key::shift() const
{
    // get shift flag
    return m_shift;
}


bool PTCAPI Key::control() const
{
    // get control flag
    return m_control;
}




Key& PTCAPI Key::operator =(const Key &key)
{
    // check for self assignment
    if (this==&key) throw Error("self assignment is not allowed");
        
    // assignment
    m_code = key.m_code;
    m_alt = key.m_alt;
    m_shift = key.m_shift;
    m_control = key.m_control;
    return *this;
}


bool PTCAPI Key::operator ==(const Key &key) const
{
    // equal to
    if (m_code==key.m_code && 
        m_alt==key.m_alt && 
        m_shift==key.m_shift && 
        m_control==key.m_control) return true;

    // failure
    return false;
}


bool PTCAPI Key::operator !=(const Key &key) const
{
    // not equal to
    return !(*this==key);
}
