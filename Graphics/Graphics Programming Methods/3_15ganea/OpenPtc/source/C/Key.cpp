//
// C Key class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "C/Key.h"
#include "C/Except.h"
#include "Core/Key.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




CAPI PTC_KEY PTCAPI ptc_key_create(int code,int alt,int shift,int control)
{
    try
    {
        // create key object
        return (PTC_KEY) new Key(code,(alt==1),(shift==1),(control==1));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI void PTCAPI ptc_key_destroy(PTC_KEY object)
{
    try
    {
        // destroy key object
        delete (Key*) object;
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI int PTCAPI ptc_key_code(PTC_KEY object)
{
    try
    {
        // get key code
        return ((Key*)object)->code();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}




CAPI int PTCAPI ptc_key_alt(PTC_KEY object)
{
    try
    {
        // get alt modifier
        return ((Key*)object)->alt();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI int PTCAPI ptc_key_shift(PTC_KEY object)
{
    try
    {
        // get shift modifier
        return ((Key*)object)->shift();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI int PTCAPI ptc_key_control(PTC_KEY object)
{
    try
    {
        // get control modifier
        return ((Key*)object)->control();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}




CAPI void PTCAPI ptc_key_assign(PTC_KEY object,PTC_KEY key)
{
    try
    {
        // assign object = key
        *((Key*)object) = *((Key*)key);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI int PTCAPI ptc_key_equals(PTC_KEY object,PTC_KEY key)
{
    try
    {
        // test object == key
        return *((Key*)object) == *((Key*)key);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}
