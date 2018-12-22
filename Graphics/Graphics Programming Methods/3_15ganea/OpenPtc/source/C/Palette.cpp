//
// C Palette class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "C/Except.h"
#include "C/Palette.h"
#include "Core/Palette.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




CAPI PTC_PALETTE PTCAPI ptc_palette_create()
{
    try
    {
        // create palette object
        return (PTC_PALETTE) new Palette();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI void PTCAPI ptc_palette_destroy(PTC_PALETTE object)
{
    try
    {
        // destroy palette object
        delete (Palette*) object;
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI ptc_int32* PTCAPI ptc_palette_lock(PTC_PALETTE object)
{
    try
    {
        // lock palette
        return ((Palette*)object)->lock();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI void PTCAPI ptc_palette_unlock(PTC_PALETTE object)
{
    try
    {
        // unlock palette
        ((Palette*)object)->unlock();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void PTCAPI ptc_palette_load(PTC_PALETTE object,ptc_int32 *data)
{
    try
    {
        // load palette data
        ((Palette*)object)->load(data);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_palette_save(PTC_PALETTE object,ptc_int32 *data)
{
    try
    {
        // save palette data
        ((Palette*)object)->save(data);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI ptc_int32* PTCAPI ptc_palette_data(PTC_PALETTE object)
{
    try
    {
        // get palette data
        return (ptc_int32*) ((Palette*)object)->data();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}




CAPI void PTCAPI ptc_palette_assign(PTC_PALETTE object,PTC_PALETTE palette)
{
    try
    {
        // assign object = palette
        *((Palette*)object) = *((Palette*)palette);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI int PTCAPI ptc_palette_equals(PTC_PALETTE object,PTC_PALETTE palette)
{
    try
    {
        // test object == palette
        return *((Palette*)object) == *((Palette*)palette);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}
