//
// JNI Translate routines for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// configuration
#include "ptc.h"

#ifdef __PTC_JNI__

// include files
#include "JNI/Array.h"
#include "JNI/Translate.h"




Key jni_translate_key(JNIEnv *jni,jobject key)
{
    // get ptc.Key class
    jclass cls = jni->GetObjectClass(key);

    // get method ids
    jmethodID key_code    = jni->GetMethodID(cls,"code","()I");
    jmethodID key_alt     = jni->GetMethodID(cls,"alt","()Z");
    jmethodID key_shift   = jni->GetMethodID(cls,"shift","()Z");
    jmethodID key_control = jni->GetMethodID(cls,"control","()Z");

    // get area data
    int code     = (int) jni->CallIntMethod(key,key_code);
    bool alt     = (bool) (jni->CallBooleanMethod(key,key_alt)==1);
    bool shift   = (bool) (jni->CallBooleanMethod(key,key_shift)==1);
    bool control = (bool) (jni->CallBooleanMethod(key,key_control)==1);

	// create key object
	return Key(code,alt,shift,control);
}


jobject jni_translate_key(JNIEnv *jni,const Key &key)
{
    // get ptc.Key class
    jclass cls = jni->FindClass("ptc/Key");

    // get constructor method id
    jmethodID method = jni->GetMethodID(cls,"<init>","(IZZZ)V");

    // construct key object
    return jni->NewObject(cls,method,(jint)key.code(),(jboolean)key.alt(),(jboolean)key.shift(),(jboolean)key.control());
}


Area jni_translate_area(JNIEnv *jni,jobject area)
{
    // get ptc.Area class
    jclass cls = jni->GetObjectClass(area);

    // get method ids
    jmethodID area_left   = jni->GetMethodID(cls,"left","()I");
    jmethodID area_top    = jni->GetMethodID(cls,"top","()I");
    jmethodID area_right  = jni->GetMethodID(cls,"right","()I");
    jmethodID area_bottom = jni->GetMethodID(cls,"bottom","()I");

    // get area data
    int left   = (int) jni->CallIntMethod(area,area_left);
    int top    = (int) jni->CallIntMethod(area,area_top);
    int right  = (int) jni->CallIntMethod(area,area_right);
    int bottom = (int) jni->CallIntMethod(area,area_bottom);

	// create area object
	return Area(left,top,right,bottom);
}


jobject jni_translate_area(JNIEnv *jni,const Area &area)
{
    // get ptc.Area class
    jclass cls = jni->FindClass("ptc/Area");

    // get constructor method id
    jmethodID method = jni->GetMethodID(cls,"<init>","(IIII)V");

    // construct area object
    return jni->NewObject(cls,method,(jint)area.left(),(jint)area.top(),(jint)area.right(),(jint)area.bottom());
}


Mode jni_translate_mode(JNIEnv *jni,jobject mode)
{
    // get ptc.Mode class
    jclass cls = jni->GetObjectClass(mode);

    // get method ids
    jmethodID mode_valid  = jni->GetMethodID(cls,"valid","()Z");
    jmethodID mode_width  = jni->GetMethodID(cls,"width","()I");
    jmethodID mode_height = jni->GetMethodID(cls,"height","()I");
    jmethodID mode_format = jni->GetMethodID(cls,"format","()Lptc/Format;");

    // get mode data
    bool valid = (bool) (jni->CallBooleanMethod(mode,mode_valid)==1);
    int width = (int) jni->CallIntMethod(mode,mode_width);
    int height = (int) jni->CallIntMethod(mode,mode_height);
    jobject format = jni->CallObjectMethod(mode,mode_format);
 
    // check valid
    if (valid)
    {
        // create mode object
        return Mode(width,height,jni_translate_format(jni,format));
    }
    else
    {
        // invalid mode
        return Mode();
    }
}


jobject jni_translate_mode(JNIEnv *jni,const Mode &mode)
{
    // get ptc.Mode class
    jclass cls = jni->FindClass("ptc/Mode");

    // check valid
    if (mode.valid())
    {
        // get constructor method id
        jmethodID method = jni->GetMethodID(cls,"<init>","(IILptc/Format;)V");

        // construct key object
        return jni->NewObject(cls,method,(jint)mode.width(),(jint)mode.height(),jni_translate_format(jni,mode.format()));
    }
    else
    {
        // get constructor method id
        jmethodID method = jni->GetMethodID(cls,"<init>","()V");

        // construct key object
        return jni->NewObject(cls,method);
    }
}


Color jni_translate_color(JNIEnv *jni,jobject color)
{
    // get ptc.Color class
    jclass cls = jni->GetObjectClass(color);

    // get method ids
    jmethodID color_r       = jni->GetMethodID(cls,"r","()F");
    jmethodID color_g       = jni->GetMethodID(cls,"g","()F");
    jmethodID color_b       = jni->GetMethodID(cls,"b","()F");
    jmethodID color_a       = jni->GetMethodID(cls,"a","()F");
    jmethodID color_index   = jni->GetMethodID(cls,"index","()I");
    jmethodID color_direct  = jni->GetMethodID(cls,"direct","()Z");
    jmethodID color_indexed = jni->GetMethodID(cls,"indexed","()Z");

    // get format data
    float r = (float) jni->CallFloatMethod(color,color_r);
    float g = (float) jni->CallFloatMethod(color,color_g);
    float b = (float) jni->CallFloatMethod(color,color_b);
    float a = (float) jni->CallFloatMethod(color,color_a);
    int index = (int) jni->CallIntMethod(color,color_index);
 
    // check color type
    if (jni->CallBooleanMethod(color,color_direct))
    {
        // direct color
        return Color(r,g,b,a);
    }
    else if (jni->CallBooleanMethod(color,color_indexed))
    {
        // indexed color
        return Color(index);
    }
    else
    {
        // default constructor
        return Color();
    }
}


jobject jni_translate_color(JNIEnv *jni,const Color &color)
{
    // get ptc.Color class
    jclass cls = jni->FindClass("ptc/Color");

    // check color type
    if (color.direct())
    {
        // get constructor method id
        jmethodID method = jni->GetMethodID(cls,"<init>","(FFFF)V");

        // construct color object
        jobject object = jni->NewObject(cls,method,(jfloat)color.r(),(jfloat)color.g(),(jfloat)color.b(),(jfloat)color.a());

        // success
        return object;
    }
    else if (color.indexed())
    {
        // get constructor method id
        jmethodID method = jni->GetMethodID(cls,"<init>","(I)V");

        // construct color object
        return jni->NewObject(cls,method,(jint)color.index());
    }
    else
    {
        // get constructor method id
        jmethodID method = jni->GetMethodID(cls,"<init>","()V");

        // construct color object
        return jni->NewObject(cls,method);
    }
}


Format jni_translate_format(JNIEnv *jni,jobject format)
{
    // get ptc.Format class
    jclass cls = jni->GetObjectClass(format);

    // get method ids
    jmethodID format_r       = jni->GetMethodID(cls,"r","()I");
    jmethodID format_g       = jni->GetMethodID(cls,"g","()I");
    jmethodID format_b       = jni->GetMethodID(cls,"b","()I");
    jmethodID format_a       = jni->GetMethodID(cls,"a","()I");
    jmethodID format_bits    = jni->GetMethodID(cls,"bits","()I");
    jmethodID format_direct  = jni->GetMethodID(cls,"direct","()Z");
    jmethodID format_indexed = jni->GetMethodID(cls,"indexed","()Z");

    // get format data
    int32 r  = (int32) jni->CallIntMethod(format,format_r);
    int32 g  = (int32) jni->CallIntMethod(format,format_g);
    int32 b  = (int32) jni->CallIntMethod(format,format_b);
    int32 a  = (int32) jni->CallIntMethod(format,format_a);
    int bits = (int)   jni->CallIntMethod(format,format_bits);
 
    // check format type
    if (jni->CallBooleanMethod(format,format_direct))
    {
        // direct color
        return Format(bits,r,g,b,a);
    }
    else if (jni->CallBooleanMethod(format,format_indexed))
    {
        // indexed color
        return Format(bits);
    }
    else
    {
        // default constructor
        return Format();
    }
}


jobject jni_translate_format(JNIEnv *jni,const Format &format)
{
    // get ptc.Format class
    jclass cls = jni->FindClass("ptc/Format");

    // check format type
    if (format.direct())
    {
        // get constructor method id
        jmethodID method = jni->GetMethodID(cls,"<init>","(IIIII)V");

        // construct format object
        return jni->NewObject(cls,method,(jint)format.bits(),(jint)format.r(),(jint)format.g(),(jint)format.b(),(jint)format.a());
    }
    else if (format.indexed())
    {
        // get constructor method id
        jmethodID method = jni->GetMethodID(cls,"<init>","(I)V");

        // construct format object
        return jni->NewObject(cls,method,(jint)format.bits());
    }
    else
    {
        // get constructor method id
        jmethodID method = jni->GetMethodID(cls,"<init>","()V");

        // construct format object
        return jni->NewObject(cls,method);
    }
}


Palette jni_translate_palette(JNIEnv *jni,jobject palette)
{
    // fudge!
    Palette pal;
    int32 *arse = pal.lock();
    for (int i=0; i<256; i++) arse[i] = i;
    pal.unlock();
    return pal;

    /*
    // get ptc.Palette class
    jclass cls = jni->GetObjectClass(palette);

    // get method ids
    jmethodID palette_lock   = jni->GetMethodID(cls,"lock","()[I");
    jmethodID palette_unlock = jni->GetMethodID(cls,"unlock","()V");
    
    // lock palette data
    jintArray data = (jintArray) jni->CallObjectMethod(palette,palette_lock);
    
    // copy flag
    jboolean copy;

    // lock java palette array
    int32 *array = (int32*) jni->GetIntArrayElements(data,&copy);

    // create palette object
    Palette translated(array);

    // unlock palette data
    jni->CallObjectMethod(palette,palette_unlock);

    // return palette
    return translated;
    */
}


jobject jni_translate_palette(JNIEnv *jni,const Palette &palette)
{
    // get ptc.Palette class
    jclass cls = jni->FindClass("ptc/Palette");

    // get constructor method id
    jmethodID method = jni->GetMethodID(cls,"<init>","([I)V");

    // construct palette object
    return jni->NewObject(cls,method,(jint*)palette.data());
}


jstring jni_translate_string(JNIEnv *jni,const char string[])
{
    // translate c string to java string
    return jni->NewStringUTF(string);
}


const char* jni_translate_string(JNIEnv *jni,jstring string)
{
    // copy flag
    jboolean copy;

    // translate java string to c string
    return jni->GetStringUTFChars(string,&copy);
}




#endif
