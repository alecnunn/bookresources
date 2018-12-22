//
// RNI Library routines for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_RNI_LIBRARY_H
#define __PTC_RNI_LIBRARY_H

// include files
#include <windows.h>
#include <native.h>
#include "ptc.h"




// library management
void rni_load_library();
void rni_free_library();
bool rni_check_library();


// imported rni library functions
extern HObject* (__cdecl *rni_execute_java_constructor)(ExecEnv *ee, const char *classname,ClassClass *cb, const char *signature,...);
extern void (__cdecl *rni_exceptionSet)( ExecEnv *ee, HObject *phThrowable );
extern Hjava_lang_String* (__cdecl *rni_makeJavaStringFromUtf8)(const char *pszUtf8);
extern char* (__cdecl *rni_javaString2CString)(Hjava_lang_String *s, char *buf, int buflen);
extern long (__cdecl *rni_execute_java_dynamic_method)(ExecEnv *ee, HObject *obj, const char *method_name, const char *signature, ...);
extern int64_t (__cdecl *rni_execute_java_dynamic_method64)(ExecEnv *ee, HObject *obj, const char *method_name, const char *signature, ...);
extern int64_t (__cdecl *rni_execute_java_dynamic_methodV)(ExecEnv *ee, HObject *obj, const char *method_name, const char *signature, va_list args);
extern HObject* (__cdecl *rni_ArrayAlloc)(int type, int cItems);
extern HObject* (__cdecl *rni_ClassArrayAlloc)(int type, int cItems, const char *szSig);




#endif
