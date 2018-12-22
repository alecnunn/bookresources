#ifndef _CREMOTELOGGER_H_
#define _CREMOTELOGGER_H_

#pragma once

#include "..\Common\RemoteLoggingGlobalDefines.h"

struct CRemoteLoggerImplementation;

class CRemoteLogger
{
public:
    CRemoteLogger(void);
    ~CRemoteLogger(void);

    void StartLog();
    void StopLog();
    void PauseLog( const bool in_bPause = true);
    void AddPage( const RemoteLoggingGlobal::EPageType in_ePageType, const char* const in_pcPageName );
    void AddLogValue( const char* const in_pcPageName, const char* const in_pcFormat, ... ) const;
    void AddVariableValue( const char* const in_pcPageName, const char* const in_pcVariableName, const char* const in_pcFormat, ... ) const;

    // Enable us to accept connection while in a single thread model.
    void Tick();

    inline bool IsLogEnabled() const { return m_bLogEnabled; }

private:
    // We use the pImpl programming idiom to confine Socket related methog and variable in our implementation.
    CRemoteLoggerImplementation *m_pImpl;

    bool                        m_bLogEnabled;
};

#endif // #ifndef _CREMOTELOGGER_H_