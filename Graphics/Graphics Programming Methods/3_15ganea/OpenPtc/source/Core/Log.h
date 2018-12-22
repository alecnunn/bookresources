//
// Log class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

#ifndef __PTC_LOG_H
#define __PTC_LOG_H

// include files
#include <fstream.h>
#include "config.h"
#include "Core/Error.h"
#include "Core/Format.h"

// begin namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
namespace ptc {
#endif




#ifdef __PTC_LOGGING__

class Log
{
    public:

        // logging control
        static void enable();
        static void disable();

        // logging functions
        static void log(const char message[]);
        static void log(const char message[],bool data);
        static void log(const char message[],int data);
        static void log(const char message[],double data);
        static void log(const char message[],const char data[]);
        static void log(const char message[],const Error &data);
        static void log(const char message[],const Format &format);

    private:

        // stream management
        static void open();
        static void close();

        // static log data
        static bool m_create;
        static bool m_logging;
        static ofstream m_output;
};

// logging helper functions
inline void LOG(const char message[]) { Log::log(message); }
inline void LOG(const char message[],bool data) { Log::log(message,data); }
inline void LOG(const char message[],int data) { Log::log(message,data); }
inline void LOG(const char message[],double data) { Log::log(message,data); }
inline void LOG(const char message[],const char data[]) { Log::log(message,data); }
inline void LOG(const char message[],const Error &data) { Log::log(message,data); }
inline void LOG(const char message[],const Format &data) { Log::log(message,data); }

#else

// disabled
#define LOG //

#endif




// end namespace declaration
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
}
#endif

#endif
