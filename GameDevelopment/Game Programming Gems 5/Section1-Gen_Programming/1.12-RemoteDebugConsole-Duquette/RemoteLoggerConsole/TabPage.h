#pragma once

class CTabPage
{
public:
    CTabPage(void);
    ~CTabPage(void);

    virtual bool CreatePage( CWnd* in_pParent, int io_ID, const CRect& in_Rect ) PURE;
    void ShowWindow( const bool in_bShow = true );
    virtual void ProcessEntry( const void* const in_pcData ) PURE;

protected:
    CWnd* m_pWndPage;
};
