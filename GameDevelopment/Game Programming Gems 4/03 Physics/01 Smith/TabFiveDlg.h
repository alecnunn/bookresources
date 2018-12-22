#if !defined(AFX_TABFIVEDLG_H__1C7225CC_15B6_462F_A7FA_50BF15C5C974__INCLUDED_)
#define AFX_TABFIVEDLG_H__1C7225CC_15B6_462F_A7FA_50BF15C5C974__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabFiveDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabFiveDlg dialog

class CTabFiveDlg : public CDialog
{
// Construction
public:
	CTabFiveDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTabFiveDlg)
	enum { IDD = IDD_TAB_FIVE_DLG };
	double	m_dProbabilityOfHit;
	int		m_nNumberOfRounds;
	double	m_dStandardDeviation;
	float	m_fLethalArea;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabFiveDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabFiveDlg)
	afx_msg void OnCalculateButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABFIVEDLG_H__1C7225CC_15B6_462F_A7FA_50BF15C5C974__INCLUDED_)
