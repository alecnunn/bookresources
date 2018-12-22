//
// JNI Exception routines for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// configuration
#include "ptc.h"

#ifdef __PTC_JNI__

// include files
#include "JNI/Except.h"




void jni_throw(JNIEnv *jni,const Error &error)
{
    // get ptc.Error class
    jclass cls = jni->FindClass("ptc/Error");

    // get constructor method id
    jmethodID method = jni->GetMethodID(cls,"<init>","(Ljava/lang/String;)V");

    // create error string
    jstring string = jni->NewStringUTF(error.message());

    // construct exception object
    jthrowable object = (jthrowable) jni->NewObject(cls,method,string);

    // throw error
    jni->Throw(object);
}




#endif
