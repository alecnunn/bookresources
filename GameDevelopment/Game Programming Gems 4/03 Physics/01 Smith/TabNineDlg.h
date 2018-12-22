#if !defined(AFX_TABNINEDLG_H__350758A4_80DD_4E4D_9C08_EBF83A52ABBD__INCLUDED_)
#define AFX_TABNINEDLG_H__350758A4_80DD_4E4D_9C08_EBF83A52ABBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabNineDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabNineDlg dialog

class CTabNineDlg : public CDialog
{
// Construction
public:
	CTabNineDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTabNineDlg)
	enum { IDD = IDD_TAB_NINE_DLG };
	double	m_dChangePreyPopulation;
	int		m_nNumOfHunters;
	int		m_nNumberOfPrey;
	double	m_dPreyHardness;
	double	m_dProbabilityOfDetection;
	int		m_nTotalPrey;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabNineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabNineDlg)
	afx_msg void OnCalculateButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABNINEDLG_H__350758A4_80DD_4E4D_9C08_EBF83A52ABBD__INCLUDED_)
