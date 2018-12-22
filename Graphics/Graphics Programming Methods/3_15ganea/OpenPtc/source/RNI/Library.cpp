//
// RNI Library routines for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// configuration
#include "ptc.h"

#ifdef __PTC_RNI__

// include files
#include "RNI/Library.h"




DWORD __cdecl RNIGetCompatibleVersion()
{
    // version id
    return RNIVER;
}


// imported rni library functions
HObject* (__cdecl *rni_execute_java_constructor)(ExecEnv *ee, const char *classname,ClassClass *cb, const char *signature,...) = 0;
void (__cdecl *rni_exceptionSet)( ExecEnv *ee, HObject *phThrowable ) = 0;
Hjava_lang_String* (__cdecl *rni_makeJavaStringFromUtf8)(const char *pszUtf8) = 0;
char* (__cdecl *rni_javaString2CString)(Hjava_lang_String *s, char *buf, int buflen) = 0;
long (__cdecl *rni_execute_java_dynamic_method)(ExecEnv *ee, HObject *obj, const char *method_name, const char *signature, ...) = 0;
int64_t (__cdecl *rni_execute_java_dynamic_method64)(ExecEnv *ee, HObject *obj, const char *method_name, const char *signature, ...) = 0;
int64_t (__cdecl *rni_execute_java_dynamic_methodV)(ExecEnv *ee, HObject *obj, const char *method_name, const char *signature, va_list args) = 0;
HObject* (__cdecl *rni_ArrayAlloc)(int type, int cItems) = 0;
HObject* (__cdecl *rni_ClassArrayAlloc)(int type, int cItems, const char *szSig) = 0;

// "msjava.dll" library
static HMODULE rni_library = 0;

// library reference count
static int rni_library_reference_count = 0;


void rni_import(HMODULE module,void **function,char name[])
{
	// import function
	*function = (void*) GetProcAddress(module,name);

	// check import 
	if (!*function) throw Error("could not import msjava function");
}


void rni_load_library()
{
    try
    {
        // increase reference count
        rni_library_reference_count ++;

        // check library reference count
        if (rni_library_reference_count==1)
        {
            // load rni library (msjava.dll)
            rni_library = LoadLibrary("msjava.dll");
            if (!rni_library) throw Error("could not load msjava.dll");

            // import rni functions
            rni_import(rni_library,(void**)&rni_execute_java_constructor,"execute_java_constructor");
            rni_import(rni_library,(void**)&rni_exceptionSet,"exceptionSet");
            rni_import(rni_library,(void**)&rni_makeJavaStringFromUtf8,"makeJavaStringFromUtf8");
            rni_import(rni_library,(void**)&rni_javaString2CString,"javaString2CString");
            rni_import(rni_library,(void**)&rni_execute_java_dynamic_method,"execute_java_dynamic_method");
            rni_import(rni_library,(void**)&rni_execute_java_dynamic_method64,"execute_java_dynamic_method64");
            rni_import(rni_library,(void**)&rni_execute_java_dynamic_methodV,"execute_java_dynamic_methodV");
            rni_import(rni_library,(void**)&rni_ArrayAlloc,"ArrayAlloc");
            rni_import(rni_library,(void**)&rni_ClassArrayAlloc,"ClassArrayAlloc");
        }
    }
    catch (Error &error)
    {
        // free rni library
        rni_free_library();

        // rethrow
        throw error;
    }
}


void rni_free_library()
{
    // check library reference count
    if (rni_library_reference_count==1)
    {
        // check library
        if (rni_library)
        {
            // free rni library
            FreeLibrary(rni_library);

            // clear library
            rni_library = 0;
        }

        // clear imported functions
        rni_execute_java_constructor = 0;
        rni_exceptionSet = 0;
        rni_makeJavaStringFromUtf8 = 0;
        rni_javaString2CString = 0;
        rni_execute_java_dynamic_method = 0;
        rni_execute_java_dynamic_method64 = 0;
        rni_execute_java_dynamic_methodV = 0;
        rni_ArrayAlloc = 0;
        rni_ClassArrayAlloc = 0;
    }

    // decrease library reference count
    if (rni_library_reference_count>0) rni_library_reference_count--;
}


bool rni_check_library()
{
    // check library load
    return (rni_library!=0);
}




#endif
