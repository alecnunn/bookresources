//
// RNI Clear class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// configuration
#include "ptc.h"

#ifdef __PTC_RNI__

// include files
#include <windows.h>
#include "RNI/Clear.h"
#include "RNI/Except.h"
#include "RNI/Library.h"
#include "RNI/Translate.h"




__declspec(dllexport) int64_t __cdecl ptc_rni_Clear_create(struct Hptc_rni_Clear *java)
{
    try
    {
        // load rni library
        rni_load_library();

        // create clear object
        return (int64_t) new Clear();
    }
    catch (Error &error)
    {
        // throw rni error
        rni_throw(error);
    }
    catch (...)
    {
        // unknown
        rni_throw();
    }

    // failure
    return 0;
}


__declspec(dllexport) void __cdecl ptc_rni_Clear_destroy(struct Hptc_rni_Clear *java,int64_t object)
{
    try
    {
        // destroy clear object
        delete (Clear*) object;

        // free rni library
        rni_free_library();
    }
    catch (Error &error)
    {
        // throw rni error
        rni_throw(error);
    }
    catch (...)
    {
        // unknown
        rni_throw();
    }
}


__declspec(dllexport) void __cdecl ptc_rni_Clear_request(struct Hptc_rni_Clear *java,int64_t object,struct Hptc_Format *format)
{
    try
    {
        // request clear
        ((Clear*)object)->request(rni_translate_format(format));
    }
    catch (Error &error)
    {
        // throw rni error
        rni_throw(error);
    }
    catch (...)
    {
        // unknown
        rni_throw();
    }
}


__declspec(dllexport) void __cdecl ptc_rni_Clear_clear(struct Hptc_rni_Clear *java,int64_t object,HObject *pixels,long x,long y,long width,long height,long pitch,struct Hptc_Color *color)
{
    try
    {
        // lock pixel array
        void *pixel_array = ((ClassArrayOfByte*)pixels)->body;

        // clear pixels
        ((Clear*)object)->clear(pixel_array,x,y,width,height,pitch,rni_translate_color(color));
    }
    catch (Error &error)
    {
        // throw rni error
        rni_throw(error);
    }
    catch (...)
    {
        // unknown
        rni_throw();
    }
}




#endif
