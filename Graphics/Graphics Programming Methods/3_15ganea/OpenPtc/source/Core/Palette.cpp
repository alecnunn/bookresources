//
// Palette class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include <string.h>
#include "Core/Error.h"
#include "Core/Palette.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




PTCAPI Palette::Palette()
{
    // clear locked flag
    m_locked = false;

    // initialize hermes
    if (!Hermes_Init()) throw Error("could not initialize hermes");

    // create hermes palette instance
    m_handle = Hermes_PaletteInstance();

    // check hermes palette instance
    if (!m_handle) throw Error("could not create hermes palette instance");

    // clear the palette
    int32 zero[256];
    memset(zero,0,sizeof(zero));
    load(zero);
}


PTCAPI Palette::Palette(const int32 data[])
{
    // clear locked flag
    m_locked = false;

    // initialize hermes
    if (!Hermes_Init()) throw Error("could not initialize hermes");

    // create hermes palette instance
    m_handle = Hermes_PaletteInstance();

    // check hermes palette instance
    if (!m_handle) throw Error("could not create hermes palette instance");

    // load data
    load(data);
}


PTCAPI Palette::Palette(const Palette &palette)
{
    // clear locked flag
    m_locked = false;

    // initialize hermes
    if (!Hermes_Init()) throw Error("could not initialize hermes");

    // create hermes palette instance
    m_handle = Hermes_PaletteInstance();

    // check hermes palette instance
    if (!m_handle) throw Error("could not create hermes palette instance");

    // assignment
    *this = palette;
}


PTCAPI Palette::~Palette()
{
    // check if palette is locked
    if (m_locked) throw Error("palette is still locked");

    // return palette instance
    Hermes_PaletteReturn(m_handle);

    // free hermes
    Hermes_Done();
}




int32* PTCAPI Palette::lock()
{
    // check locked flag
    if (m_locked) throw Error("palette is already locked");

    // set locked flag
    m_locked = true;

    // return hermes palette
    return Hermes_PaletteGet(m_handle);
}


void PTCAPI Palette::unlock()
{
    // check locked flag
    if (!m_locked) throw Error("palette is not locked");

    // reset locked flag
    m_locked = false;
}




void PTCAPI Palette::load(const int32 data[])
{
#ifdef __COMPILER_SUPPORTS_CONST_CAST__
        
    // const cast source_pixels to void pointer
    Hermes_PaletteSet(m_handle,const_cast<int32*>(data));

#else

    // load palette data
    Hermes_PaletteSet(m_handle,(int32*)data);

#endif
}




void PTCAPI Palette::save(int32 data[]) const
{
    // save palette data
    memcpy(data,Hermes_PaletteGet(m_handle),1024);
}




const int32* PTCAPI Palette::data() const
{
    // get palette data
    return Hermes_PaletteGet(m_handle);
}




Palette& PTCAPI Palette::operator =(const Palette &palette)
{
    // assignment
    Hermes_PaletteSet(m_handle,Hermes_PaletteGet(palette.m_handle));
    return *this;
}


bool PTCAPI Palette::operator ==(const Palette &palette) const
{
    // is equal to
    if (memcmp(Hermes_PaletteGet(m_handle),Hermes_PaletteGet(palette.m_handle),1024)==0) return true;
    else return false;
}


bool PTCAPI Palette::operator !=(const Palette &palette) const
{
    // not equal to
    return !(*this==palette);
}
