//
// RNI Console class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// configuration
#include "ptc.h"

#ifdef __PTC_RNI__

// include files
#include <windows.h>
#include "RNI/Except.h"
#include "RNI/Console.h"
#include "RNI/Library.h"
#include "RNI/Translate.h"




__declspec(dllexport) int64_t __cdecl ptc_rni_Console_create(struct Hptc_rni_Console *java)
{
    try
    {
        // load rni library
        rni_load_library();

        // create console object
        return (int64_t) new Console();
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


__declspec(dllexport) void __cdecl ptc_rni_Console_destroy(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // destroy console object
        delete (Console*) object;

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


__declspec(dllexport) void __cdecl ptc_rni_Console_configure(struct Hptc_rni_Console *java,int64_t object,struct Hjava_lang_String *file)
{
    try
    {
        // configure console
        ((Console*)object)->configure(rni_translate_string(file));
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

__declspec(dllexport) long __cdecl ptc_rni_Console_option(struct Hptc_rni_Console *java,int64_t object,struct Hjava_lang_String *option)
{
    try
    {
        // console option
        return ((Console*)object)->option(rni_translate_string(option));
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


__declspec(dllexport) struct HArrayOfObject * __cdecl ptc_rni_Console_modes(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // get console modes
        const Mode *modes = ((Console*)object)->modes();

        // count the number of modes
        int count = 0;
        while (modes[count].valid()) count++;

        // create an array of java mode objects
        ClassArrayOfObject *java_modes = (ClassArrayOfObject*) rni_ClassArrayAlloc(T_CLASS,count+1,"ptc/Mode");

        // initialize the java mode aray
        for (int i=0; i<count; i++)
        {
            // initialize java mode to corresponding c++ mode
            java_modes->body[i] = (HObject*) rni_translate_mode(modes[i]);
        }

        // terminate mode list
        java_modes->body[count] = (HObject*) rni_translate_mode(Mode());

        // return java modes
        return java_modes;
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


__declspec(dllexport) void __cdecl ptc_rni_Console_open_1(struct Hptc_rni_Console *java,int64_t object,struct Hjava_lang_String *title,long pages)
{
    try
    {
        // open console
        ((Console*)object)->open(rni_translate_string(title),pages);
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


__declspec(dllexport) void __cdecl ptc_rni_Console_open_2(struct Hptc_rni_Console *java,int64_t object,struct Hjava_lang_String *title,struct Hptc_Format *format,long pages)
{
    try
    {
        // open console
        ((Console*)object)->open(rni_translate_string(title),rni_translate_format(format),pages);
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


__declspec(dllexport) void __cdecl ptc_rni_Console_open_3(struct Hptc_rni_Console *java,int64_t object,struct Hjava_lang_String *title,long width,long height,struct Hptc_Format *format,long pages)
{
    try
    {
        // open console
        ((Console*)object)->open(rni_translate_string(title),width,height,rni_translate_format(format),pages);
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


_declspec(dllexport) void __cdecl ptc_rni_Console_open_4(struct Hptc_rni_Console *java,int64_t object,struct Hjava_lang_String *title,struct Hptc_Mode *mode,long pages)
{
    try
    {
        // open console
        ((Console*)object)->open(rni_translate_string(title),rni_translate_mode(mode),pages);
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


__declspec(dllexport) void __cdecl ptc_rni_Console_close(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // close console
        ((Console*)object)->close();
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


__declspec(dllexport) void __cdecl ptc_rni_Console_flush(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // flush console
        ((Console*)object)->flush();
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


__declspec(dllexport) void __cdecl ptc_rni_Console_finish(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // finish console
        ((Console*)object)->finish();
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


__declspec(dllexport) void __cdecl ptc_rni_Console_update_1(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // update console
        ((Console*)object)->update();
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


__declspec(dllexport) void __cdecl ptc_rni_Console_update_2(struct Hptc_rni_Console *java,int64_t object,struct Hptc_Area *area)
{
    try
    {
        // update console area
        ((Console*)object)->update(rni_translate_area(area));
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


__declspec(dllexport) long __cdecl ptc_rni_Console_key(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // check for key press
        return (long) ((Console*)object)->key();
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


__declspec(dllexport) struct Hptc_Key * __cdecl ptc_rni_Console_read(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // read key press
        return (Hptc_Key*) rni_translate_key(((Console*)object)->read());
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


__declspec(dllexport) void __cdecl ptc_rni_Console_copy_1(struct Hptc_rni_Console *java,int64_t object,struct Hptc_base_Surface *surface)
{
    try
    {
        // copy console to surface
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


__declspec(dllexport) void __cdecl ptc_rni_Console_copy_2(struct Hptc_rni_Console *java,int64_t object,struct Hptc_base_Surface *surface,struct Hptc_Area *source,struct Hptc_Area *destination)
{
    try
    {
        // copy console area to surface area
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


__declspec(dllexport) HObject * __cdecl ptc_rni_Console_lock(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // lock console
        throw Error("console locking is not supported");

        // failure
        return 0;
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


__declspec(dllexport) void __cdecl ptc_rni_Console_unlock(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // unlock console
        throw Error("console unlocking is not supported");
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


__declspec(dllexport) void __cdecl ptc_rni_Console_load_1(struct Hptc_rni_Console *java,int64_t object,HObject *pixels,long width,long height,long pitch,struct Hptc_Format *format,struct Hptc_Palette *palette)
{
    try
    {
        // lock pixel array
        void *pixel_array = ((ClassArrayOfByte*)pixels)->body;

        // load pixels to console
        ((Console*)object)->load(pixel_array,width,height,pitch,rni_translate_format(format),rni_translate_palette(palette));
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


__declspec(dllexport) void __cdecl ptc_rni_Console_load_2(struct Hptc_rni_Console *java,int64_t object,HObject *pixels,long width,long height,long pitch,struct Hptc_Format *format,struct Hptc_Palette *palette,struct Hptc_Area *source,struct Hptc_Area *destination)
{
    try
    {
        // lock pixel array
        void *pixel_array = ((ClassArrayOfByte*)pixels)->body;

        // load pixels to console area
        ((Console*)object)->load(pixel_array,width,height,pitch,rni_translate_format(format),rni_translate_palette(palette),rni_translate_area(source),rni_translate_area(destination));
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


__declspec(dllexport) void __cdecl ptc_rni_Console_save_1(struct Hptc_rni_Console *java,int64_t object,HObject *pixels,long width,long height,long pitch,struct Hptc_Format *format,struct Hptc_Palette *palette)
{
    try
    {
        // lock pixel array
        void *pixel_array = ((ClassArrayOfByte*)pixels)->body;

        // save console pixels
        ((Console*)object)->save(pixel_array,width,height,pitch,rni_translate_format(format),rni_translate_palette(palette));
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


__declspec(dllexport) void __cdecl ptc_rni_Console_save_2(struct Hptc_rni_Console *java,int64_t object,HObject *pixels,long width,long height,long pitch,struct Hptc_Format *format,struct Hptc_Palette *palette,struct Hptc_Area *source,struct Hptc_Area *destination)
{
    try
    {
        // lock pixel array
        void *pixel_array = ((ClassArrayOfByte*)pixels)->body;

        // save console area pixels
        ((Console*)object)->save(pixel_array,width,height,pitch,rni_translate_format(format),rni_translate_palette(palette),rni_translate_area(source),rni_translate_area(destination));
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


__declspec(dllexport) void __cdecl ptc_rni_Console_clear_1(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // clear console
        ((Console*)object)->clear();
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


__declspec(dllexport) void __cdecl ptc_rni_Console_clear_2(struct Hptc_rni_Console *java,int64_t object,struct Hptc_Color *color)
{
    try
    {
        // clear console color
        ((Console*)object)->clear(rni_translate_color(color));
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


__declspec(dllexport) void __cdecl ptc_rni_Console_clear_3(struct Hptc_rni_Console *java,int64_t object,struct Hptc_Color *color,struct Hptc_Area *area)
{
    try
    {
        // clear console color area
        ((Console*)object)->clear(rni_translate_color(color),rni_translate_area(area));
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


__declspec(dllexport) void __cdecl ptc_rni_Console_palette_1(struct Hptc_rni_Console *java,int64_t object,struct Hptc_Palette *palette)
{
    try
    {
        // set console palette
        ((Console*)object)->palette(rni_translate_palette(palette));
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


__declspec(dllexport) struct Hptc_Palette * __cdecl ptc_rni_Console_palette_2(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // get console palette
        return (Hptc_Palette*) rni_translate_palette(((Console*)object)->palette());
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


__declspec(dllexport) void __cdecl ptc_rni_Console_clip_1(struct Hptc_rni_Console *java,int64_t object,struct Hptc_Area *area)
{
    try
    {
        // set console clip area
        ((Console*)object)->clip(rni_translate_area(area));
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


__declspec(dllexport) long __cdecl ptc_rni_Console_width(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // get console width
        return ((Console*)object)->width();
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


__declspec(dllexport) long __cdecl ptc_rni_Console_height(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // get console height
        return ((Console*)object)->height();
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


__declspec(dllexport) long __cdecl ptc_rni_Console_pages(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // get console pages
        return ((Console*)object)->pages();
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


__declspec(dllexport) long __cdecl ptc_rni_Console_pitch(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // get console pitch
        return ((Console*)object)->pitch();
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


__declspec(dllexport) struct Hptc_Area * __cdecl ptc_rni_Console_area(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // get console area
        return (Hptc_Area*) rni_translate_area(((Console*)object)->area());
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


__declspec(dllexport) struct Hptc_Area * __cdecl ptc_rni_Console_clip_2(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // get console clip area
        return rni_translate_area(((Console*)object)->clip());
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


__declspec(dllexport) struct Hptc_Format * __cdecl ptc_rni_Console_format(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // get console format
        return rni_translate_format(((Console*)object)->format());
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


__declspec(dllexport) struct Hjava_lang_String * __cdecl ptc_rni_Console_name(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // get console name
        return rni_translate_string(((Console*)object)->name());
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


__declspec(dllexport) struct Hjava_lang_String * __cdecl ptc_rni_Console_title(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // get console title
        return rni_translate_string(((Console*)object)->title());
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


__declspec(dllexport) struct Hjava_lang_String * __cdecl ptc_rni_Console_information(struct Hptc_rni_Console *java,int64_t object)
{
    try
    {
        // get console information
        return rni_translate_string(((Console*)object)->information());
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
