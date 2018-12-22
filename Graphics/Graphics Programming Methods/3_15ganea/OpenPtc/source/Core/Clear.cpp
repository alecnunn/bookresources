//
// Clear class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include <string.h>
#include "Core/Clear.h"
#include "Core/Color.h"
#include "Core/Error.h"
#include "Core/Format.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




PTCAPI Clear::Clear()
{ 
    // initialize hermes
    if (!Hermes_Init()) throw Error("could not initialize hermes");

    // static default format
    static Format default_format;

    // default current format
    m_format = &default_format;

    // create hermes clearer instance
    m_handle = Hermes_ClearerInstance();

    // check hermes clearer instance
    if (!m_handle) throw Error("could not create hermes clearer instance"); 
}


PTCAPI Clear::~Clear()
{ 
    // return the clearer instance
    Hermes_ClearerReturn(m_handle);

    // free hermes
    Hermes_Done();
}




void PTCAPI Clear::request(const Format &format)
{ 
#ifdef __COMPILER_SUPPORTS_CONST_CAST__

    // cast from const format to hermes format pointer
    HermesFormat *hermes_format = const_cast<HermesFormat*>(&format.m_format);

#else

    // cast from const format to hermes format pointer
    HermesFormat *hermes_format = (HermesFormat*)&format.m_format;

#endif

    // request surface clear for this format
    if (!Hermes_ClearerRequest(m_handle,hermes_format)) throw Error("unsupported clear format");

    // update current format
    *m_format = format;
}




void PTCAPI Clear::clear(void *pixels,int x,int y,int width,int height,int pitch,const Color &color)
{ 
    #ifdef __DEBUG__

        //
        // This checking is performed only when __DEBUG__ is defined,
        // and can be used to track down errors early caused by passing
        // null pointers to the clear function.
        //
        // Even though technically clear should never recieve a null
        // pointer, we provide a check here to assist in debugging
        // just in case it ever does!
        //

        // check pixels pointer
        if (!pixels) throw Error("null pixels pointer in clear");

    #else

        // In release build no checking is performed for the sake of efficiency.

    #endif
        
    // check format type
    if (m_format->direct())
    {
        // check color type
        if (!color.direct()) throw Error("direct pixel formats can only be cleared with direct color");

        // setup clear color
        int32 r = (int32) ( color.r() * 255 );
        int32 g = (int32) ( color.g() * 255 );
        int32 b = (int32) ( color.b() * 255 );
        int32 a = (int32) ( color.a() * 255 );

        // clamp red
        if (r>255) r = 255;
        else if (r<0) r = 0;

        // clamp green
        if (g>255) g = 255;
        else if (g<0) g = 0;

        // clamp blue
        if (b>255) b = 255;
        else if (b<0) b = 0;

        // clamp alpha
        if (a>255) a = 255;
        else if (a<0) a = 0;

        // perform the clearing
        Hermes_ClearerClear(m_handle,pixels,x,y,width,height,pitch,r,g,b,(char8)a);
    }
    else
    {
        // check color type
        if (!color.indexed()) throw Error("indexed pixel formats can only be cleared with indexed color");

        // setup clear index
        int32 index = (int32) color.index();

        // clamp color index
        if (index>255) index = 255;
        else if (index<0) index = 0;

        // perform the clearing
        Hermes_ClearerClear(m_handle,pixels,x,y,width,height,pitch,0,0,0,(char8)index);
    }
}
