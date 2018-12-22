//
// JNI Clear class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// configuration
#include "ptc.h"

#ifdef __PTC_JNI__

// include files
#include "JNI/Clear.h"
#include "JNI/Array.h"
#include "JNI/Except.h"
#include "JNI/Translate.h"




JNIEXPORT jlong JNICALL Java_ptc_jni_Clear_create(JNIEnv *jni,jobject java)
{
    try
    {
        // create clear object
        return (jlong) new Clear();
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


JNIEXPORT void JNICALL Java_ptc_jni_Clear_destroy(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // destroy clear object
        delete (Clear*) object;
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


JNIEXPORT void JNICALL Java_ptc_jni_Clear_request(JNIEnv *jni,jobject java,jlong object,jobject format)
{
    try
    {
        // request clear
        ((Clear*)object)->request(jni_translate_format(jni,format));
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


JNIEXPORT void JNICALL Java_ptc_jni_Clear_clear(JNIEnv *jni,jobject java,jlong object,jobject pixels,jint x,jint y,jint width,jint height,jint pitch,jobject color)
{
    try
    {
        // copy flag
        jboolean copy;

        // lock java pixel array
        void *array = jni_lock(jni,pixels,copy);

        // clear pixels
        ((Clear*)object)->clear(array,x,y,width,height,pitch,jni_translate_color(jni,color));

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




#endif
