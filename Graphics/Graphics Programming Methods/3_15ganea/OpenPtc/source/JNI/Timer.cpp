//
// JNI Timer class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// configuration
#include "ptc.h"

#ifdef __PTC_JNI__

// include files
#include "JNI/Timer.h"
#include "JNI/Except.h"




JNIEXPORT jlong JNICALL Java_ptc_jni_Timer_create(JNIEnv *jni,jobject java)
{
    try
    {
        // create timer object
        return (jlong) new Timer();
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


JNIEXPORT void JNICALL Java_ptc_jni_Timer_destroy(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // destroy timer object
        delete (Timer*) object;
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


JNIEXPORT void JNICALL Java_ptc_jni_Timer_set(JNIEnv *jni,jobject java,jlong object,jdouble time)
{
    try
    {
        // set timer
        ((Timer*)object)->set((double)time);
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


JNIEXPORT void JNICALL Java_ptc_jni_Timer_start(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // start timer
        ((Timer*)object)->start();
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

                                                            
JNIEXPORT void JNICALL Java_ptc_jni_Timer_stop(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // stop timer
        ((Timer*)object)->stop();
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


JNIEXPORT jdouble JNICALL Java_ptc_jni_Timer_time(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // get time
        return (jdouble) ((Timer*)object)->time();
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


JNIEXPORT jdouble JNICALL Java_ptc_jni_Timer_delta(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // get delta
        return (jdouble) ((Timer*)object)->delta();
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


JNIEXPORT jdouble JNICALL Java_ptc_jni_Timer_resolution(JNIEnv *jni,jobject java,jlong object)
{
    try
    {
        // get resolution
        return (jdouble) ((Timer*)object)->resolution();
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
