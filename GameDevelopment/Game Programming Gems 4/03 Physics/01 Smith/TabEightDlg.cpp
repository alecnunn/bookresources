// TabEightDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Driver of Death.h"
#include "TabEightDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabEightDlg dialog


CTabEightDlg::CTabEightDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTabEightDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabEightDlg)
	m_dAngleToFarEdge = 0.0;
	m_dAngleToNearEdge = 0.0;
	m_nNumberOfProjectiles = 0;
	m_dProbabilityOfHit = 0.0;
	m_dRangeToTargetFromDet = 0.0;
	m_dVulnerableArea = 0.0;
	//}}AFX_DATA_INIT
}


void CTabEightDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabEightDlg)
	DDX_Text(pDX, IDC_EDIT_TAB_EIGHT_ANGLE_FAR, m_dAngleToFarEdge);
	DDX_Text(pDX, IDC_EDIT_TAB_EIGHT_ANGLE_NEAR, m_dAngleToNearEdge);
	DDX_Text(pDX, IDC_EDIT_TAB_EIGHT_NUM_PROJ, m_nNumberOfProjectiles);
	DDX_Text(pDX, IDC_EDIT_TAB_EIGHT_POH, m_dProbabilityOfHit);
	DDX_Text(pDX, IDC_EDIT_TAB_EIGHT_RANGE_DET, m_dRangeToTargetFromDet);
	DDX_Text(pDX, IDC_EDIT_TAB_EIGHT_VUL_AREA, m_dVulnerableArea);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabEightDlg, CDialog)
	//{{AFX_MSG_MAP(CTabEightDlg)
	ON_BN_CLICKED(IDC_CALCULATE_BUTTON, OnCalculateButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabEightDlg message handlers

void CTabEightDlg::OnCalculateButton() 
{
	UpdateData(TRUE);
	
	if(m_dAngleToFarEdge && m_dAngleToNearEdge && m_dRangeToTargetFromDet && m_dVulnerableArea && m_nNumberOfProjectiles != 0)
	{
		m_dProbabilityOfHit = ShrapnelWedge(m_nNumberOfProjectiles,m_dVulnerableArea,m_dRangeToTargetFromDet,m_dAngleToNearEdge,m_dAngleToFarEdge);
		UpdateData(FALSE);
	}
}
