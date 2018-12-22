// TabOneDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Driver of Death.h"
#include "TabOneDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabOneDlg dialog


CTabOneDlg::CTabOneDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTabOneDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabOneDlg)
	m_dStandardDeviation = 0.0;
	m_dWidthOfTarget = 0.0;
	m_dProbabilityOfHit = 0.0;
	//}}AFX_DATA_INIT
}


void CTabOneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabOneDlg)
	DDX_Text(pDX, IDC_EDIT_TAB_ONE_STD_DEV, m_dStandardDeviation);
	DDX_Text(pDX, IDC_EDIT_TAB_ONE_WIDTH, m_dWidthOfTarget);
	DDX_Text(pDX, IDC_EDIT_TAB_ONE_POH, m_dProbabilityOfHit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabOneDlg, CDialog)
	//{{AFX_MSG_MAP(CTabOneDlg)
	ON_BN_CLICKED(IDC_CALCULATE_BUTTON, OnCalculateButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabOneDlg message handlers

void CTabOneDlg::OnCalculateButton() 
{
	UpdateData(TRUE);

	if(m_dWidthOfTarget && m_dStandardDeviation != 0)
	{
		m_dProbabilityOfHit = HittingARibbon(m_dWidthOfTarget, m_dStandardDeviation);
		UpdateData(FALSE);
	}
}
