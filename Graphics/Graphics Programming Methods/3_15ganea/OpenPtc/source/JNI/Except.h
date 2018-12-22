//
// JNI Exception routines for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_JNI_EXCEPT_H
#define __PTC_JNI_EXCEPT_H

// include files
#include <jni.h>
#include "ptc.h"




// throw a jni java exception
void jni_throw(JNIEnv *jni,const Error &error = Error("unknown jni exception"));




#endif
