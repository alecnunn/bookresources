//
// Dynamic Timer class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_DYNAMIC_TIMER_H
#define __PTC_DYNAMIC_TIMER_H

// include files
#include "config.h"
#include "C/Timer.h"
#include "C/Extra.h"
#include "Core/Types.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class DynamicTimer
{
    public:

        DynamicTimer()
        {
            // use exceptions
            ptc_use_exceptions();

            // create timer object
            object = ptc_timer_create();
        }

        DynamicTimer(double time)
        {
            // use exceptions
            ptc_use_exceptions();

            // create timer object
            object = ptc_timer_create();

            // set time
            ptc_timer_set(object,time);
        }

        DynamicTimer(const DynamicTimer &timer)
        {
            // use exceptions
            ptc_use_exceptions();

            // create timer object
            object = ptc_timer_create();

            // assign timer
            ptc_timer_assign(object,timer.object);
        }

        ~DynamicTimer()
        {
            // destroy timer object
            ptc_timer_destroy(object);
        }

        void set(double time)
        {
            // set timer
            ptc_timer_set(object,time);
        }
        
        void start()
        {
            // start timer
            ptc_timer_start(object);
        }

        void stop()
        {
            // stop timer
            ptc_timer_stop(object);
        }

        double time()
        {
            // get time
            return ptc_timer_time(object);
        }
            
        double delta()
        {
            // get delta
            return ptc_timer_delta(object);
        }

        double resolution()
        {
            // get resolution
            return ptc_timer_resolution(object);
        }

        DynamicTimer& operator =(const DynamicTimer &timer)
        {
            // assignment operator
            ptc_timer_assign(object,timer.object);
            return *this;
        }

        bool operator ==(const DynamicTimer &timer) const
        {
            // is equal to
            return (ptc_timer_equals(object,timer.object)==1);
        }

        bool operator !=(const DynamicTimer &timer) const
        {
            // not equal to
            return (ptc_timer_equals(object,timer.object)==0);
        }

    private:

        // timer object
        PTC_TIMER object;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
