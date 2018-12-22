// TabThreeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Driver of Death.h"
#include "TabThreeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabThreeDlg dialog


CTabThreeDlg::CTabThreeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTabThreeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabThreeDlg)
	m_dProbabilityOfHit = 0.0;
	m_dStandardDeviationX = 0.0;
	m_dStandardDeviationY = 0.0;
	m_dLengthX = 0.0;
	m_dWidthY = 0.0;
	//}}AFX_DATA_INIT
}


void CTabThreeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabThreeDlg)
	DDX_Text(pDX, IDC_EDIT_TAB_ONE_POH, m_dProbabilityOfHit);
	DDX_Text(pDX, IDC_EDIT_TAB_ONE_STD_DEV, m_dStandardDeviationX);
	DDX_Text(pDX, IDC_EDIT_TAB_ONE_STD_DEV2, m_dStandardDeviationY);
	DDX_Text(pDX, IDC_EDIT_TAB_ONE_WIDTH, m_dLengthX);
	DDX_Text(pDX, IDC_EDIT_TAB_ONE_WIDTH2, m_dWidthY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabThreeDlg, CDialog)
	//{{AFX_MSG_MAP(CTabThreeDlg)
	ON_BN_CLICKED(IDC_CALCULATE_BUTTON, OnCalculateButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabThreeDlg message handlers

void CTabThreeDlg::OnCalculateButton() 
{
	UpdateData(TRUE);

	if(m_dLengthX && m_dWidthY && m_dStandardDeviationX && m_dStandardDeviationY != 0)
	{
		m_dProbabilityOfHit = HittingARectangle(m_dLengthX,m_dWidthY,m_dStandardDeviationX,m_dStandardDeviationY);
		UpdateData(FALSE);
	}
}
