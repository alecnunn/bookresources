// Notice: No microsoft typedefs were used for the sake of portability ( like UINT, BYTE, etc ... )

// Leave the #ifndef in this file since it is also used by the server part which 
// could be used by a compiler not supporting #pragma once
#ifndef _REMOTELOGGINGGLOBAL_H_
#define _REMOTELOGGINGGLOBAL_H_

#include <memory>

namespace RemoteLoggingGlobal
{
    enum EDataMaxSize
    {
        eDMS_PAGE_NAME = 64,
        eDMS_UNPARSEDDATA_DATA = 396,
        eDMS_LOGLISTPAGEDATA_STRING = 255,
        eDMS_VALUEPAGEDATA_NAME = 64,
        eDMS_VALUEPAGEDATA_VALUE = 255
    };

    enum EDebugConsoleCommand
    {
        eDCC_ADD_PAGE,
        eDCC_DATA,
        eDCC_MAX
    };

    enum EPageType
    {
        ePT_LOGLIST,
        ePT_VALUELIST,
        ePT_MAX
    };

    struct CUnParsedData
    {
        CUnParsedData() : m_uiCommand( eDCC_MAX ) 
        {
            memset( m_pData, 0, eDMS_UNPARSEDDATA_DATA );
        }

        unsigned int m_uiCommand;
        unsigned char m_pData[eDMS_UNPARSEDDATA_DATA];
    };

    struct CNewPageData
    {
        CNewPageData() : m_eType( ePT_MAX )
        {
            memset( m_pcPageName, 0, eDMS_PAGE_NAME );
        }

        char m_pcPageName[eDMS_PAGE_NAME];
        EPageType m_eType;
    };

    struct CBasePageData
    {
        CBasePageData()
        {
            memset( m_pcPageName, 0, eDMS_PAGE_NAME );
        }

        char m_pcPageName[eDMS_PAGE_NAME];
    };

    struct CLogListPageData
    {
        CLogListPageData()
        {
            memset( m_pcPageName, 0, eDMS_PAGE_NAME );
            memset( m_pcString, 0, eDMS_LOGLISTPAGEDATA_STRING );
        }

        char m_pcPageName[eDMS_PAGE_NAME];
        char m_pcString[eDMS_LOGLISTPAGEDATA_STRING];
    };

    struct CValuePageData
    {
        CValuePageData()
        {
            memset( m_pcPageName, 0, eDMS_PAGE_NAME);
            memset( m_pcName, 0, eDMS_VALUEPAGEDATA_NAME);
            memset( m_pcValue, 0, eDMS_VALUEPAGEDATA_VALUE);
        }

        char m_pcPageName[eDMS_PAGE_NAME];
        char m_pcName[eDMS_VALUEPAGEDATA_NAME];
        char m_pcValue[eDMS_VALUEPAGEDATA_VALUE];
    };
}

#endif // #ifndef _REMOTELOGGINGGLOBAL_H_