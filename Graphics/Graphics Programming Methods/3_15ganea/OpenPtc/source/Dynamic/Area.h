//
// Dynamic Area class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_DYNAMIC_AREA_H
#define __PTC_DYNAMIC_AREA_H

// include files
#include "config.h"
#include "C/Area.h"
#include "C/Extra.h"
#include "Core/Types.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class DynamicArea
{
    public:

        DynamicArea()
        {
            // use exceptions
            ptc_use_exceptions();

            // create area object
            object = ptc_area_create(0,0,0,0);
        }

        DynamicArea(int left,int top,int right,int bottom)
        {
            // use exceptions
            ptc_use_exceptions();

            // create area object
            object = ptc_area_create(left,top,right,bottom);
        }

        DynamicArea(const DynamicArea &area)
        {
            // use exceptions
            ptc_use_exceptions();

            // create area object
            object = ptc_area_create(area.left(),area.top(),area.right(),area.bottom());
        }

        ~DynamicArea()
        {
            // destroy area object
            ptc_area_destroy(object);
        }

        int left() const
        {
            // get left
            return ptc_area_left(object);
        }

        int right() const
        {
            // get right
            return ptc_area_right(object);
        }

        int top() const
        {
            // get top
            return ptc_area_top(object);
        }

        int bottom() const
        {
            // get bottom
            return ptc_area_bottom(object);
        }

        int width()  const
        {
            // get width
            return ptc_area_width(object);
        }

        int height() const
        {
            // get height
            return ptc_area_height(object);
        }
        
        DynamicArea& operator =(const DynamicArea &area)
        {
            // assignment operator
            ptc_area_assign(object,area.object);
            return *this;
        }

        bool operator ==(const DynamicArea &area) const
        {
            // is equal to
            return (ptc_area_equals(object,area.object)==1);
        }

        bool operator !=(const DynamicArea &area) const
        {
            // not equal to
            return (ptc_area_equals(object,area.object)==0);
        }
     
    private:

        // area object
        PTC_AREA object;

        // friend classes
        friend class DynamicClipper;
        friend class DynamicSurface;
        friend class DynamicConsole;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
