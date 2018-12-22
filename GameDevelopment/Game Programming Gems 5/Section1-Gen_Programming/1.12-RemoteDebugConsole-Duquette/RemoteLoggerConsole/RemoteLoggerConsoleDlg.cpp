// RemoteLoggerConsoleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RemoteLoggerConsole.h"
#include "RemoteLoggerConsoleDlg.h"
#include "TabPage.h"
#include "RemoteAddressDlg.h"
#include "PageLogList.h"
#include "PageValueList.h"

// guard against 2 warning present in wspiapi.h
#pragma warning( push )
#pragma warning( disable : 4127 ) // conditional expression is constant
#pragma warning( disable : 4706 ) // assignment within conditional expression
#include <Ws2tcpip.h>
#pragma warning( pop )

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
    const UINT WSA_EVENT = ( WM_USER + 1 );
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CRemoteLoggerConsoleDlg dialog



CRemoteLoggerConsoleDlg::CRemoteLoggerConsoleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRemoteLoggerConsoleDlg::IDD, pParent),
    m_Socket( INVALID_SOCKET ),
    m_bConnected( false )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRemoteLoggerConsoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TABCTRL, m_TabCtrl);
}

BEGIN_MESSAGE_MAP(CRemoteLoggerConsoleDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_INITMENU()
    ON_MESSAGE( WSA_EVENT, OnWSAEvent )
//}}AFX_MSG_MAP
    ON_COMMAND(ID_REMOTELOG_CONNECT, OnRemotelogConnect)
    ON_COMMAND(ID_REMOTELOG_DISCONNECT, OnRemotelogDisconnect)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TABCTRL, OnSelchangeTab)
    ON_NOTIFY(TCN_SELCHANGING, IDC_TABCTRL, OnSelchangingTab)
END_MESSAGE_MAP()


// CRemoteLoggerConsoleDlg message handlers

BOOL CRemoteLoggerConsoleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRemoteLoggerConsoleDlg::OnInitMenu(CMenu* pMenu)
{
    if( GetMenu() == pMenu )
    {
        pMenu->EnableMenuItem( ID_REMOTELOG_CONNECT, MF_BYCOMMAND | (m_bConnected ? MF_DISABLED | MF_GRAYED : MF_ENABLED) );
        pMenu->EnableMenuItem( ID_REMOTELOG_DISCONNECT, MF_BYCOMMAND | (m_bConnected ? MF_ENABLED : MF_DISABLED | MF_GRAYED) );
    }
}

void CRemoteLoggerConsoleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRemoteLoggerConsoleDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRemoteLoggerConsoleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRemoteLoggerConsoleDlg::OnRemotelogConnect()
{
    if( INVALID_SOCKET != m_Socket )
    {
        OnRemotelogConnect();
    }

    // Popup connection dialog
    CRemoteAddressDlg dlg;
    if( IDOK != dlg.DoModal() )
    {
        return;
    }

    // try to connect
    m_Socket = socket( AF_INET, SOCK_STREAM, 0 );
    if( m_Socket == INVALID_SOCKET )
    {
        MessageBox( "Could not create socket.", "Error", MB_ICONERROR | MB_OK );
        return;
    }

    addrinfo hint;
    ::ZeroMemory( &hint, sizeof( hint ) );
    hint.ai_family = AF_INET;
    hint.ai_protocol = SOCK_STREAM;

    addrinfo *pAddressInfo = NULL;

    if( 0 != getaddrinfo( (LPCTSTR) dlg.m_Edit_Address, (LPCTSTR) dlg.m_Edit_Port, &hint, &pAddressInfo ) )
    {
        MessageBox( "Could not retreive the remote station translated address", "Error", MB_ICONERROR | MB_OK );
        closesocket( m_Socket );
        m_Socket = INVALID_SOCKET;
        return;
    }

    if( 0 != connect( m_Socket, pAddressInfo->ai_addr, (int) pAddressInfo->ai_addrlen ) )
    {
        MessageBox( "Could not connect to the remote address", "Error", MB_ICONERROR | MB_OK );
        freeaddrinfo( pAddressInfo );
        closesocket( m_Socket );
        m_Socket = INVALID_SOCKET;
        return;
    }

    // set the callback msg for socket events
    if( 0 != WSAAsyncSelect( m_Socket, m_hWnd, WSA_EVENT, FD_READ | FD_CLOSE ) ) 
    {

        MessageBox( "Could not set the event call back for the socket notifications" );
        goto cleanup;
    }

cleanup:
    if( NULL != pAddressInfo )
    {
        freeaddrinfo( pAddressInfo );
    }
}

void CRemoteLoggerConsoleDlg::OnRemotelogDisconnect()
{
    if( INVALID_SOCKET != m_Socket )
    {
        closesocket( m_Socket );
        m_Socket = INVALID_SOCKET;
    }
}

LRESULT CRemoteLoggerConsoleDlg::OnWSAEvent( WPARAM in_wParam, LPARAM in_lParam )
{
    int iStatus;
    WORD event;
    SOCKET sock = in_wParam;

    if( WSAGETSELECTERROR( in_lParam ) )
    {
        MessageBox( "Error while processing Network event ...", "Error", MB_ICONERROR | MB_OK );
        return TRUE;
    }

    event = WSAGETSELECTEVENT( in_lParam );
    if( event == FD_READ )
    {
        do
        {
            // The size of our buffer is the size for a complete packet
            static const UINT uiBuffSize = sizeof( RemoteLoggingGlobal::CUnParsedData );
            static char pcBuf[uiBuffSize] = { '\0' };
            static UINT uiBuffNeeded = uiBuffSize;
            static char* pcBuffIndex = pcBuf;

            iStatus = recv( sock, pcBuffIndex, uiBuffNeeded, 0 );
            if( SOCKET_ERROR == iStatus )
            {
                MessageBox( "Could not retreive the packet", "Error", MB_ICONERROR | MB_OK );
            }
            else
            {
                // if we only have a partial packet ...
                if( iStatus != (int) uiBuffNeeded )
                {
                    uiBuffNeeded = uiBuffSize - iStatus;
                    pcBuffIndex = pcBuf + iStatus;

                    // since we don't have a complete packet, continue
                    continue;
                }
                else
                {
                    uiBuffNeeded = uiBuffSize;
                    pcBuffIndex = pcBuf;
                }

                RemoteLoggingGlobal::CUnParsedData UnParsedData;
                memcpy( &UnParsedData, pcBuf, iStatus > sizeof( UnParsedData ) ? sizeof( UnParsedData ) : iStatus );

                switch( UnParsedData.m_uiCommand )
                {
                case RemoteLoggingGlobal::eDCC_ADD_PAGE:
                    AddTabPage( (RemoteLoggingGlobal::CNewPageData*) UnParsedData.m_pData );
                    break;
                case RemoteLoggingGlobal::eDCC_DATA:
                    ProcessData( (RemoteLoggingGlobal::CBasePageData*) UnParsedData.m_pData );
                    break;
                default:
                    ASSERT( 0 );
                    break;
                }
            }
        }while( WSAGetLastError() == WSAEMSGSIZE );
    }
    else if( event == FD_CLOSE )
    {
        m_Socket = INVALID_SOCKET;
        m_bConnected = false;
        MessageBox( "Connecttion reset by peer", "Warning", MB_ICONWARNING | MB_OK );
    }

    return FALSE;
}

void CRemoteLoggerConsoleDlg::OnCancel()
{
    NetworkCleanUp();

    CDialog::OnCancel();
}


void CRemoteLoggerConsoleDlg::NetworkCleanUp()
{
    if ( m_Socket != INVALID_SOCKET ) 
    {
        closesocket( m_Socket );
    }
}
void CRemoteLoggerConsoleDlg::OnSelchangeTab( NMHDR* io_pNMHDR, LRESULT* io_pResult ) 
{
    UNUSED_PARAM( io_pNMHDR );
    UNUSED_PARAM( io_pResult );

    int iIndex = m_TabCtrl.GetCurSel();
    ASSERT( iIndex <= (int) m_vTabPage.size() );
    m_vTabPage[iIndex]->ShowWindow();
}

void CRemoteLoggerConsoleDlg::OnSelchangingTab( NMHDR* io_pNMHDR, LRESULT* io_pResult )
{
    UNUSED_PARAM( io_pNMHDR );
    UNUSED_PARAM( io_pResult );

    int iIndex = m_TabCtrl.GetCurSel();
    ASSERT( iIndex <= (int) m_vTabPage.size() );
    m_vTabPage[iIndex]->ShowWindow( false );
}

void CRemoteLoggerConsoleDlg::AddTabPage( const RemoteLoggingGlobal::CNewPageData* const in_pNewPageData )
{
    // only add a page if it's unique
    MapStringToInt::iterator it = m_mapPageNameIndex.find( in_pNewPageData->m_pcPageName );
    if( it != m_mapPageNameIndex.end() )
    {
        return;
    }

    // tab ctrl limitation (MSDN documentation)
    if( m_TabCtrl.GetItemCount() >= 27 )
    {
        ASSERT( 0 && "CRemoteLoggerConsoleDlg::AddTabPage : tried to add a page while 27 page were already created" );
        return;
    }

    TCITEM tabItem;
    tabItem.mask = TCIF_TEXT;

    tabItem.pszText = (char*) in_pNewPageData->m_pcPageName;
    // Insert at the end of the list ...
    m_TabCtrl.InsertItem( m_TabCtrl.GetItemCount(), &tabItem );

    CRect ItemRect;
    m_TabCtrl.GetItemRect( 0, &ItemRect );

    CRect TabCtrlRect;
    m_TabCtrl.GetClientRect( &TabCtrlRect );

    CRect ClientAreaRect;
    ClientAreaRect.top = TabCtrlRect.top + ItemRect.Height() + 12;
    ClientAreaRect.left = TabCtrlRect.left + 7;
    ClientAreaRect.right = TabCtrlRect.right - 10;
    ClientAreaRect.bottom = TabCtrlRect.bottom - 10;

    CTabPage* pPage = NULL;

    switch( in_pNewPageData->m_eType )
    {
    case RemoteLoggingGlobal::ePT_LOGLIST:
        pPage = new CPageLogList;
        break;
    case RemoteLoggingGlobal::ePT_VALUELIST:
        pPage = new CPageValueList;
        break;
    default:
        ASSERT( 0 && "CRemoteLoggerConsoleDlg::AddTabPage : Unsupported page type requested ..." );
        break;
    }

    pPage->CreatePage( &m_TabCtrl, 1024, ClientAreaRect );
    m_vTabPage.push_back( pPage );

    ASSERT( (int) m_vTabPage.size() > 0 );
    std::pair< MapStringToInt::iterator, bool > ReturnValue;
    ReturnValue = m_mapPageNameIndex.insert( MapStringToInt::value_type( in_pNewPageData->m_pcPageName, ((int)m_vTabPage.size()) - 1 ) );

    if( !ReturnValue.second )
    {
        m_vTabPage.pop_back();
        delete pPage;
        return;
    }

    // if first page we insert, focus it :)
    if( m_vTabPage.size() == 1 )
    {
        m_TabCtrl.SetCurSel( 0 );
        pPage->ShowWindow();
    }
    else
    {
        pPage->ShowWindow( false );
    }
}

void CRemoteLoggerConsoleDlg::ProcessData( const RemoteLoggingGlobal::CBasePageData* const in_pData )
{
    MapStringToInt::iterator it = m_mapPageNameIndex.find( in_pData->m_pcPageName );

    if( it != m_mapPageNameIndex.end() )
    {
        m_vTabPage[it->second ]->ProcessEntry( in_pData );
    }
    else
    {
        ASSERT( 0 && "CRemoteLoggerConsoleDlg::ProcessData : Tried to access a page that doesn't exist ..." );
    }
}
