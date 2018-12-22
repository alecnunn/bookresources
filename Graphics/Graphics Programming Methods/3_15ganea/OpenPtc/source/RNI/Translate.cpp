//
// RNI Translate routines for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// configuration
#include "ptc.h"

#ifdef __PTC_RNI__

// include files
#include <windows.h>
#include "RNI/Console.h"
#include "RNI/Library.h"
#include "RNI/Translate.h"




Key rni_translate_key(Hptc_Key *key)
{
    // create key object
    return Key(key->code,(key->alt==1),(key->shift==1),(key->control==1));
}


Hptc_Key* rni_translate_key(const Key &key)
{
    // construct java key object
    return (Hptc_Key*) rni_execute_java_constructor(0,"ptc/Key",0,"(IZZZ)",key.code(),key.alt(),key.shift(),key.control());
}


Area rni_translate_area(Hptc_Area *area)
{
    // create area object
    return Area(area->left,area->top,area->right,area->bottom);
}


Hptc_Area* rni_translate_area(const Area &area)
{
    // construct java area object
    return (Hptc_Area*) rni_execute_java_constructor(0,"ptc/Area",0,"(IIII)",area.left(),area.top(),area.right(),area.bottom());
}


Mode rni_translate_mode(Hptc_Mode *mode)
{
    // check if mode is valid
    if (mode->valid)
    {
        // create mode object
        return Mode(mode->width,mode->height,rni_translate_format(mode->format));
    }
    else
    {
        // default constructor
        return Mode();
    }
}


Hptc_Mode* rni_translate_mode(const Mode &mode)
{
    // check if mode is valid
    if (mode.valid())
    {
        // construct java mode object
        return (Hptc_Mode*) rni_execute_java_constructor(0,"ptc/Mode",0,"(IILptc/Format;)",mode.width(),mode.height(),rni_translate_format(mode.format()));
    }
    else
    {
        // default constructor
        return (Hptc_Mode*) rni_execute_java_constructor(0,"ptc/Mode",0,"()");
    }
}


Color rni_translate_color(Hptc_Color *color)
{
    // check color type
    if (color->direct)
    {
        // direct color
        return Color(color->r,color->g,color->b,color->a);
    }
    else if (color->indexed)
    {
        // indexed color
        return Color(color->index);
    }
    else
    {
        // default constructor
        return Color();
    }
}


Hptc_Color* rni_translate_color(const Color &color)
{
    // check color type
    if (color.direct())
    {
        // direct color
        return (Hptc_Color*) rni_execute_java_constructor(0,"ptc/Color",0,"(FFFF)",color.r(),color.g(),color.b(),color.a());
    }
    else if (color.indexed())
    {
        // indexed color
        return (Hptc_Color*) rni_execute_java_constructor(0,"ptc/Color",0,"(I)",color.index());
    }
    else
    {
        // default constructor
        return (Hptc_Color*) rni_execute_java_constructor(0,"ptc/Color",0,"()");
    }
}


Format rni_translate_format(Hptc_Format *format)
{
    // check format type
    if (format->direct)
    {
        // direct color
        return Format(format->bits,format->r,format->g,format->b,format->a);
    }
    else if (format->indexed)
    {
        // indexed color
        return Format(format->bits);
    }
    else
    {
        // default constructor
        return Format();
    }
}


Hptc_Format* rni_translate_format(const Format &format)
{
    // check format type
    if (format.direct())
    {
        // direct color
        return (Hptc_Format*) rni_execute_java_constructor(0,"ptc/Format",0,"(IIIII)",format.bits(),format.r(),format.g(),format.b(),format.a());
    }
    else if (format.indexed())
    {
        // indexed color
        return (Hptc_Format*) rni_execute_java_constructor(0,"ptc/Format",0,"(I)",format.bits());
    }
    else
    {
        // default constructor
        return (Hptc_Format*) rni_execute_java_constructor(0,"ptc/Format",0,"()");
    }
}


Palette rni_translate_palette(Hptc_Palette *palette)
{
    // fudge!
    Palette pal;
    int32 *arse = pal.lock();
    for (int i=0; i<256; i++) arse[i] = i;
    pal.unlock();
    return pal;

    /*
    // lock java palette object
    ClassArrayOfInt *array = (ClassArrayOfInt*) rni_execute_java_dynamic_method(0,(HObject*)palette,"lock","()[I");

    // create translated palette
    Palette translated((int32*)array->body);

    // unlock java palette object
    rni_execute_java_dynamic_method(0,(HObject*)palette,"unlock","()V");

    // return palette
    return translated;
    */

    /*
    // get palette data array
    int32 *array = (int32*) (palette->data->body);

    // fudge
    for (int i=0; i<256; i++) array[i] = i;

    // create palette object
    return Palette(array);
    */
}


Hptc_Palette* rni_translate_palette(const Palette &palette)
{
    // create an array of java integers
    ClassArrayOfInt *array = (ClassArrayOfInt*) rni_ArrayAlloc(T_INT,256);

    // save palette data to array
    palette.save((int32*)array->body);
    
    // create java palette object
    return (Hptc_Palette*) rni_execute_java_constructor(0,"ptc/Palette",0,"([I)",array);
}


const char* rni_translate_string(Hjava_lang_String *string)
{
    // translate java to c string
    static char c_string[1024];
	rni_javaString2CString(string,c_string,sizeof c_string);
    return c_string;
}


Hjava_lang_String* rni_translate_string(const char string[])
{
    // translate c string to java string
    return rni_makeJavaStringFromUtf8(string);
}




#endif
