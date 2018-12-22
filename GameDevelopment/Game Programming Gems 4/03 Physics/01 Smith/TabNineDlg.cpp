// TabNineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Driver of Death.h"
#include "TabNineDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabNineDlg dialog


CTabNineDlg::CTabNineDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTabNineDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabNineDlg)
	m_dChangePreyPopulation = 0.0;
	m_nNumOfHunters = 0;
	m_nNumberOfPrey = 0;
	m_dPreyHardness = 0.0;
	m_dProbabilityOfDetection = 0.0;
	m_nTotalPrey = 0;
	//}}AFX_DATA_INIT
}


void CTabNineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabNineDlg)
	DDX_Text(pDX, IDC_EDIT_TAB_NINE_CHANGE_IN_PREY_POP, m_dChangePreyPopulation);
	DDX_Text(pDX, IDC_EDIT_TAB_NINE_NUM_HUNTERS, m_nNumOfHunters);
	DDX_Text(pDX, IDC_EDIT_TAB_NINE_NUM_PREY, m_nNumberOfPrey);
	DDX_Text(pDX, IDC_EDIT_TAB_NINE_PREY_HARDNESS, m_dPreyHardness);
	DDX_Text(pDX, IDC_EDIT_TAB_NINE_PROB_DETECTION, m_dProbabilityOfDetection);
	DDX_Text(pDX, IDC_EDIT_TAB_NINE_TOTAL_PREY, m_nTotalPrey);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabNineDlg, CDialog)
	//{{AFX_MSG_MAP(CTabNineDlg)
	ON_BN_CLICKED(IDC_CALCULATE_BUTTON, OnCalculateButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabNineDlg message handlers

void CTabNineDlg::OnCalculateButton() 
{
	UpdateData(TRUE);
	
	if(m_dPreyHardness && m_dProbabilityOfDetection && m_nNumberOfPrey && m_nNumOfHunters && m_nTotalPrey != 0)
	{
		std::vector<double> dVector;
		std::vector<int> nVector;
		dVector.push_back(m_dProbabilityOfDetection);
		nVector.push_back(m_nNumOfHunters);

		m_dChangePreyPopulation = BeatingTheBushes(m_nNumberOfPrey,m_dPreyHardness,m_nTotalPrey,dVector,nVector);
		UpdateData(FALSE);
	}
}
