// TabTwoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Driver of Death.h"
#include "TabTwoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabTwoDlg dialog


CTabTwoDlg::CTabTwoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTabTwoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabTwoDlg)
	m_dProbabilityOfHit = 0.0;
	m_dStandardDeviation = 0.0;
	m_dRadius = 0.0;
	//}}AFX_DATA_INIT
}


void CTabTwoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabTwoDlg)
	DDX_Text(pDX, IDC_EDIT_TAB_ONE_POH, m_dProbabilityOfHit);
	DDX_Text(pDX, IDC_EDIT_TAB_ONE_STD_DEV, m_dStandardDeviation);
	DDX_Text(pDX, IDC_EDIT_TAB_ONE_WIDTH, m_dRadius);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabTwoDlg, CDialog)
	//{{AFX_MSG_MAP(CTabTwoDlg)
	ON_BN_CLICKED(IDC_CALCULATE_BUTTON, OnCalculateButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabTwoDlg message handlers

void CTabTwoDlg::OnCalculateButton() 
{
	UpdateData(TRUE);
	if(m_dRadius && m_dStandardDeviation != 0)
	{
		m_dProbabilityOfHit = HittingTheBullseye(m_dRadius,m_dStandardDeviation);
		UpdateData(FALSE);
	}
}
