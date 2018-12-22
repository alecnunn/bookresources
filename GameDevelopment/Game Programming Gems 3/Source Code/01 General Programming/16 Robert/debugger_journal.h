#ifndef DEBUGGER__HEADER
#define DEBUGGER__HEADER

#include "journaling.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

///////////////////////////////////////////////////////////////////////////////
//
// DebuggerStreamBuf
//
///////////////////////////////////////////////////////////////////////////////

class DebuggerStreamBuf : public std::streambuf
{
    char myBuffer[512];

public:

    DebuggerStreamBuf() {
        setup();
    }

protected:

    virtual int sync() {
    	size_t len = pptr() - pbase();
        if(len > 0) {
            myBuffer[len] = 0;

            OutputDebugString(myBuffer);
            Sleep(0);

            setup();
        }

        return 0;
    }

    virtual int_type overflow(int_type c = traits_type::eof()) {
        sync();

        if(c != traits_type::eof()) {
            myBuffer[0] = c;
            pbump(1);

            return c;
        }

        return traits_type::not_eof(c);
    }

private:

    void setup() {
        setp(myBuffer, myBuffer + sizeof(myBuffer) - 1);
    }
};

///////////////////////////////////////////////////////////////////////////////
//
// DebuggerJournal
//
///////////////////////////////////////////////////////////////////////////////

class DebuggerJournal : public Journal
{
    DebuggerStreamBuf myStreamBuf;

public:

    DebuggerJournal() {
	    init(&myStreamBuf);
    }

    virtual char endReport() {
        *this << std::endl << std::flush;
        return 0;
    }
};

#endif
