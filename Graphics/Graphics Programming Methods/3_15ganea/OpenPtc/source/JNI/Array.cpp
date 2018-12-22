//
// JNI Array routines for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// configuration
#include "ptc.h"

#ifdef __PTC_JNI__

// include files
#include "JNI/Array.h"




JNI_JAVATYPE jni_type(JNIEnv *jni,jobject object)
{
    // get java array classes
    jclass jbyte_class  = jni->FindClass("[B");
    jclass jshort_class = jni->FindClass("[S");
    jclass jint_class   = jni->FindClass("[I");
    jclass jlong_class  = jni->FindClass("[J");
    
    // return array type id
    if (jni->IsInstanceOf(object,jbyte_class))  return JNI_BYTEARRAY;
    if (jni->IsInstanceOf(object,jshort_class)) return JNI_SHORTARRAY;
    if (jni->IsInstanceOf(object,jint_class))   return JNI_INTARRAY;
    if (jni->IsInstanceOf(object,jlong_class))  return JNI_LONGARRAY;

    // unknown type
    return JNI_UNKNOWN;
}


void* jni_lock(JNIEnv *jni,jobject array,jboolean &copy)
{
    // get java array type id
    int id = jni_type(jni,array);
    
    // data pointer
    void *data = 0;

    // lock array
    switch (id)
    {
        case JNI_BYTEARRAY:  data = jni->GetByteArrayElements((jbyteArray)array,&copy);   break;
        case JNI_SHORTARRAY: data = jni->GetShortArrayElements((jshortArray)array,&copy); break;
        case JNI_INTARRAY:   data = jni->GetIntArrayElements((jintArray)array,&copy);     break;
        case JNI_LONGARRAY:  data = jni->GetLongArrayElements((jlongArray)array,&copy);   break;
        default:             throw Error("invalid java array type");
    }
    
    // success
    return data;
}


void jni_unlock(JNIEnv *jni,jobject array,void *data,jboolean copy)
{
    // check copy
    if (copy)
    {
        // get java array type id
        int id = jni_type(jni,array);
    
        // release java array
        switch (id)
        {
            case JNI_BYTEARRAY:  jni->ReleaseByteArrayElements((jbyteArray)array,(jbyte*)data,0);    break;
            case JNI_SHORTARRAY: jni->ReleaseShortArrayElements((jshortArray)array,(jshort*)data,0); break;
            case JNI_INTARRAY:   jni->ReleaseIntArrayElements((jintArray)array,(jint*)data,0);       break;
            case JNI_LONGARRAY:  jni->ReleaseLongArrayElements((jlongArray)array,(jlong*)data,0);    break;
        }
    }
}




#endif
