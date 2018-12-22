#include "stdafx.h"
#include "PageValueList.h"

CPageValueList::CPageValueList()
{
}

CPageValueList::~CPageValueList()
{
}

bool CPageValueList::CreatePage( CWnd* in_pParent, int io_ID, const CRect& in_Rect )
{
    m_pWndPage = new CListCtrl;

    CListCtrl* pListCtrl = DYNAMIC_DOWNCAST( CListCtrl, m_pWndPage );

    // create a listctrl using the report style but no header
    pListCtrl->Create( WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_NOSORTHEADER, in_Rect, in_pParent, io_ID );

	pListCtrl->InsertColumn( 0, "Name", LVCFMT_LEFT, 80 );
    pListCtrl->InsertColumn( 1, "Value", LVCFMT_LEFT, (in_Rect.Width() - 80 > 100) ? in_Rect.Width() - 80 : 100 );

    return NULL != m_pWndPage;
}

void CPageValueList::ProcessEntry( const void* const in_pData )
{
    ASSERT( m_pWndPage->IsKindOf( RUNTIME_CLASS( CListCtrl ) ) );
    CListCtrl* pListCtrl = DYNAMIC_DOWNCAST( CListCtrl, m_pWndPage );

    const RemoteLoggingGlobal::CValuePageData* pValueData = static_cast<const RemoteLoggingGlobal::CValuePageData*>( in_pData );

    MapStringToInt::const_iterator it = m_mapVariableToIndex.find( pValueData->m_pcName );

    // if not found, insert it
    if( it == m_mapVariableToIndex.end() )
    {
        int iIndex = pListCtrl->GetItemCount();
        int iInsertedIndex = pListCtrl->InsertItem( iIndex, pValueData->m_pcName );
        ASSERT( iIndex == iInsertedIndex );

        pListCtrl->SetItemText( iIndex, 1, pValueData->m_pcValue );

        m_mapVariableToIndex.insert( MapStringToInt::value_type( pValueData->m_pcName, iIndex ) );
    }
    else // the variable already exist so update it
    {
        pListCtrl->SetItemText( it->second, 1, pValueData->m_pcValue );
    }
}
