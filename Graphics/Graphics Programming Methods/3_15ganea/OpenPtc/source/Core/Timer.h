//
// Timer class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_TIMER_H
#define __PTC_TIMER_H

// include files
#include "config.h"
#include "Core/Types.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




class Timer
{
    public:

        // setup
        PTCAPI Timer();
        PTCAPI Timer(double time);
        PTCAPI Timer(const Timer &timer);
        PTCAPI ~Timer();

        // set time
        void PTCAPI set(double time);

        // control
        void PTCAPI start();
        void PTCAPI stop();

        // time data
        double PTCAPI time();
        double PTCAPI delta();
        double PTCAPI resolution();
    
        // operators
        Timer& PTCAPI operator =(const Timer &timer);
        bool PTCAPI operator ==(const Timer &timer) const;
        bool PTCAPI operator !=(const Timer &timer) const;

    private:

        // clock time
        double clock();

        // data
        double m_old;
        double m_time;
        double m_start;
        double m_current;
        bool m_running;
        __int64 m_frequency;
};




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
