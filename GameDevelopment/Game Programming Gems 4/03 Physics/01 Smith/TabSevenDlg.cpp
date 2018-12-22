// TabSevenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Driver of Death.h"
#include "TabSevenDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabSevenDlg dialog


CTabSevenDlg::CTabSevenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTabSevenDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabSevenDlg)
	m_dWeightOfChemical = 0.0;
	m_dDistanceOfRounds = 0.0;
	m_nNumberOfRounds = 0;
	m_dProbabilityOfHit = 0.0;
	//}}AFX_DATA_INIT
}


void CTabSevenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabSevenDlg)
	DDX_Text(pDX, IDC_EDIT_TAB_SEVEN_CHEM_WEIGHT, m_dWeightOfChemical);
	DDX_Text(pDX, IDC_EDIT_TAB_SEVEN_DIST_RNDS, m_dDistanceOfRounds);
	DDX_Text(pDX, IDC_EDIT_TAB_SEVEN_NUM_RNDS, m_nNumberOfRounds);
	DDX_Text(pDX, IDC_EDIT_TAB_SEVEN_POH, m_dProbabilityOfHit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabSevenDlg, CDialog)
	//{{AFX_MSG_MAP(CTabSevenDlg)
	ON_BN_CLICKED(IDC_CALCULATE_BUTTON, OnCalculateButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabSevenDlg message handlers

void CTabSevenDlg::OnCalculateButton() 
{
	UpdateData(TRUE);
	
	if(m_dDistanceOfRounds && m_dWeightOfChemical && m_nNumberOfRounds != 0)
	{
		m_dProbabilityOfHit = AreaEffectWeapons(m_nNumberOfRounds,m_dWeightOfChemical,m_dDistanceOfRounds);
		UpdateData(FALSE);
	}
}
