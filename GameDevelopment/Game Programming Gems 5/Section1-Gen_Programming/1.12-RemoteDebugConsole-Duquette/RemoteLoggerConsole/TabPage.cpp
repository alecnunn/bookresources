#include "StdAfx.h"
#include "TabPage.h"

CTabPage::CTabPage(void)
{
}

CTabPage::~CTabPage(void)
{
    delete m_pWndPage;
}

void CTabPage::ShowWindow( const bool in_bShow )
{
    m_pWndPage->ShowWindow( in_bShow ? SW_SHOW : SW_HIDE );
}
