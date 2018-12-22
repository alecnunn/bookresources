//
// RNI Copy class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// configuration
#include "ptc.h"

#ifdef __PTC_RNI__

// include files
#include <windows.h>
#include "RNI/Copy.h"
#include "RNI/Except.h"
#include "RNI/Library.h"
#include "RNI/Translate.h"




__declspec(dllexport) int64_t __cdecl ptc_rni_Copy_create(struct Hptc_rni_Copy *java)
{
    try
    {
        // load rni library
        rni_load_library();

        // create copy object
        return (int64_t) new Copy();
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


__declspec(dllexport) void __cdecl ptc_rni_Copy_destroy(struct Hptc_rni_Copy *java,int64_t object)
{
    try
    {
        // destroy copy object
        delete (Copy*) object;

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


__declspec(dllexport) void __cdecl ptc_rni_Copy_request(struct Hptc_rni_Copy *java,int64_t object,struct Hptc_Format *source,struct Hptc_Format *destination)
{
    try
    {
        // request copy
        ((Copy*)object)->request(rni_translate_format(source),rni_translate_format(destination));
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


__declspec(dllexport) void __cdecl ptc_rni_Copy_palette(struct Hptc_rni_Copy *java,int64_t object,struct Hptc_Palette *source,struct Hptc_Palette *destination)
{
    try
    {
        // set copy palettes
        ((Copy*)object)->palette(rni_translate_palette(source),rni_translate_palette(destination));
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


__declspec(dllexport) void __cdecl ptc_rni_Copy_copy(struct Hptc_rni_Copy *java,int64_t object,HObject *source_pixels,long source_x,long source_y,long source_width,long source_height,long source_pitch,HObject *destination_pixels,long destination_x,long destination_y,long destination_width,long destination_height,long destination_pitch)
{
    try
    {
        // lock source pixel array
        void *source_pixel_array = ((ClassArrayOfByte*)source_pixels)->body;

        // lock destination pixel array
        void *destination_pixel_array = ((ClassArrayOfByte*)destination_pixels)->body;
        
        // copy pixels
        ((Copy*)object)->copy(source_pixel_array,source_x,source_y,source_width,source_height,source_pitch,destination_pixel_array,destination_x,destination_y,destination_width,destination_height,destination_pitch);
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


__declspec(dllexport) long __cdecl ptc_rni_Copy_option(struct Hptc_rni_Copy *java,int64_t object,struct Hjava_lang_String *option)
{
    try
    {
        // copy option
        return ((Copy*)object)->option(rni_translate_string(option));
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




#endif
