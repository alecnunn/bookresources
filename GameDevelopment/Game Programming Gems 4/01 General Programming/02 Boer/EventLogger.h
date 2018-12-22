//---------------------------------------------------------- -*- Mode: C++ -*-
// eventlogger.h
//----------------------------------------------------------------------------

#ifndef EVENTLOGGER_H
#define EVENTLOGGER_H
#ifdef _MSC_VER
#pragma once
#endif

// This turns on or off all logging functionality
#define LOGGING
// Log to disk functionality
#define HTML_OUTPUT

// If logging is not enabled, the system goes away completely
#ifdef LOGGING 

#include <vector>
#include <map>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>


namespace logger
{
    // Actual interface to event logging class
    #define FN(var_1)                   EventLogFN obj____unique_name(var_1)
    #define LOG                         g_Log.logEvent
    #define LOG_INIT                    g_Log.init
    #define LOG_UPDATE                  g_Log.update
    #define LOG_TERM                    g_Log.term
    #define LOG_FLUSH                   g_Log.flush
    #define LOG_REGISTER(var_1, var_2)  g_Log.registerDebug(var_1, var_2)

    // Helper function designed to push and pop
    class EventLogFN
    {
    public:
        EventLogFN(const char* function);
        ~EventLogFN();
    private:
        const char* m_class;
        const char* m_function;
    };

    // General text modifier flags
    #define LOG_COLOR_RED           0x00000001
    #define LOG_COLOR_DK_RED        0x00000002
    #define LOG_COLOR_GREEN         0x00000004
    #define LOG_COLOR_DK_GREEN      0x00000008
    #define LOG_COLOR_BLUE          0x00000010
    #define LOG_COLOR_DK_BLUE       0x00000020
    #define LOG_SIZE_LARGE          0x00000040
    #define LOG_SIZE_SMALL          0x00000080
    #define LOG_BOLD                0x00000100
    #define LOG_ITALICS             0x00000200
    #define LOG_UNDERLINE           0x00000400

    // "stack" typedef
    typedef std::vector<const char*> CharPtrVec;

    //----------------------------------------------------------------------------
    // EventLogger
    //----------------------------------------------------------------------------
    class EventLogger
    {
    public:
        EventLogger();

        // Initialize and shut down the event logging system
        bool init(const char* logName);
        void term();

        // Is the event logging system initialized?
        bool isInitialized() { return m_initialized; }

        // Log an event (with or without formatting flags)
        void logEvent(unsigned int flags, const char* format, ...);
        void logEvent(const char* format, ...);

        // Call once per game tick to update the event logger
        void update();

        // Use this function to force a write of the event logger.  Handy
        // if you've run into an error which will cause a crash.
        void flush();

        // Push a function onto the call stack
        void pushFunction(const char* name);
        // Pop a function off the call stack
        void popFunction();

    private:

        void logOutput(char* buffer, unsigned int flags);
        void logCallStack();

        // Debug output function
        void debugUpdate();
        void debugOutput(const char* buffer, unsigned int flags);

#ifdef HTML_OUTPUT

        // HTML-based disk logging
        void htmlUpdate();
        void htmlOutput(const char* buffer, unsigned int flags);
        void htmlFunctionStart(const char* fn);
        void htmlFunctionEnd();

        // HTML output functions writing to disk
        void writeStartParagraph();
        void writeEndParagraph();
        void writeStartUnsortedList();
        void writeEndUnsortedList();
        void writeStartListItem();
        void writeEndListItem();
        void writeStartFont(const char* fontName, int size, unsigned char red, unsigned char green, unsigned char blue);
        void writeEndFont();
        void writeText(unsigned int flags, const char* format, ...);
        void writeEndLine();
        void writeIndent();
        
#endif // HTML_OUTPUT


#ifdef HTML_OUTPUT
        FILE*           m_file;
#endif // HTML_OUTPUT

        
        CharPtrVec      m_callStack;
        bool            m_loggedEvent;
        unsigned int    m_previousStackLevel;
        unsigned int    m_updateCount;
        bool            m_initialized;
    };

}; // namespace game

extern logger::EventLogger g_Log;

#else


#define LOG         0 &&
#define FN(x)
#define LOG_INIT(x)
#define LOG_UPDATE()
#define LOG_TERM()
#define LOG_FLUSH()
#define LOG_REGISTER(a, b)

// have to have these because the compiler does see them
#define LOG_COLOR_RED           0
#define LOG_COLOR_DK_RED        0
#define LOG_COLOR_GREEN         0
#define LOG_COLOR_DK_GREEN      0
#define LOG_COLOR_BLUE          0
#define LOG_COLOR_DK_BLUE       0
#define LOG_SIZE_LARGE          0
#define LOG_SIZE_SMALL          0
#define LOG_BOLD                0
#define LOG_ITALICS             0
#define LOG_UNDERLINE           0

#endif // LOGGING

#endif // EVENTLOGGER_H
