// ReflectionGemDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ReflectionGem.h"
#include "ReflectionGemDlg.h"
#include "BaseObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CReflectionGemDlg, CDialog)
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CReflectionGemDlg::CReflectionGemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReflectionGemDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CReflectionGemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLACEHOLDER, m_PlaceHolder);
}


BOOL CReflectionGemDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_UI.SetGrid( &m_PropertyGrid );

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon	

	// Get the size of the place holder, this will be used when creating the grid.
	CRect rc;
	m_PlaceHolder.GetWindowRect( &rc );
	ScreenToClient( &rc );

	// Create the property grid.
	m_PropertyGrid.Create( rc, this, IDC_PROPERTY_GRID );
	m_PropertyGrid.ShowToolBar( true );
	m_UI.BuildUIForObject( &m_TestObject, "Monster Stats" );

	SetWindowPos( NULL, 1, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE );
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

LRESULT CReflectionGemDlg::OnGridNotify( WPARAM wParam, LPARAM lParam )
{
	return m_UI.OnGridNotify( wParam, lParam );
}