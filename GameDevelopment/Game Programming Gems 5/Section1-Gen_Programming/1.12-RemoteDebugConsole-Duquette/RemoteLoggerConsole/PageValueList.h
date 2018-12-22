#pragma once

#include "TabPage.h"    // Is a
#include <map>          //As a
#include <string>       // As a

class CPageValueList : public CTabPage
{
public:
    CPageValueList();
    virtual ~CPageValueList();

    virtual bool CreatePage( CWnd* in_pParent, int io_ID, const CRect& in_Rect );
    virtual void ProcessEntry( const void* const in_pData );

protected:
    typedef std::map< std::string, int> MapStringToInt;
    MapStringToInt m_mapVariableToIndex;
};

