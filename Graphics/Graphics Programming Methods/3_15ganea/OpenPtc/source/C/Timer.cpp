//
// C Timer class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include "C/Timer.h"
#include "C/Except.h"
#include "Core/Timer.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




CAPI PTC_TIMER PTCAPI ptc_timer_create()
{
    try
    {
        // create timer object
        return (PTC_TIMER) new Timer();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI void PTCAPI ptc_timer_destroy(PTC_TIMER object)
{
    try
    {
        // destroy timer object
        delete (Timer*) object;
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void PTCAPI ptc_timer_set(PTC_TIMER object,double time)
{
    try
    {
        // set timer time
        ((Timer*)object)->set(time);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI void PTCAPI ptc_timer_start(PTC_TIMER object)
{
    try
    {
        // start timer
        ((Timer*)object)->start();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI void PTCAPI ptc_timer_stop(PTC_TIMER object)
{
    try
    {
        // stop timer
        ((Timer*)object)->stop();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}




CAPI double PTCAPI ptc_timer_time(PTC_TIMER object)
{
    try
    {
        // get time
        return ((Timer*)object)->time();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI double PTCAPI ptc_timer_delta(PTC_TIMER object)
{
    try
    {
        // get delta
        return ((Timer*)object)->delta();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}


CAPI double PTCAPI ptc_timer_resolution(PTC_TIMER object)
{
    try
    {
        // get resolution
        return ((Timer*)object)->resolution();
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}




CAPI void PTCAPI ptc_timer_assign(PTC_TIMER object,PTC_TIMER timer)
{
    try
    {
        // assign object = timer
        *((Timer*)object) = *((Timer*)timer);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);
    }
}


CAPI int PTCAPI ptc_timer_equals(PTC_TIMER object,PTC_TIMER timer)
{
    try
    {
        // test object == timer
        return *((Timer*)object) == *((Timer*)timer);
    }
    catch (Error &error)
    {
        // handle ptc exception
        ptc_exception_handle(error);

        // failure
        return 0;
    }
}
