//
// JNI Array routines for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_JNI_ARRAY_H
#define __PTC_JNI_ARRAY_H

// include files
#include <jni.h>
#include "ptc.h"




// java type constants
enum JNI_JAVATYPE { JNI_UNKNOWN, JNI_BYTEARRAY, JNI_SHORTARRAY, JNI_INTARRAY, JNI_LONGARRAY };

// java type identification
JNI_JAVATYPE jni_type(JNIEnv *jni,jobject object);

// java array locking functions
void* jni_lock(JNIEnv *jni,jobject array,jboolean &copy);
void jni_unlock(JNIEnv *jni,jobject array,void *data,jboolean copy);




#endif
