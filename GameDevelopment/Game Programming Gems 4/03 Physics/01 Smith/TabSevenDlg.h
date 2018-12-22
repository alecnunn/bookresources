#if !defined(AFX_TABSEVENDLG_H__4CD94EE0_6733_4AD1_B892_CC5D63D26354__INCLUDED_)
#define AFX_TABSEVENDLG_H__4CD94EE0_6733_4AD1_B892_CC5D63D26354__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabSevenDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabSevenDlg dialog

class CTabSevenDlg : public CDialog
{
// Construction
public:
	CTabSevenDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTabSevenDlg)
	enum { IDD = IDD_TAB_SEVEN_DLG };
	double	m_dWeightOfChemical;
	double	m_dDistanceOfRounds;
	int		m_nNumberOfRounds;
	double	m_dProbabilityOfHit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabSevenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabSevenDlg)
	afx_msg void OnCalculateButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABSEVENDLG_H__4CD94EE0_6733_4AD1_B892_CC5D63D26354__INCLUDED_)
