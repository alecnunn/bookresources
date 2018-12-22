//
// Log class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include <stdio.h>
#include "Core/Log.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif

#ifdef __PTC_LOGGING__




// static log data
ofstream Log::m_output;
bool Log::m_create = true;

#ifdef __DEBUG__

    // enable logging by default
    bool Log::m_logging = true;

#else

    // disable logging by default
    bool Log::m_logging = false;

#endif




void Log::enable()
{
    // enable logging
    m_logging = true;
}


void Log::disable()
{
    // disable logging
    m_logging = false;
}



void Log::log(const char message[])
{
    // check logging
    if (m_logging)
    {
        // open
        open();

        // log the message
        m_output << message << "\n\n";

        // close
        close();
    }
}


void Log::log(const char message[],bool data)
{
    // check logging
    if (m_logging)
    {
        // open
        open();

        // check data
        if (data==true)
        {
            // log the message and true to output
            m_output << message << " = true\n\n";
        }
        else
        {
            // log the message and false to output
            m_output << message << " = false\n\n";
        }

        // close
        close();
    }
}


void Log::log(const char message[],int data)
{
    // check logging
    if (m_logging)
    {
        // open
        open();

        // log the message and integer data to output
        m_output << message << " = " << data << "\n\n";

        // close
        close();
    }
}


void Log::log(const char message[],double data)
{
    // check logging
    if (m_logging)
    {
        // open
        open();

        // log the message and double data to output
        m_output << message << " = " << data << "\n\n";

        // close
        close();
    }
}


void Log::log(const char message[],const char data[])
{
    // check logging
    if (m_logging)
    {
        // open
        open();

        // log the message and string to output
        m_output << message << " = " << '"' << data << '"' << "\n\n";

        // close
        close();
    }
}


void Log::log(const char message[],const Error &data)
{
    // check logging
    if (m_logging)
    {
        // open
        open();
                
        // log the message and error to output
        m_output << message << ": " << data.message() << "\n\n";

        // close
        close();
    }
}


void Log::log(const char message[],const Format &data)
{
    // check logging
    if (m_logging)
    {
        // open
        open();

        // output string
        char output[1024];

        // check format type
        if (data.direct())
        {
            // check alpha
            if (!data.a())
            {
                // direct color format without alpha
                sprintf(output,"Format(%d,0x%.8X,0x%.8X,0x%.8X)",data.bits(),data.r(),data.g(),data.b());
            }
            else
            {
                // direct color format with alpha
                sprintf(output,"Format(%d,0x%.8X,0x%.8X,0x%.8X,0x%.8X)",data.bits(),data.r(),data.g(),data.b(),data.a());
            }
        }
        else
        {
            // indexed color format
            sprintf(output,"Format(%d)",data.bits());
        }

        // log the message and format to output
        m_output << message << " = " << output << "\n\n";

        // close
        close();
    }
}




void Log::open()
{
    // check create
    if (m_create)
    {
        // open new log file for writing
        m_output.open("ptc.log",ios::out);
        
        // output win32 version string
        m_output << "[" << __PTC_WIN32_VERSION__ << "]" << "\n\n";

        // clear create flag
        m_create = false;
    }
    else
    {
        // open existing log file for appending
        m_output.open("ptc.log",ios::out|ios::app);
    }
}


void Log::close()
{
    // close stream
    m_output.close();
}




#endif
