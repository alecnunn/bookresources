// TabFourDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Driver of Death.h"
#include "TabFourDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabFourDlg dialog


CTabFourDlg::CTabFourDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTabFourDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabFourDlg)
	m_dProbabilityOfHit = 0.0;
	m_fLethalArea = 0.0;
	m_dStandardDeviation = 0.0;
	m_dNumberOfRounds = 0.0;
	//}}AFX_DATA_INIT
}


void CTabFourDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabFourDlg)
	DDX_Text(pDX, IDC_EDIT_TAB_ONE_POH, m_dProbabilityOfHit);
	DDX_Text(pDX, IDC_EDIT_TAB_ONE_LETHAL_AREA, m_fLethalArea);
	DDX_Text(pDX, IDC_EDIT_TAB_ONE_STD_DEV, m_dStandardDeviation);
	DDX_Text(pDX, IDC_EDIT_TAB_ONE_NUM_OF_RNDS, m_dNumberOfRounds);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabFourDlg, CDialog)
	//{{AFX_MSG_MAP(CTabFourDlg)
	ON_BN_CLICKED(IDC_CALCULATE_BUTTON, OnCalculateButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabFourDlg message handlers

void CTabFourDlg::OnCalculateButton() 
{
	UpdateData(TRUE);
	
	if(m_fLethalArea && m_dNumberOfRounds && m_dStandardDeviation != 0)
	{
		m_dProbabilityOfHit = ShotgunningASmallTarget(m_dNumberOfRounds,m_fLethalArea,m_dStandardDeviation);
		UpdateData(FALSE);
	}
}
