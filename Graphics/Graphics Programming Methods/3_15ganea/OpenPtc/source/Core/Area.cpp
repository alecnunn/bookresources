//
// Area class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files 
#include "Core/Area.h"
#include "Core/Error.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif

                  


PTCAPI Area::Area()
{
    // defaults
    m_left   = 0;
    m_top    = 0;
    m_right  = 0;
    m_bottom = 0;
}


PTCAPI Area::Area(int left,int top,int right,int bottom)
{
    // setup x data
    if (left<right)
    {
        // assign
        m_left  = left;
        m_right = right;
    }
    else
    {
        // switch left & right
        m_left  = right;
        m_right = left;
    }

    // setup y data
    if (top<bottom)
    {
        // assign
        m_top    = top;
        m_bottom = bottom;
    }
    else
    {
        // switch top & bottom
        m_top    = bottom;
        m_bottom = top;
    }
}


PTCAPI Area::Area(const Area &area)
{
    // assignment
    *this = area;
}


PTCAPI Area::~Area()
{
    // destructor
}




int PTCAPI Area::left() const
{
    // get left
    return m_left;
}


int PTCAPI Area::right() const
{
    // get right
    return m_right;
}


int PTCAPI Area::top() const
{
    // get top
    return m_top;
}


int PTCAPI Area::bottom() const
{
    // get bottom
    return m_bottom;
}


int PTCAPI Area::width() const
{
    // get width
    return m_right-m_left;
}


int PTCAPI Area::height() const
{
    // get height
    return m_bottom-m_top;
}




Area& PTCAPI Area::operator =(const Area &area)
{
    // check for self assignment
    if (this==&area) throw Error("self assignment is not allowed");
        
    // assignment
    m_left   = area.m_left;
    m_top    = area.m_top;
    m_right  = area.m_right;
    m_bottom = area.m_bottom;
    return *this;
}


bool PTCAPI Area::operator ==(const Area &area) const
{
    // is equal to
    if (m_left==area.m_left && m_top==area.m_top && m_right==area.m_right && m_bottom==area.m_bottom) return true;
    else return false;
}


bool PTCAPI Area::operator !=(const Area &area) const
{
    // not equal to
    return !(*this==area);
}
