#include "stdafx.h"
#include "RemoteLoggerConsole.h"
#include "RemoteAddressDlg.h"

// CRemoteAddressDlg dialog

IMPLEMENT_DYNAMIC(CRemoteAddressDlg, CDialog)
CRemoteAddressDlg::CRemoteAddressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRemoteAddressDlg::IDD, pParent)
    , m_Edit_Address(_T(""))
    , m_Edit_Port(_T(""))
{
}

CRemoteAddressDlg::~CRemoteAddressDlg()
{
}

void CRemoteAddressDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_ADDRESS, m_Edit_Address);
    DDX_Text(pDX, IDC_EDIT_PORT, m_Edit_Port);
}


BEGIN_MESSAGE_MAP(CRemoteAddressDlg, CDialog)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CRemoteAddressDlg message handlers

BOOL CRemoteAddressDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_Edit_Address = "127.0.0.1";
    m_Edit_Port = "5002";

    UpdateData( FALSE );

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRemoteAddressDlg::OnBnClickedOk()
{
    UpdateData();

    if( 0 >= m_Edit_Address.GetLength() )
    {
        MessageBox( "You must specify an address to connect to", "Missing information ...", MB_ICONEXCLAMATION | MB_OK );
        return;
    }

    if( 0 >= m_Edit_Port.GetLength() )
    {
        MessageBox( "You must specify a port to connect to", "Missing information ...", MB_ICONEXCLAMATION | MB_OK );
        return;
    }

    OnOK();
}
