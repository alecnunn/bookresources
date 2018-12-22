//
// Format class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//  

// include files
#include "Core/Error.h"
#include "Core/Format.h"
#include "Core/Hermes.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




PTCAPI Format::Format()
{
    // defaults
    m_format.r = 0;
    m_format.g = 0;
    m_format.b = 0;
    m_format.a = 0;
    m_format.bits = 0;
    m_format.indexed = 0;

    // initialize hermes
    if (!Hermes_Init()) throw Error("could not initialize hermes");
}


PTCAPI Format::Format(int bits)
{
    // check bits per pixel
    if (bits!=8) throw Error("unsupported bits per pixel");

    // indexed color
    m_format.r = 0;
    m_format.g = 0;
    m_format.b = 0;
    m_format.a = 0;
    m_format.bits = bits;
    m_format.indexed = 1;

    // initialize hermes
    if (!Hermes_Init()) throw Error("could not initialize hermes");
}


PTCAPI Format::Format(int bits,int32 r,int32 g,int32 b,int32 a)
{
    // check bits per pixel
    if (bits%8 || bits<=0 || bits>32) throw Error("unsupported bits per pixel");

    // direct color
    m_format.r = r;
    m_format.g = g;
    m_format.b = b;
    m_format.a = a;
    m_format.bits = bits;
    m_format.indexed = 0;

    // initialize hermes
    if (!Hermes_Init()) throw Error("could not initialize hermes");
}


PTCAPI Format::Format(const Format &format)
{
    // initialize hermes
    if (!Hermes_Init()) throw Error("could not initialize hermes");

#ifdef __COMPILER_SUPPORTS_CONST_CAST__

    // copy constructor
    Hermes_FormatCopy(const_cast<HermesFormat*>(&format.m_format),&m_format);

#else

    // copy constructor
    Hermes_FormatCopy((HermesFormat*)&format.m_format,&m_format);

#endif
}


PTCAPI Format::~Format()
{
    // free hermes
    Hermes_Done();
}




int32 PTCAPI Format::r() const
{
    // get r
    return m_format.r;
}


int32 PTCAPI Format::g() const
{
    // get g
    return m_format.g;
}


int32 PTCAPI Format::b() const
{
    // get b
    return m_format.b;
}


int32 PTCAPI Format::a() const
{
    // get a
    return m_format.a;
}


int PTCAPI Format::bits() const
{
    // get bits
    return m_format.bits;
}


int PTCAPI Format::bytes() const
{
    // get bytes
    return m_format.bits / 8;
}


bool PTCAPI Format::direct() const
{
    // is direct color?
    return m_format.indexed==0;
}


bool PTCAPI Format::indexed() const
{
    // is indexed color?
    return m_format.indexed==1;
}




Format& PTCAPI Format::operator =(const Format &format)
{
    // check for self assignment
    if (this==&format) throw Error("self assignment is not allowed");

#ifdef __COMPILER_SUPPORTS_CONST_CAST__

    // copy other hermes format to this hermes format
    Hermes_FormatCopy(const_cast<HermesFormat*>(&format.m_format),&m_format);

#else

    // copy other hermes format to this hermes format
    Hermes_FormatCopy((HermesFormat*)&format.m_format,&m_format);

#endif

    // return reference
    return *this;
}


bool PTCAPI Format::operator ==(const Format &format) const
{
#ifdef __COMPILER_SUPPORTS_CONST_CAST__

    // is equal to
    return Hermes_FormatEquals(const_cast<HermesFormat*>(&format.m_format),const_cast<HermesFormat*>(&m_format))==1;

#else

    // is equal to
    return Hermes_FormatEquals((HermesFormat*)&format.m_format,(HermesFormat*)&m_format)==1;

#endif
}


bool PTCAPI Format::operator !=(const Format &format) const
{
    // not equal to
    return !(*this==format);
}
