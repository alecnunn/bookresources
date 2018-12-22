//
// JNI Copy class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// configuration
#include "ptc.h"

#ifdef __PTC_JNI__

// include files
#include "JNI/Copy.h"
#include "JNI/Except.h"




JNIEXPORT jlong JNICALL Java_ptc_jni_Copy_create(JNIEnv *jni,jobject java)
{
    try
    {
        // create copy object
        return (jlong) new Copy();
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


JNIEXPORT void JNICALL Java_ptc_jni_Copy_destroy(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // destroy copy object
        delete (Copy*) object;
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


JNIEXPORT void JNICALL Java_ptc_jni_Copy_request(JNIEnv *jni,jobject java,jlong object,jobject source,jobject destination)
{
    try
    {
        // request copy
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


JNIEXPORT void JNICALL Java_ptc_jni_Copy_palette(JNIEnv *jni,jobject java,jlong object,jobject source,jobject destination)
{
    try
    {
        // set copy palettes
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


JNIEXPORT void JNICALL Java_ptc_jni_Copy_copy(JNIEnv *jni,jobject java,jlong object,jobject pixels,jint source_x,jint source_y,jint source_width,jint source_height,jint source_pitch,jobject destination_pixels,jint destination_x,jint destination_y,jint destination_width,jint destination_height,jint destination_pitch)
{
    try
    {
        // copy pixels
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


JNIEXPORT jboolean JNICALL Java_ptc_jni_Copy_option(JNIEnv *jni,jobject java,jlong object,jstring option)
{
    try
    {
        // set copy option
        return false;
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




#endif
