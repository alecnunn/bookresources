//
// Copy class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org) 
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include <string.h>
#include "Core/Copy.h"
#include "Core/Error.h"
#include "Core/Format.h"
#include "Core/Palette.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




PTCAPI Copy::Copy()
{ 
    // initialize hermes
    if (!Hermes_Init()) throw Error("could not initialize hermes");

    // default convert flags
    m_flags = HERMES_CONVERT_NORMAL;

    // create hermes converter instance
    m_handle = Hermes_ConverterInstance(m_flags);

    // check converter instance
    if (!m_handle) throw Error("could not create hermes converter instance");
}


PTCAPI Copy::~Copy()
{ 
    // return the converter instance
    Hermes_ConverterReturn(m_handle);

    // free hermes
    Hermes_Done();
}




void PTCAPI Copy::request(const Format &source,const Format &destination)
{ 
#ifdef __COMPILER_SUPPORTS_CONST_CAST__

    // const cast to hermes formats
    HermesFormat *hermes_source_format = const_cast<HermesFormat*>(&source.m_format);
    HermesFormat *hermes_destination_format = const_cast<HermesFormat*>(&destination.m_format);

#else

    // cast to hermes formats
    HermesFormat *hermes_source_format = (HermesFormat*)&source.m_format;
    HermesFormat *hermes_destination_format = (HermesFormat*)&destination.m_format;

#endif

    // request a format conversion from hermes
    if (!Hermes_ConverterRequest(m_handle,hermes_source_format,hermes_destination_format)) throw Error("unsupported hermes pixel format conversion");
}




void PTCAPI Copy::palette(const Palette &source,const Palette &destination)
{
    // set source and destination palettes
    if (!Hermes_ConverterPalette(m_handle,source.m_handle,destination.m_handle)) throw Error("could not set hermes conversion palettes");
}




void PTCAPI Copy::copy(const void *source_pixels,int source_x,int source_y,int source_width,int source_height,int source_pitch,
                              void *destination_pixels,int destination_x,int destination_y,int destination_width,int destination_height,int destination_pitch)
{ 
    #ifdef __DEBUG__

        //
        // This checking is performed only when __DEBUG__ is defined,
        // and can be used to track down errors early caused by passing
        // null pointers to surface and console functions.
        //
        // Even though technicially it is the users responsibility
        // to ensure that all pointers are non-null, it is useful
        // to provide a check here in debug build to prevent such
        // bugs from ever occuring.
        //
        // The checking function also tests that the source and destination
        // pointers are not the same, a bug that can be caused by copying
        // a surface to itself. The nature of the copy routine is that
        // this operation is undefined if the source and destination memory
        // areas overlap.
        //

        // check source pointer
        if (!source_pixels) throw Error("null source pointer in copy");

        // check destination pointer
        if (!destination_pixels) throw Error("null destination pointer in copy");

        // check that source and destination are not identical
        if (source_pixels==destination_pixels) throw Error("identical source and destination pointers in copy");

    #else

        // in release build no checking is performed for the sake of efficiency.

    #endif

#ifdef __COMPILER_SUPPORTS_CONST_CAST__
        
    // const cast source_pixels to void pointer
    void *source = const_cast<void *>(source_pixels);

#else

    // cast source_pixels to void pointer
    void *source = (void *)source_pixels;

#endif

    // perform copy conversion via hermes
    if (!Hermes_ConverterCopy(m_handle,source,source_x,source_y,source_width,source_height,source_pitch,
                              destination_pixels,destination_x,destination_y,destination_width,destination_height,destination_pitch))
    {
        // error message
        throw Error("hermes conversion failure");
    }
}




bool PTCAPI Copy::option(const char option[])
{
    // handle "enable dithering" option
    if (!stricmp(option,"attempt dithering") && !(m_flags&HERMES_CONVERT_DITHER)) 
    {
        // set dithering flag flag
        m_flags |= HERMES_CONVERT_DITHER;

        // update
        update();

        // recognized
        return true;
    }

    // handle "disable dithering" option
    if (!stricmp(option,"disable dithering") && (m_flags&HERMES_CONVERT_DITHER)) 
    {
        // clear out dithering flag
        m_flags &= ~HERMES_CONVERT_DITHER;

        // update
        update();

        // recognized
        return true;
    }

    // not recognized
    return false;
}




void Copy::update()
{
    // return old converter handle
    Hermes_ConverterReturn(m_handle);

    // create new hermes converter instance
    m_handle = Hermes_ConverterInstance(m_flags);

    // check hermes converter instance
    if (!m_handle) throw Error("could not update hermes converter instance");
}
