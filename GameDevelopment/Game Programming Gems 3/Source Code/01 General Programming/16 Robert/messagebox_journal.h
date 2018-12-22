#ifndef MESSAGEBOX_JOURNAL__HEADER
#define MESSAGEBOX_JOURNAL__HEADER

#include "journaling.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

///////////////////////////////////////////////////////////////////////////////
//
// MessageBoxStreamBuf
//
///////////////////////////////////////////////////////////////////////////////

class MessageBoxStreamBuf : public std::streambuf
{
    char myBuffer[4096];

public:

    MessageBoxStreamBuf() {
        setup();
    }

    char * getBuffer() {
    	size_t len = pptr() - pbase();
        if(len >= 0) {
            myBuffer[len] = 0;
        }

        return myBuffer;
    }

protected:

    virtual int sync() {
        setup();
        return 0;
    }

    virtual int_type overflow(int_type c = traits_type::eof()) {
        if(c != traits_type::eof()) {
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
// MessageBoxJournal
//
///////////////////////////////////////////////////////////////////////////////

class MessageBoxJournal : public Journal
{
    MessageBoxStreamBuf myStreamBuf;

public:

    MessageBoxJournal() {
	    init(&myStreamBuf);
    }

    virtual char endReport() {
        *this << "\n\nChoose one of the following:\n\n";

        *this << "\t\"Abort\" to terminate the program\n";
        *this << "\t\"Retry\" to break into the debugger\n";
        *this << "\t\"Ignore\" to continue";

        int result = MessageBox(0, myStreamBuf.getBuffer(), "Assertion Report", MB_ABORTRETRYIGNORE |
                                                                                MB_DEFBUTTON2 |
                                                                                MB_ICONSTOP);

        *this << std::flush;

        if(result == IDABORT) {
            return ABORT;
        }

        if(result == IDRETRY) {
            return BREAK;
        }

        return CONTINUE;
    }
};

#endif
