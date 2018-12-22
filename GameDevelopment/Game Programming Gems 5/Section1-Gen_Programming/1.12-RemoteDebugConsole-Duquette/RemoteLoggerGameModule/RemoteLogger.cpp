#include "StdAfx.h"
#include "RemoteLogger.h"

#include <WinSock2.h>
#include <assert.h>
#include <string>
#include <map>

namespace
{
    const unsigned short    g_usPort( 5002 );
    // Currently, the maximum number of page supported by the RemoteLogger is 27 due to the limitation of the TabCtrl used 
    // (refer to MSDN documentation for more detail)
    const size_t            g_uiMaxPage( 27 );
}

struct CRemoteLoggerImplementation
{
    CRemoteLoggerImplementation() : m_ListeningSocket( INVALID_SOCKET ), m_OutputSocket( INVALID_SOCKET ), m_bNetworkLayerInitialized( false ) {}

    // this function is implementation specific
    void RemoteLoggerErrorMsg( const char* const in_pcContext, const int in_iWSAErroCode );
    bool Write( const RemoteLoggingGlobal::CUnParsedData* const in_pData );
    void SendRegisteredPage();

    // Keep the NewPageData. It will be sent to the RemoteLogger every time we call StartLog
    typedef std::map<std::string, RemoteLoggingGlobal::EPageType> PageNameTypeMap;
    PageNameTypeMap m_mapRegisteredPage;

    SOCKET  m_ListeningSocket;
    SOCKET  m_OutputSocket;
    bool    m_bNetworkLayerInitialized;
};

// this function is implementation specific and only a basic example is given here
void CRemoteLoggerImplementation::RemoteLoggerErrorMsg( const char* const in_pcContext, const int in_iWSAErroCode )
{
    LPVOID lpMsgBuf;
    if( !FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                        NULL,
                        in_iWSAErroCode,
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                        (LPTSTR) &lpMsgBuf,
                        0,
                        NULL ) )
    {
        // Handle the error.
        return;
    }

    // Display the string.
    MessageBox( NULL, (LPCTSTR) lpMsgBuf, in_pcContext, MB_OK | MB_ICONINFORMATION );

    // Free the buffer.
    LocalFree( lpMsgBuf );
}

bool CRemoteLoggerImplementation::Write( const RemoteLoggingGlobal::CUnParsedData* const in_pData )
{
    assert( INVALID_SOCKET != m_OutputSocket && "CRemoteLoggerImplementation::Write : socket not connected ..." );

    if( sizeof( RemoteLoggingGlobal::CUnParsedData ) != send( m_OutputSocket, (char*) in_pData, sizeof( RemoteLoggingGlobal::CUnParsedData ), 0 ) )
    {
        RemoteLoggerErrorMsg( "CRemoteLoggerImplementation::Write", WSAGetLastError() );
        return false;
    }

    return true;
}

void CRemoteLoggerImplementation::SendRegisteredPage()
{
    if( !m_mapRegisteredPage.empty() )
    {
        PageNameTypeMap::iterator it = m_mapRegisteredPage.begin();
        PageNameTypeMap::iterator itEnd = m_mapRegisteredPage.end();

        RemoteLoggingGlobal::CUnParsedData UnparsedData;

        while( it != itEnd )
        {
            memset( UnparsedData.m_pData, 0, RemoteLoggingGlobal::eDMS_UNPARSEDDATA_DATA );

            UnparsedData.m_uiCommand = RemoteLoggingGlobal::eDCC_ADD_PAGE;
            ((RemoteLoggingGlobal::CNewPageData*) UnparsedData.m_pData)->m_eType = it->second;
            strcpy( ((RemoteLoggingGlobal::CNewPageData*) UnparsedData.m_pData)->m_pcPageName, it->first.c_str() );

            //Send page update to logger
            Write( &UnparsedData );

            ++it;
        }
    }
}

CRemoteLogger::CRemoteLogger(void) :
    m_bLogEnabled( false )
{
    m_pImpl = new CRemoteLoggerImplementation;

    // Windows socket initialization
    WSADATA WSAData;
    if( 0 != WSAStartup( MAKEWORD( 2, 2 ), &WSAData ) ) 
    {
        return;
    }

    m_pImpl->m_bNetworkLayerInitialized = true;
}

CRemoteLogger::~CRemoteLogger(void)
{
    if( m_pImpl->m_bNetworkLayerInitialized )
    {
        WSACleanup();
    }

    if( NULL != m_pImpl )
    {
        delete m_pImpl;
    }
}

void CRemoteLogger::StartLog()
{
    assert( m_pImpl && "CRemoteLogger::StartLog : m_pImpl not created ?" );
    if( !m_pImpl->m_bNetworkLayerInitialized )
    {
        return;
    }

    if( m_bLogEnabled )
    {
        return;
    }

    if( INVALID_SOCKET != m_pImpl->m_ListeningSocket )
    {
        closesocket( m_pImpl->m_ListeningSocket );
    }

    m_pImpl->m_ListeningSocket = socket( AF_INET, SOCK_STREAM, 0 );

    if( INVALID_SOCKET == m_pImpl->m_ListeningSocket )
    {
        // Implementation specifix erro display
        m_pImpl->RemoteLoggerErrorMsg( "CRemoteLogger::StartLog", WSAGetLastError() );
        return;
    }

    SOCKADDR_IN sockBindAddress;
    sockBindAddress.sin_family = AF_INET;
    sockBindAddress.sin_addr.S_un.S_addr = ADDR_ANY;
    sockBindAddress.sin_port = htons( g_usPort );

    if( SOCKET_ERROR == bind( m_pImpl->m_ListeningSocket, (SOCKADDR*) &sockBindAddress, sizeof( sockBindAddress ) ) )
    {
        // Implementation specifix erro display
        m_pImpl->RemoteLoggerErrorMsg( "CRemoteLogger::StartLog", WSAGetLastError() );
        closesocket( m_pImpl->m_ListeningSocket );
        m_pImpl->m_ListeningSocket = INVALID_SOCKET;
        return;
    }

    // we currently only allow one RemoteLogger connection at a time.
    if( SOCKET_ERROR == listen( m_pImpl->m_ListeningSocket, 1 ) )
    {
        closesocket( m_pImpl->m_ListeningSocket );
        m_pImpl->m_ListeningSocket = INVALID_SOCKET;
        return;
    }

    m_bLogEnabled = true;
}

void CRemoteLogger::StopLog()
{
    assert( m_pImpl && "CRemoteLogger::StopLog : m_pImpl not created ?" );
    if( !m_pImpl->m_bNetworkLayerInitialized )
    {
        return;
    }

    if( !m_bLogEnabled )
    {
        return;
    }

    m_bLogEnabled = false;

    // stop accepting connection
    if( INVALID_SOCKET != m_pImpl->m_ListeningSocket )
    {
        closesocket( m_pImpl->m_ListeningSocket );
    }

    // stop sending 
    if( INVALID_SOCKET != m_pImpl->m_OutputSocket )
    {
        closesocket( m_pImpl->m_OutputSocket );
    }
}

void CRemoteLogger::PauseLog( const bool in_bPause /* = true */ )
{
    assert( m_pImpl && "CRemoteLogger::PauseLog : m_pImpl not created ?" );
    if( !m_pImpl->m_bNetworkLayerInitialized )
    {
        return;
    }

    m_bLogEnabled = in_bPause;
}

void CRemoteLogger::AddPage( const RemoteLoggingGlobal::EPageType in_ePageType, const char* const in_pcPageName )
{
    assert( RemoteLoggingGlobal::ePT_MAX > in_ePageType && "CRemoteLogger::AddPage : unrecognized page type ..." );
    assert( RemoteLoggingGlobal::eDMS_PAGE_NAME > strlen( in_pcPageName ) && "CRemoteLogger::AddPage : PageName exceed the maximum allowed length" );

    RemoteLoggingGlobal::CNewPageData *pNewPageData = new RemoteLoggingGlobal::CNewPageData;
    assert( NULL != pNewPageData && "CRemoteLogger::AddPage : Could not create new page struct, out of memory ?" );

    pNewPageData->m_eType = in_ePageType;
    strcpy( pNewPageData->m_pcPageName, in_pcPageName );

    // Keep the page data. 
    std::pair<CRemoteLoggerImplementation::PageNameTypeMap::iterator, bool> pairInsertResult;
    pairInsertResult = m_pImpl->m_mapRegisteredPage.insert( CRemoteLoggerImplementation::PageNameTypeMap::value_type( in_pcPageName, in_ePageType ) );
    assert( pairInsertResult.second && "CRemoteLogger::AddPage : A page with this name is already created ..." );

    if( INVALID_SOCKET != m_pImpl->m_OutputSocket )
    {
        RemoteLoggingGlobal::CUnParsedData UnparsedData;
        UnparsedData.m_uiCommand = RemoteLoggingGlobal::eDCC_ADD_PAGE;
        ((RemoteLoggingGlobal::CNewPageData*) UnparsedData.m_pData)->m_eType = in_ePageType;
        strcpy( ((RemoteLoggingGlobal::CNewPageData*) UnparsedData.m_pData)->m_pcPageName, in_pcPageName );

        //Send page update to logger directly.
        m_pImpl->Write( &UnparsedData );
    }
}

void CRemoteLogger::AddLogValue( const char* const in_pcPageName, const char* const in_pcFormat, ... ) const
{
    assert( m_pImpl && "CRemoteLogger::AddLogValue : m_pImpl not created ?" );
    if( !m_pImpl->m_bNetworkLayerInitialized )
    {
        return;
    }

    // Should never assert as we are using the macro instead of directly calling the functions.
    assert( m_bLogEnabled && "CRemoteLogger::AddLogValue : Log is not enabled ..." );

    assert( RemoteLoggingGlobal::eDMS_PAGE_NAME > strlen( in_pcPageName ) && "CRemoteLogger::AddLogValue : PageName is too long ..." );

    va_list vaList;
    char acBuffer[RemoteLoggingGlobal::eDMS_LOGLISTPAGEDATA_STRING] = { '\0' };

    va_start( vaList, in_pcFormat );
    if( RemoteLoggingGlobal::eDMS_LOGLISTPAGEDATA_STRING < (_vscprintf( in_pcFormat, vaList ) + 1) )
    {
        assert( !"CRemoteLogger::AddLogValue : converted string is too long ..." );
        return;
    }

    vsprintf( acBuffer, in_pcFormat, vaList );
    va_end( vaList );

    // Sanity check to make sure our requested page exist ...
    CRemoteLoggerImplementation::PageNameTypeMap::const_iterator itFind = m_pImpl->m_mapRegisteredPage.find( in_pcPageName );
    if( itFind == m_pImpl->m_mapRegisteredPage.end() )
    {
        //ErrorMsg page doesn't exist ...
        assert( 0 && "CRemoteLogger::AddLogValue page doesn't exist" );
        return;
    }

    // Page is of the correct type ?
    assert( RemoteLoggingGlobal::ePT_LOGLIST == itFind->second && "CRemoteLogger::AddLogValue : Incorrect page type ..." );

    RemoteLoggingGlobal::CUnParsedData UnparsedData;
    UnparsedData.m_uiCommand = RemoteLoggingGlobal::eDCC_DATA;

    RemoteLoggingGlobal::CLogListPageData *pPageData = (RemoteLoggingGlobal::CLogListPageData*) &UnparsedData.m_pData;

    strcpy( pPageData->m_pcPageName, in_pcPageName );
    strcpy( pPageData->m_pcString, acBuffer );

    m_pImpl->Write( &UnparsedData );
}

void CRemoteLogger::AddVariableValue( const char* const in_pcPageName, const char* const in_pcVariableName, 
                       const char* const in_pcFormat, ... ) const
{
    assert( m_pImpl && "CRemoteLogger::AddVariableValue : m_pImpl not created ?" );
    if( !m_pImpl->m_bNetworkLayerInitialized )
    {
        return;
    }

    // Should never assert as we are using the macro instead of directly calling the functions.
    assert( m_bLogEnabled && "CRemoteLogger::AddVariableValue : Log is not enabled ..." );

    assert( RemoteLoggingGlobal::eDMS_PAGE_NAME > strlen( in_pcPageName ) && "CRemoteLogger::AddVariableValue : PageName is too long ..." );
    assert( RemoteLoggingGlobal::eDMS_VALUEPAGEDATA_NAME > strlen( in_pcVariableName ) && "CRemoteLogger::AddVariableValue : VariableName is too long ..." );

    va_list vaList;
    char acBuffer[RemoteLoggingGlobal::eDMS_VALUEPAGEDATA_VALUE] = { '\0' };

    va_start( vaList, in_pcFormat );
    if( RemoteLoggingGlobal::eDMS_VALUEPAGEDATA_VALUE < (_vscprintf( in_pcFormat, vaList ) + 1) )
    {
        assert( !"CRemoteLogger::AddVariableValue : converted string is too long ..." );
        return;
    }

    vsprintf( acBuffer, in_pcFormat, vaList );
    va_end( vaList );

    // Sanity check to make sure our requested page exist ...
    CRemoteLoggerImplementation::PageNameTypeMap::const_iterator itFind = m_pImpl->m_mapRegisteredPage.find( in_pcPageName );
    if( itFind == m_pImpl->m_mapRegisteredPage.end() )
    {
        //ErrorMsg page doesn't exist ...
        assert( 0 && "CRemoteLogger::AddVariableValue page doesn't exist" );
        return;
    }

    // Page is of the correct type ?
    assert( RemoteLoggingGlobal::ePT_VALUELIST == itFind->second && "CRemoteLogger::AddVariableValue : Incorrect page type ..." );

    RemoteLoggingGlobal::CUnParsedData UnparsedData;
    UnparsedData.m_uiCommand = RemoteLoggingGlobal::eDCC_DATA;

    RemoteLoggingGlobal::CValuePageData *pPageData = (RemoteLoggingGlobal::CValuePageData*) &UnparsedData.m_pData;

    strcpy( pPageData->m_pcPageName, in_pcPageName );
    strcpy( pPageData->m_pcName, in_pcVariableName );
    strcpy( pPageData->m_pcValue, acBuffer );

    m_pImpl->Write( &UnparsedData );
}

// This function is called every frame to see if a new connection was made.
void CRemoteLogger::Tick()
{
    assert( m_pImpl && "CRemoteLogger::RemoteLog : m_pImpl not created ?" );
    if( !m_pImpl->m_bNetworkLayerInitialized )
    {
        return;
    }

    if( !m_bLogEnabled )
    {
        return;
    }

    fd_set fdsetAccept;
    FD_ZERO( &fdsetAccept );

#pragma warning( push )
// The implementation of FD_SET use a do { ... }while(0) and thus fire this warning
#pragma warning( disable : 4127 ) // conditional expression is constant
    if( INVALID_SOCKET != m_pImpl->m_ListeningSocket )
        FD_SET( m_pImpl->m_ListeningSocket, &fdsetAccept );
    else
        FD_SET( m_pImpl->m_OutputSocket, &fdsetAccept );
#pragma warning( pop )

    // Specify a timeval of 0 sec 0 microsecond. This will make it a non-blocking call
    timeval timevalTimeOut;
    timevalTimeOut.tv_sec = 0l;
    timevalTimeOut.tv_usec = 0l;

    int iSelectRetVal = select( 0, &fdsetAccept, NULL, NULL, &timevalTimeOut );
    if( SOCKET_ERROR == iSelectRetVal )
    {
        int iLastError = WSAGetLastError();

        StopLog();

        m_pImpl->RemoteLoggerErrorMsg( "CRemoteLogger::Tick", iLastError );
        return;
    }

    // No connection waiting to be accepted so continue
    if( 0 == iSelectRetVal )
    {
        return;
    }

    // if the listening socket is invalid it means that we are not accepting connection and should only check to see 
    // if the outgoing socket was reset/closed
    if( INVALID_SOCKET == m_pImpl->m_ListeningSocket )
    {
        return;
    }

    m_pImpl->m_OutputSocket = accept( m_pImpl->m_ListeningSocket, NULL, NULL );

    if( INVALID_SOCKET == m_pImpl->m_OutputSocket )
    {
        m_pImpl->RemoteLoggerErrorMsg( "CRemoteLogger::Tick", WSAGetLastError() );
        StopLog();
        return;
    }

    // Only one connection at a time
    closesocket( m_pImpl->m_ListeningSocket );
    m_pImpl->m_ListeningSocket = INVALID_SOCKET;

    // send the registered page to the remote logger
    m_pImpl->SendRegisteredPage();
}
