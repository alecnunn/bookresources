//
// Palette class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_PALETTE_H
#define __PTC_PALETTE_H

// include files
#include "config.h"
#include "Core/Copy.h"
#include "Core/Types.h"
#include "Core/Hermes.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class Palette
{
    public:

        // setup
        PTCAPI Palette();
        PTCAPI Palette(const int32 data[]);
        PTCAPI Palette(const Palette &palette);
        PTCAPI ~Palette();

        // memory access
        int32* PTCAPI lock();
        void PTCAPI unlock();

        // load palette data
        void PTCAPI load(const int32 data[]);

        // save palette data
        void PTCAPI save(int32 data[]) const;
        
        // get palette data
        const int32* PTCAPI data() const;

        // operators
        Palette& PTCAPI operator =(const Palette &palette);
        bool PTCAPI operator ==(const Palette &palette) const;
        bool PTCAPI operator !=(const Palette &palette) const;

    private:

        // locked flag
        bool m_locked;

        // hermes handle
        HermesHandle m_handle;

        // copy friend function
        friend void Copy::palette(const Palette &source,const Palette &destination);
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
