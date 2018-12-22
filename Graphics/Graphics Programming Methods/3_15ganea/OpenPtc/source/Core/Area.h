//
// Area class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_AREA_H
#define __PTC_AREA_H

// include files
#include "config.h"
#include "Core/Types.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class Area
{
    public:

        // setup
        PTCAPI Area();
        PTCAPI Area(int left,int top,int right,int bottom);
        PTCAPI Area(const Area &area);
        PTCAPI ~Area();

        // data access
        int PTCAPI left()   const;
        int PTCAPI right()  const;
        int PTCAPI top()    const;
        int PTCAPI bottom() const;
        int PTCAPI width()  const;
        int PTCAPI height() const;

        // operators
        Area& PTCAPI operator =(const Area &area);
        bool PTCAPI operator ==(const Area &area) const;
        bool PTCAPI operator !=(const Area &area) const;

    private:

        // data
        int m_left;
        int m_right;
        int m_top;
        int m_bottom;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
