//
// JNI Console class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// configuration
#include "ptc.h"

#ifdef __PTC_JNI__

// include files
#include "JNI/Array.h"
#include "JNI/Except.h"
#include "JNI/Console.h"
#include "JNI/Translate.h"




JNIEXPORT jlong JNICALL Java_ptc_jni_Console_create(JNIEnv *jni,jobject java)
{
    try
    {
        // create console object
        return (jlong) new Console();
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }

    // failure
    return 0;
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_destroy(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // destroy console object
        delete (Console*) object;
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_configure(JNIEnv *jni,jobject java,jlong object,jstring file)
{
    try
    {
        // configure console
        ((Console*)object)->configure(jni_translate_string(jni,file));
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT jboolean JNICALL Java_ptc_jni_Console_option(JNIEnv *jni,jobject java,jlong object,jstring option)
{
    try
    {
        // console option string
        return (jboolean) ((Console*)object)->option(jni_translate_string(jni,option));
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }

    // failure
    return 0;
}


JNIEXPORT jobjectArray JNICALL Java_ptc_jni_Console_modes(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // get console modes
        const Mode *modes = ((Console*)object)->modes();

        // count the number of modes
        int count = 0;
        while (modes[count].valid()) count++;

        // get ptc.Mode class
        jclass cls = jni->FindClass("ptc/Mode");

        // create a default java mode object
        jobject mode = jni_translate_mode(jni,Mode());

        // create an array of java mode objects
        jobjectArray java_modes = jni->NewObjectArray(count+1,cls,mode);

        // initialize the java mode aray
        for (int i=0; i<count; i++)
        {
            // initialize java mode to corresponding c++ mode
            jni->SetObjectArrayElement(java_modes,i,jni_translate_mode(jni,modes[i]));
        }

        // return java modes
        return java_modes;
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }

    // failure
    return 0;
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_open__JLjava_lang_String_2I(JNIEnv *jni,jobject java,jlong object,jstring title,jint pages)
{
    try
    {
        // open console
        ((Console*)object)->open(jni_translate_string(jni,title),pages);
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}
                                                                                                              

JNIEXPORT void JNICALL Java_ptc_jni_Console_open__JLjava_lang_String_2Lptc_Format_2I(JNIEnv *jni,jobject java,jlong object,jstring title,jobject format,jint pages)
{
    try
    {
        // open console
        ((Console*)object)->open(jni_translate_string(jni,title),jni_translate_format(jni,format),pages);
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_open__JLjava_lang_String_2IILptc_Format_2I(JNIEnv *jni,jobject java,jlong object,jstring title,jint width,jint height,jobject format,jint pages)
{
    try
    {
        // open console
        ((Console*)object)->open(jni_translate_string(jni,title),width,height,jni_translate_format(jni,format),pages);
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_open__JLjava_lang_String_2Lptc_Mode_2I(JNIEnv *jni,jobject java,jlong object,jstring title,jobject mode,jint pages)
{
    try
    {
        // open console
        ((Console*)object)->open(jni_translate_string(jni,title),jni_translate_mode(jni,mode),pages);
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_close(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // close console
        ((Console*)object)->close();
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_flush(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // flush console
        ((Console*)object)->flush();
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_finish(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // finish console
        ((Console*)object)->finish();
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_update__J(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // update console
        ((Console*)object)->update();
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_update__JLptc_Area_2(JNIEnv *jni,jobject java,jlong object,jobject area)
{
    try
    {
        // update console area
        ((Console*)object)->update(jni_translate_area(jni,area));
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT jboolean JNICALL Java_ptc_jni_Console_key(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // check for key press
        return (jboolean) ((Console*)object)->key();
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }

    // failure
    return 0;
}


JNIEXPORT jobject JNICALL Java_ptc_jni_Console_read(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // read key press
        Key key = ((Console*)object)->read();

        // translate c++ key to java key
        return jni_translate_key(jni,key);
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }

    // failure
    return 0;
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_copy__JLptc_base_Surface_2(JNIEnv *jni,jobject java,jlong object,jobject surface)
{
    try
    {
        // not implemented!
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_copy__JLptc_base_Surface_2Lptc_Area_2Lptc_Area_2(JNIEnv *jni,jobject java,jlong object,jobject surface,jobject source,jobject destination)
{
    try
    {
        // not implemented!
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT jobject JNICALL Java_ptc_jni_Console_lock(JNIEnv *jni,jobject java,jlong object)
{
    // console lock is not supported
    jni_throw(jni,Error("console lock is not supported"));

    // failure
    return 0;
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_unlock(JNIEnv *jni,jobject java,jlong object)
{
    // console unlock is not supported
    jni_throw(jni,Error("console unlock is not supported"));
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_load__JLjava_lang_Object_2IIILptc_Format_2Lptc_Palette_2(JNIEnv *jni,jobject java,jlong object,jobject pixels,jint width,jint height,jint pitch,jobject format,jobject palette)
{
    try
    {
        // copy flag
        jboolean copy;

        // lock java pixel array
        void *array = jni_lock(jni,pixels,copy);

        // load pixels to console
        ((Console*)object)->load(array,width,height,pitch,jni_translate_format(jni,format),jni_translate_palette(jni,palette));
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_load__JLjava_lang_Object_2IIILptc_Format_2Lptc_Palette_2Lptc_Area_2Lptc_Area_2(JNIEnv *jni,jobject java,jlong object,jobject pixels,jint width,jint height,jint pitch,jobject format,jobject palette,jobject source,jobject destination)
{
    try
    {
        // copy flag
        jboolean copy;

        // lock java pixel array
        void *array = jni_lock(jni,pixels,copy);

        // load pixels to console
        ((Console*)object)->load(array,width,height,pitch,jni_translate_format(jni,format),jni_translate_palette(jni,palette),jni_translate_area(jni,source),jni_translate_area(jni,destination));
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_save__JLjava_lang_Object_2IIILptc_Format_2Lptc_Palette_2(JNIEnv *jni,jobject java,jlong object,jobject pixels,jint width,jint height,jint pitch,jobject format,jobject palette)
{
    try
    {
        // copy flag
        jboolean copy;

        // lock java pixel array
        void *array = jni_lock(jni,pixels,copy);

        // save console pixels
        ((Console*)object)->save(array,width,height,pitch,jni_translate_format(jni,format),jni_translate_palette(jni,palette));

        // unlock java pixel array
        jni_unlock(jni,pixels,array,copy);

    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_save__JLjava_lang_Object_2IIILptc_Format_2Lptc_Palette_2Lptc_Area_2Lptc_Area_2(JNIEnv *jni,jobject java,jlong object,jobject pixels,jint width,jint height,jint pitch,jobject format,jobject palette,jobject source,jobject destination)
{
    try
    {
        // copy flag
        jboolean copy;

        // lock java pixel array
        void *array = jni_lock(jni,pixels,copy);

        // save console area pixels
        ((Console*)object)->save(array,width,height,pitch,jni_translate_format(jni,format),jni_translate_palette(jni,palette),jni_translate_area(jni,source),jni_translate_area(jni,destination));

        // unlock java pixel array
        jni_unlock(jni,pixels,array,copy);
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_clear__J(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // clear console
        ((Console*)object)->clear();
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_clear__JLptc_Color_2(JNIEnv *jni,jobject java,jlong object,jobject color)
{
    try
    {
        // clear console color
        ((Console*)object)->clear(jni_translate_color(jni,color));
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_clear__JLptc_Color_2Lptc_Area_2(JNIEnv *jni,jobject java,jlong object,jobject color,jobject area)
{
    try
    {
        // clear console color area
        ((Console*)object)->clear(jni_translate_color(jni,color),jni_translate_area(jni,area));
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_palette__JLptc_Palette_2(JNIEnv *jni,jobject java,jlong object,jobject palette)
{
    try
    {
        // set console palette
        ((Console*)object)->palette(jni_translate_palette(jni,palette));
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT jobject JNICALL Java_ptc_jni_Console_palette__J(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // get console palette
        return jni_translate_palette(jni,((Console*)object)->palette());
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }

    // failure
    return 0;
}


JNIEXPORT void JNICALL Java_ptc_jni_Console_clip__JLptc_Area_2(JNIEnv *jni,jobject java,jlong object,jobject area)
{
    try
    {
        // set console clip area
        ((Console*)object)->clip(jni_translate_area(jni,area));
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }
}


JNIEXPORT jint JNICALL Java_ptc_jni_Console_width(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // get console width
        return ((Console*)object)->width();
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }

    // failure
    return 0;
}


JNIEXPORT jint JNICALL Java_ptc_jni_Console_height(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // get console height
        return ((Console*)object)->height();
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }

    // failure
    return 0;
}


JNIEXPORT jint JNICALL Java_ptc_jni_Console_pages(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // get console pages
        return ((Console*)object)->pages();
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }

    // failure
    return 0;
}


JNIEXPORT jint JNICALL Java_ptc_jni_Console_pitch(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // get console pitch
        return ((Console*)object)->pitch();
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }

    // failure
    return 0;
}


JNIEXPORT jobject JNICALL Java_ptc_jni_Console_area(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // get console area
        return jni_translate_area(jni,((Console*)object)->area());
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }

    // failure
    return 0;
}


JNIEXPORT jobject JNICALL Java_ptc_jni_Console_clip__J(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // get console clip area
        return jni_translate_area(jni,((Console*)object)->clip());
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }

    // failure
    return 0;
}


JNIEXPORT jobject JNICALL Java_ptc_jni_Console_format(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // get console format
        return jni_translate_format(jni,((Console*)object)->format());
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }

    // failure
    return 0;
}


JNIEXPORT jstring JNICALL Java_ptc_jni_Console_name(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // get console name
        return jni_translate_string(jni,((Console*)object)->name());
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }

    // failure
    return 0;
}


JNIEXPORT jstring JNICALL Java_ptc_jni_Console_title(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // get console title
        return jni_translate_string(jni,((Console*)object)->title());
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }

    // failure
    return 0;
}


JNIEXPORT jstring JNICALL Java_ptc_jni_Console_information(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // get console information
        return jni_translate_string(jni,((Console*)object)->information());
    }
    catch (Error &error)
    {
        // throw jni error
        jni_throw(jni,error);
    }
    catch (...)
    {
        // unknown
        jni_throw(jni);
    }

    // failure
    return 0;
}




#endif
