//
// C Copy class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org) 
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "C/Copy.h"
#include "C/Except.h"
#include "Core/Copy.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




CAPI PTC_COPY PTCAPI ptc_copy_create()
{
    try
    {
        // create copy object
        return (PTC_COPY) new Copy();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI void PTCAPI ptc_copy_destroy(PTC_COPY object)
{
    try
    {
        // destroy copy object
        delete (Copy*) object;
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void PTCAPI ptc_copy_request(PTC_COPY object,PTC_FORMAT source,PTC_FORMAT destination)
{
    try
    {
        // request convert
        ((Copy*)object)->request(*((Format*)source),*((Format*)destination));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void PTCAPI ptc_copy_palette(PTC_COPY object,PTC_PALETTE source,PTC_PALETTE destination)
{   
    try
    {
        // set palettes
        ((Copy*)object)->palette(*((Palette*)source),*((Palette*)destination));
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_copy_copy(PTC_COPY object,void *source_pixels,int source_x,int source_y,int source_width,int source_height,int source_pitch,void *destination_pixels,int destination_x,int destination_y,int destination_width,int destination_height,int destination_pitch)
{
    try
    {
        // copy pixels
        ((Copy*)object)->copy(source_pixels,source_x,source_y,source_width,source_height,source_pitch,destination_pixels,destination_x,destination_y,destination_width,destination_height,destination_pitch);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI int PTCAPI ptc_copy_option(PTC_COPY object,char *option)
{
    try
    {
        // copy option
        return (int) ((Copy*)object)->option(option);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}
