//
// Dynamic Palette class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_DYNAMIC_PALETTE_H
#define __PTC_DYNAMIC_PALETTE_H

// include files
#include "config.h"
#include "C/Extra.h"
#include "C/Palette.h"
#include "Core/Types.h"
#include "Dynamic/Copy.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class DynamicPalette
{
    public:

        DynamicPalette()
        {
            // use exceptions
            ptc_use_exceptions();

            // create palette object
            object = ptc_palette_create();
        }

        DynamicPalette(const int32 data[])
        {
            // use exceptions
            ptc_use_exceptions();

            // create palette object
            object = ptc_palette_create();

            // load data
            ptc_palette_load(object,(int32*)data);
        }

        DynamicPalette(const DynamicPalette &palette)
        {
            // use exceptions
            ptc_use_exceptions();

            // create palette object
            object = ptc_palette_create();

            // assign palette
            ptc_palette_assign(object,palette.object);
        }

        ~DynamicPalette()
        {
            // destroy palette object
            ptc_palette_destroy(object);
        }

        int32* lock()
        {
            // lock palette
            return ptc_palette_lock(object);
        }

        void unlock()
        {
            // unlock palette
            ptc_palette_unlock(object);
        }

        void load(const int32 data[])
        {
            // load palette data
            ptc_palette_load(object,(int32*)data);
        }

        void save(int32 data[]) const
        {
            // save palette data
            ptc_palette_save(object,data);
        }

        const int32* data() const
        {
            // get palette data
            return ptc_palette_data(object);
        }

        DynamicPalette& operator =(const DynamicPalette &palette)
        {
            // assignment operator
            ptc_palette_assign(object,palette.object);
            return *this;
        }

        bool operator ==(const DynamicPalette &palette) const
        {
            // is equal to
            return (ptc_palette_equals(object,palette.object)==1);
        }

        bool operator !=(const DynamicPalette &palette) const
        {
            // not equal to
            return (ptc_palette_equals(object,palette.object)==0);
        }
        
    private:

        // palette object
        PTC_PALETTE object;

        // friend classes
        friend class DynamicCopy;
        friend class DynamicSurface;
        friend class DynamicConsole;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
