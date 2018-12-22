//
// RNI Timer class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// configuration
#include "ptc.h"

#ifdef __PTC_RNI__

// include files
#include <windows.h>
#include "RNI/Timer.h"
#include "RNI/Except.h"
#include "RNI/Library.h"




__declspec(dllexport) int64_t __cdecl ptc_rni_Timer_create(struct Hptc_rni_Timer *java)
{
    try
    {
        // load rni library
        rni_load_library();

        // create timer object
        return (int64_t) new Timer();
    }
    catch (Error &error)
    {
        // throw rni error
        rni_throw(error);
    }
    catch (...)
    {
        // unknown
        rni_throw();
    }

    // failure
    return 0;
}


__declspec(dllexport) void __cdecl ptc_rni_Timer_destroy(struct Hptc_rni_Timer *java,int64_t object)
{
    try
    {
        // destroy timer object
        delete (Timer*) object;

        // free rni library
        rni_free_library();
    }
    catch (Error &error)
    {
        // throw rni error
        rni_throw(error);
    }
    catch (...)
    {
        // unknown
        rni_throw();
    }
}


__declspec(dllexport) void __cdecl ptc_rni_Timer_set(struct Hptc_rni_Timer *java,int64_t object,double time)
{
    try
    {
        // set time
        ((Timer*)object)->set(time);
    }
    catch (Error &error)
    {
        // throw rni error
        rni_throw(error);
    }
    catch (...)
    {
        // unknown
        rni_throw();
    }
}


__declspec(dllexport) void __cdecl ptc_rni_Timer_start(struct Hptc_rni_Timer *java,int64_t object)
{
    try
    {
        // start timer
        ((Timer*)object)->start();
    }
    catch (Error &error)
    {
        // throw rni error
        rni_throw(error);
    }
    catch (...)
    {
        // unknown
        rni_throw();
    }
}


__declspec(dllexport) void __cdecl ptc_rni_Timer_stop(struct Hptc_rni_Timer *java,int64_t object)
{
    try
    {
        // stop timer
        ((Timer*)object)->stop();
    }
    catch (Error &error)
    {
        // throw rni error
        rni_throw(error);
    }
    catch (...)
    {
        // unknown
        rni_throw();
    }
}


__declspec(dllexport) double __cdecl ptc_rni_Timer_time(struct Hptc_rni_Timer *java,int64_t object)
{
    try
    {
        // get time
        return ((Timer*)object)->time();
    }
    catch (Error &error)
    {
        // throw rni error
        rni_throw(error);
    }
    catch (...)
    {
        // unknown
        rni_throw();
    }

    // failure
    return 0;
}


__declspec(dllexport) double __cdecl ptc_rni_Timer_delta(struct Hptc_rni_Timer *java,int64_t object)
{
    try
    {
        // get delta
        return ((Timer*)object)->delta();
    }
    catch (Error &error)
    {
        // throw rni error
        rni_throw(error);
    }
    catch (...)
    {
        // unknown
        rni_throw();
    }

    // failure
    return 0;
}


__declspec(dllexport) double __cdecl ptc_rni_Timer_resolution(struct Hptc_rni_Timer *java,int64_t object)
{
    try
    {
        // get resolution
        return ((Timer*)object)->resolution();
    }
    catch (Error &error)
    {
        // throw rni error
        rni_throw(error);
    }
    catch (...)
    {
        // unknown
        rni_throw();
    }

    // failure
    return 0;
}




#endif
