#if !defined(AFX_TABTHREEDLG_H__A869057C_30EF_46E7_B7ED_001F545C95EF__INCLUDED_)
#define AFX_TABTHREEDLG_H__A869057C_30EF_46E7_B7ED_001F545C95EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabThreeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabThreeDlg dialog

class CTabThreeDlg : public CDialog
{
// Construction
public:
	CTabThreeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTabThreeDlg)
	enum { IDD = IDD_TAB_THREE_DLG };
	double	m_dProbabilityOfHit;
	double	m_dStandardDeviationX;
	double	m_dStandardDeviationY;
	double	m_dLengthX;
	double	m_dWidthY;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabThreeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabThreeDlg)
	afx_msg void OnCalculateButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABTHREEDLG_H__A869057C_30EF_46E7_B7ED_001F545C95EF__INCLUDED_)
