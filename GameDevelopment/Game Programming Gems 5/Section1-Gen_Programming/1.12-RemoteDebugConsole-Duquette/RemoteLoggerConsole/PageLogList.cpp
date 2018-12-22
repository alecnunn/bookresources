#include "stdafx.h"
#include "PageLogList.h"

CPageLogList::CPageLogList()
{
}

CPageLogList::~CPageLogList()
{
}

bool CPageLogList::CreatePage( CWnd* in_pParent, int io_ID, const CRect& in_Rect )
{
    m_pWndPage = new CListCtrl;

    CListCtrl* pListCtrl = DYNAMIC_DOWNCAST( CListCtrl, m_pWndPage );

    // create a listctrl using the report style but no header
    pListCtrl->Create( WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_NOSORTHEADER | LVS_NOCOLUMNHEADER, in_Rect, in_pParent, io_ID );

	pListCtrl->InsertColumn( 0, "", LVCFMT_LEFT, in_Rect.Width() );

    return NULL != m_pWndPage;
}

void CPageLogList::ProcessEntry( const void* const in_pData )
{
    ASSERT( m_pWndPage->IsKindOf( RUNTIME_CLASS( CListCtrl ) ) );
    CListCtrl* pListCtrl = DYNAMIC_DOWNCAST( CListCtrl, m_pWndPage );

    const RemoteLoggingGlobal::CLogListPageData* pListData = static_cast<const RemoteLoggingGlobal::CLogListPageData*>( in_pData );

    pListCtrl->InsertItem( pListCtrl->GetItemCount(), pListData->m_pcString );
}
