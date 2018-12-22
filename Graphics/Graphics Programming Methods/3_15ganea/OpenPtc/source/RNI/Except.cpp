//
// RNI Exception routines for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// configuration
#include "ptc.h"

#ifdef __PTC_RNI__

// include files
#include "RNI/Except.h"




void rni_throw(const Error &error)
{
    // check rni library
    if (!rni_check_library())
    {
        // library is not loaded and an exception was thrown!
        MessageBox(0,error.message(),"Fatal RNI Error",MB_OK | MB_ICONERROR | MB_SETFOREGROUND | MB_TOPMOST);

        // exit
        exit(1);
    }
    else
    {
        // create error string
        Hjava_lang_String* string = rni_makeJavaStringFromUtf8(error.message());

        // construct ptc.Error object
        HObject *object = rni_execute_java_constructor(0,"ptc/Error",0,"(Ljava/lang/String;)",string);

        // set java exception
        rni_exceptionSet(0,object);
    }
}




#endif
