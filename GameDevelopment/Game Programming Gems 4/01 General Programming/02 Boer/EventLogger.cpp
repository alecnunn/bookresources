//---------------------------------------------------------- -*- Mode: C++ -*-
// eventlogger.cpp
//----------------------------------------------------------------------------

#include "eventlogger.h"
#include <string>
#include <windows.h>

#ifdef LOGGING


using namespace logger;

const int MAX_DEBUG_LINE_LEN = 1024;


logger::EventLogger g_Log;

// Helper function designed to push and pop
EventLogFN::EventLogFN(const char* function)
{
    g_Log.pushFunction(function);
}

EventLogFN::~EventLogFN()
{
    g_Log.popFunction();
}



EventLogger::EventLogger()
{
#ifdef HTML_OUTPUT
    m_file = 0;
#endif
    m_updateCount = 0;
    m_loggedEvent = false;
    m_initialized = false;
    m_previousStackLevel = 0;
    m_callStack.reserve(32);

}

// Initialize and shut down the event logging system
bool EventLogger::init(const char* logName)
{
    if(isInitialized())
        return false;

#ifdef HTML_OUTPUT
    m_file = fopen(logName, "w");
    if(!m_file)
    {
        char drive[_MAX_DRIVE];
        char dir[_MAX_DIR];
        char fname[_MAX_FNAME];
        char ext[_MAX_EXT];
        _splitpath( logName, drive, dir, fname, ext );
        std::string filePath = fname;
        filePath += ext;
        m_file = fopen(filePath.c_str(), "w");
        if(!m_file)
            return false;
    }
#endif // HTML_OUTPUT

    m_initialized = true;
    m_loggedEvent = true;
    m_previousStackLevel = 0;

    return true;
}


void EventLogger::term()
{
#ifdef HTML_OUTPUT
    if(m_file)
        fclose(m_file);
    m_file = 0;
#endif
    m_initialized = false;
}


void EventLogger::logEvent(unsigned int flags, const char* format, ...) 
{ 
    if(!isInitialized())
        return;
    char buffer[MAX_DEBUG_LINE_LEN];
    va_list args; 
    va_start(args, format); 
#if defined(WINVER) || defined(_XBOX)
    int buf = _vsnprintf(buffer, MAX_DEBUG_LINE_LEN, format, args); 
#else
    int buf = vsnprintf(buffer, MAX_DEBUG_LINE_LEN, format, args); 
#endif
    assert(buf >= 0); 
    va_end(args);

    // Log output to debug windows and/or disk depending on options
    logOutput(buffer, flags);
}


void EventLogger::logEvent(const char* format, ...)
{
    if(!isInitialized())
        return;
    char buffer[MAX_DEBUG_LINE_LEN];
    va_list args; 
    va_start(args, format); 
#if defined(WINVER) || defined(_XBOX)
    int buf = _vsnprintf(buffer, MAX_DEBUG_LINE_LEN, format, args); 
#else
    int buf = vsnprintf(buffer, MAX_DEBUG_LINE_LEN, format, args); 
#endif
    assert((buf >= 0) && (buf < MAX_DEBUG_LINE_LEN)); 
    va_end(args);

    // Log output to debug windows and/or disk depending on options
    logOutput(buffer, 0);
}



// Call once per game tick to update the event logger
void EventLogger::update()
{
    if(!isInitialized())
        return;

    m_updateCount++;

    // Finish writing any call stack output
    logCallStack();
 
#ifdef HTML_OUTPUT
    htmlUpdate();
#endif // HTML_OUTPUT

    debugUpdate();

    // Reset the logged event flags
    m_loggedEvent = false;

    flush();

}


void EventLogger::pushFunction(const char* name)
{
    if(!isInitialized())
        return;
    m_callStack.push_back(name); 
}

void EventLogger::popFunction()
{
    if(!isInitialized() || m_callStack.empty())
        return;
    m_callStack.pop_back();
    if(m_previousStackLevel >= m_callStack.size())
        logCallStack();
}


void EventLogger::flush()
{
    if(!isInitialized())
        return;

#ifdef HTML_OUTPUT
    fflush(m_file);
#endif // HTML_OUTPUT

}


////////////////////////////////////
// Common logging functions

void EventLogger::logOutput(char* buffer, unsigned int flags)
{
    // Strip any unnecessary newline characters at the end of the buffer
    int i = strlen(buffer);
    if(i == 0)
        return;
    if(buffer[i - 1] == '\n')
        buffer[i - 1] = 0;

    // Indicate that we have some info to print, so write the stack info
    m_loggedEvent = true;

    // Update the call stack output
    logCallStack();

#ifdef HTML_OUTPUT
    htmlOutput(buffer, flags);
#endif // HTML_OUTPUT
    debugOutput(buffer, flags);
}


void EventLogger::logCallStack()
{
    unsigned int currentStackLevel = m_callStack.size();

    while(m_previousStackLevel < currentStackLevel)
    {
#ifdef HTML_OUTPUT
        htmlFunctionStart(m_callStack[m_previousStackLevel]);
#endif // HTML_OUTPUT
        m_previousStackLevel++;
    }

    while(m_previousStackLevel > currentStackLevel)
    {
#ifdef HTML_OUTPUT
        htmlFunctionEnd();
#endif // HTML_OUTPUT
        m_previousStackLevel--;
    }
    
}



////////////////////////////////////
// Debug output functions


void EventLogger::debugUpdate()
{
}


void EventLogger::debugOutput(const char* buffer, unsigned int flags)
{
    char buf[MAX_DEBUG_LINE_LEN];
    sprintf(buf, "%s\n", buffer);
	OutputDebugString(buf);
}


////////////////////////////////////
// HTML output functions

#ifdef HTML_OUTPUT


void EventLogger::htmlUpdate()
{
    if(m_loggedEvent)
    {
        // Calculate game time based on ticks - assumed at 60 per second
		// If your game engine works with a variable-speed simulation rate,
		// you'll have to rework this section.
        int hundrethSeconds = m_updateCount * 100 / 60 % 100;
        int seconds = m_updateCount / 60 % 60;
        int minutes = m_updateCount / 3600 % 60;
        int hours = m_updateCount / 216000;

        // Buffer a new tick update
        writeStartParagraph();
        writeStartFont("Arial Black", 3, 0x00, 0x00, 0x00);
        writeText(0, "Update = %d, Game Time = %.2d:%.2d:%.2d.%.2d", m_updateCount, hours, minutes, seconds, hundrethSeconds);
        writeEndFont();
        writeEndParagraph();
        writeEndLine();
    }
}


void EventLogger::htmlOutput(const char* buffer, unsigned int flags)
{
    // Calculate text color
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    if(flags & LOG_COLOR_DK_RED)
        r = unsigned char(127);
    else if(flags & LOG_COLOR_RED)
        r = unsigned char(255);
    if(flags & LOG_COLOR_DK_GREEN)
        g = unsigned char(127);
    else if(flags & LOG_COLOR_GREEN)
        g = unsigned char(255);
    if(flags & LOG_COLOR_DK_BLUE)
        b = unsigned char(127);
    else if(flags & LOG_COLOR_BLUE)
        b = unsigned char(255);

    int size = 2;
    if(flags & LOG_SIZE_SMALL)
        size--;
    if(flags & LOG_SIZE_LARGE)
        size++;

    // Write formatted HTML to the output buffer
    writeIndent();
    writeStartListItem();
    writeStartFont("Arial", size, r, g, b);
    writeText(flags, buffer);
    writeEndFont();
    writeEndListItem();
    writeEndLine();
}


void EventLogger::htmlFunctionStart(const char* fn)
{
    writeIndent();
    writeStartUnsortedList();
    writeStartListItem();
    writeStartFont("Courier", 2, 0x99, 0x99, 0x99);
    writeText(0, fn);
    writeEndFont();
    writeEndListItem();
    writeEndLine();
}

void EventLogger::htmlFunctionEnd()
{
    writeIndent();
    writeEndUnsortedList();
    writeEndLine();
}





void EventLogger::writeStartParagraph()
{
    fwrite("<p>", 1, 3, m_file);
}


void EventLogger::writeEndParagraph()
{
    fwrite("</p>", 1, 4, m_file);
}


void EventLogger::writeStartUnsortedList()
{
    fwrite("<ul>", 1, 4, m_file);
}


void EventLogger::writeEndUnsortedList()
{
    fwrite("</ul>", 1, 5, m_file);
}


void EventLogger::writeStartListItem()
{
    fwrite("<li>", 1, 4, m_file);
}


void EventLogger::writeEndListItem()
{
    fwrite("</li>", 1, 5, m_file);
}

void EventLogger::writeStartFont(const char* szFontName, int iSize, unsigned char red, unsigned char green, unsigned char blue)
{
    char buffer[MAX_DEBUG_LINE_LEN];
    int len = sprintf(buffer, "<font face=\"%s\" size=\"%d\" color=\"#%.2x%.2x%.2x\">", szFontName, iSize, red, green, blue);
    assert(len < MAX_DEBUG_LINE_LEN-1);
    fwrite(buffer, 1, strlen(buffer), m_file);

}


void EventLogger::writeEndFont()
{
    fwrite("</font>", 1, 7, m_file);
}


void EventLogger::writeText(unsigned int flags, const char* lpszFormat, ...)
{
    char buffer[MAX_DEBUG_LINE_LEN];
    va_list args; 
    va_start(args, lpszFormat); 
    int nBuf = _vsnprintf(buffer, MAX_DEBUG_LINE_LEN, lpszFormat, args); 
    assert((nBuf >= 0) && (nBuf < MAX_DEBUG_LINE_LEN)); 
    va_end(args);

    // Start text modifiers
    if(flags & LOG_BOLD)
        fwrite("<b>", 1, 3, m_file);
    if(flags & LOG_ITALICS)
        fwrite("<i>", 1, 3, m_file);
    if(flags & LOG_UNDERLINE)
        fwrite("<u>", 1, 3, m_file);

    // Write text buffer
    fwrite(buffer, 1, strlen(buffer), m_file);

    // Finish text modifiers
    if(flags & LOG_BOLD)
        fwrite("</b>", 1, 4, m_file);
    if(flags & LOG_ITALICS)
        fwrite("</i>", 1, 4, m_file);
    if(flags & LOG_UNDERLINE)
        fwrite("</u>", 1, 4, m_file);
}


void EventLogger::writeEndLine()
{
    fwrite("\n", 1, 1, m_file);
}

void EventLogger::writeIndent()
{
    for(unsigned int i = 0; i < m_previousStackLevel; i++)
        fwrite("  ", 1, 2, m_file);
}


#endif // HTML_OUTPUT

#endif // LOGGING
