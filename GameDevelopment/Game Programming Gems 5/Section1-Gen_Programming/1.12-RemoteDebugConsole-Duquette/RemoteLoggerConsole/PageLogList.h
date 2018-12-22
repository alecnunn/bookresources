#pragma once

#include "TabPage.h" // IsA

class CPageLogList : public CTabPage
{
public:
    CPageLogList();
    virtual ~CPageLogList();

    virtual bool CreatePage( CWnd* in_pParent, int io_ID, const CRect& in_Rect );
    virtual void ProcessEntry( const void* const in_pData );
};

