#if !defined(AFX_TABTENDLG_H__555CF8C4_B7D5_4C78_81D4_2DD46817E742__INCLUDED_)
#define AFX_TABTENDLG_H__555CF8C4_B7D5_4C78_81D4_2DD46817E742__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabTenDLG.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabTenDLG dialog

class CTabTenDLG : public CDialog
{
// Construction
public:
	CTabTenDLG(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTabTenDLG)
	enum { IDD = IDD_TAB_TEN_DLG };
	double	m_dAverageDestruction;
	double	m_dChangeInPreyPopulation;
	int		m_nNumHunters;
	int		m_nNumPrey;
	double	m_dPreySpacing;
	int		m_nTotalPrey;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabTenDLG)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabTenDLG)
	afx_msg void OnCalculateButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABTENDLG_H__555CF8C4_B7D5_4C78_81D4_2DD46817E742__INCLUDED_)
