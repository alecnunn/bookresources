// TabSixDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Driver of Death.h"
#include "TabSixDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabSixDlg dialog


CTabSixDlg::CTabSixDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTabSixDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabSixDlg)
	m_dProbabilityOfMOrFKill = 0.0;
	m_strKilltype = _T("");
	//}}AFX_DATA_INIT
}


void CTabSixDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabSixDlg)
	DDX_Text(pDX, IDC_EDIT_TAB_SIX_PROBABILITY, m_dProbabilityOfMOrFKill);
	DDV_MinMaxDouble(pDX, m_dProbabilityOfMOrFKill, 0., 1.);
	DDX_Text(pDX, IDC_EDIT_TAB_SIX_KILL_TYPE, m_strKilltype);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabSixDlg, CDialog)
	//{{AFX_MSG_MAP(CTabSixDlg)
	ON_BN_CLICKED(IDC_CALCULATE_BUTTON, OnCalculateButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabSixDlg message handlers

void CTabSixDlg::OnCalculateButton() 
{
	UpdateData(TRUE);
	
	Killtype eKillType = nokill;

	if(m_dProbabilityOfMOrFKill != 0)
	{
		eKillType = KillsComeInFourFlavors(m_dProbabilityOfMOrFKill);

		if(eKillType == nokill)
		{
			m_strKilltype = "No Kill";
		}
		else if(eKillType == mobility)
		{
			m_strKilltype = "Mobility Kill";
		}
		else if(eKillType == firepower)
		{
			m_strKilltype = "Firepower Kill";
		}
		else if(eKillType == mobilityandfirepower)
		{
			m_strKilltype = "Mobility and Firepower Kill";
		}
		else if(eKillType == catastrophic)
		{
			m_strKilltype = "Catastrophic Kill";
		}
		UpdateData(FALSE);
	}
}
