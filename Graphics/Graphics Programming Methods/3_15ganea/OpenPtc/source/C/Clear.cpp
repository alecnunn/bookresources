//
// C Clear class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "C/Clear.h"
#include "C/Except.h"
#include "Core/Clear.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




CAPI PTC_CLEAR PTCAPI ptc_clear_create()
{
    try
    {
        // create clear object
        return (PTC_CLEAR) new Clear();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI void PTCAPI ptc_clear_destroy(PTC_CLEAR object)
{
    try
    {
        // destroy clear object
        delete (Clear*) object;
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void PTCAPI ptc_clear_request(PTC_CLEAR object,PTC_FORMAT format)
{
    try
    {
        // request clear
        ((Clear*)object)->request(*((Format*)format));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void PTCAPI ptc_clear_clear(PTC_CLEAR object,void *pixels,int x,int y,int width,int height,int pitch,PTC_COLOR color)
{
    try
    {
        // clear pixels
        ((Clear*)object)->clear(pixels,x,y,width,height,pitch,*((Color*)color));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}
