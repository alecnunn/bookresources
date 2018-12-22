// TabTenDLG.cpp : implementation file
//

#include "stdafx.h"
#include "Driver of Death.h"
#include "TabTenDLG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabTenDLG dialog


CTabTenDLG::CTabTenDLG(CWnd* pParent /*=NULL*/)
	: CDialog(CTabTenDLG::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabTenDLG)
	m_dAverageDestruction = 0.0;
	m_dChangeInPreyPopulation = 0.0;
	m_nNumHunters = 0;
	m_nNumPrey = 0;
	m_dPreySpacing = 0.0;
	m_nTotalPrey = 0;
	//}}AFX_DATA_INIT
}


void CTabTenDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabTenDLG)
	DDX_Text(pDX, IDC_EDIT_TAB_TEN_AVG_DESTRUCTION, m_dAverageDestruction);
	DDX_Text(pDX, IDC_EDIT_TAB_TEN_CHANGE_PREY_POP, m_dChangeInPreyPopulation);
	DDX_Text(pDX, IDC_EDIT_TAB_TEN_NUM_HUNTERS, m_nNumHunters);
	DDX_Text(pDX, IDC_EDIT_TAB_TEN_NUM_PREY, m_nNumPrey);
	DDX_Text(pDX, IDC_EDIT_TAB_TEN_PREY_SPACING, m_dPreySpacing);
	DDX_Text(pDX, IDC_EDIT_TAB_TEN_TOTAL_PREY, m_nTotalPrey);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabTenDLG, CDialog)
	//{{AFX_MSG_MAP(CTabTenDLG)
	ON_BN_CLICKED(IDC_CALCULATE_BUTTON, OnCalculateButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabTenDLG message handlers

void CTabTenDLG::OnCalculateButton() 
{
	UpdateData(TRUE);
	
	if(m_dAverageDestruction && m_dPreySpacing && m_nNumHunters && m_nNumPrey && m_nTotalPrey != 0)
	{
		std::vector<double> dVector;
		std::vector<int> nVector;
		dVector.push_back(m_dAverageDestruction);
		nVector.push_back(m_nNumHunters);

		m_dChangeInPreyPopulation = BeatingTheBushesWithPreySpacing(m_nNumPrey,m_dPreySpacing,m_nTotalPrey,dVector,nVector);
		UpdateData(FALSE);
	}
}
