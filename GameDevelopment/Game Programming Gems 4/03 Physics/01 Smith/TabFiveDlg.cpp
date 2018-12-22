// TabFiveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Driver of Death.h"
#include "TabFiveDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabFiveDlg dialog


CTabFiveDlg::CTabFiveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTabFiveDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabFiveDlg)
	m_dProbabilityOfHit = 0.0;
	m_nNumberOfRounds = 0;
	m_dStandardDeviation = 0.0;
	m_fLethalArea = 0.0f;
	//}}AFX_DATA_INIT
}


void CTabFiveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabFiveDlg)
	DDX_Text(pDX, IDC_EDIT_TAB_ONE_POH, m_dProbabilityOfHit);
	DDX_Text(pDX, IDC_EDIT_TAB_FIVE_NUM_RNDS, m_nNumberOfRounds);
	DDX_Text(pDX, IDC_EDIT_TAB_FIVE_STD_DEV, m_dStandardDeviation);
	DDX_Text(pDX, IDC_EDIT_TAB_FIVE_LETHAL_AREA, m_fLethalArea);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabFiveDlg, CDialog)
	//{{AFX_MSG_MAP(CTabFiveDlg)
	ON_BN_CLICKED(IDC_CALCULATE_BUTTON, OnCalculateButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabFiveDlg message handlers

void CTabFiveDlg::OnCalculateButton() 
{
	UpdateData(TRUE);
	
	if(m_dStandardDeviation && m_fLethalArea && m_nNumberOfRounds != 0)
	{
		m_dProbabilityOfHit = DeathByWalkingArtillery(m_nNumberOfRounds,m_fLethalArea,m_dStandardDeviation);
		UpdateData(FALSE);
	}
}
