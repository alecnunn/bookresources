//
// JNI Translate routines for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_JNI_TRANSLATE_H
#define __PTC_JNI_TRANSLATE_H

// include files
#include <jni.h>
#include "ptc.h"




// key translation
Key jni_translate_key(JNIEnv *jni,jobject key);
jobject jni_translate_key(JNIEnv *jni,const Key &key);

// area translation
Area jni_translate_area(JNIEnv *jni,jobject area);
jobject jni_translate_area(JNIEnv *jni,const Area &area);

// mode translation
Mode jni_translate_mode(JNIEnv *jni,jobject mode);
jobject jni_translate_mode(JNIEnv *jni,const Mode &mode);

// color translation
Color jni_translate_color(JNIEnv *jni,jobject color);
jobject jni_translate_color(JNIEnv *jni,const Color &color);

// format translation
Format jni_translate_format(JNIEnv *jni,jobject format);
jobject jni_translate_format(JNIEnv *jni,const Format &format);

// palette translation
Palette jni_translate_palette(JNIEnv *jni,jobject palette);
jobject jni_translate_palette(JNIEnv *jni,const Palette &palette);

// string translation
jstring jni_translate_string(JNIEnv *jni,const char string[]);
const char* jni_translate_string(JNIEnv *jni,jstring string);




#endif
