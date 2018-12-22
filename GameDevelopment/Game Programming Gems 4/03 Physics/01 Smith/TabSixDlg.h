#if !defined(AFX_TABSIXDLG_H__DCA12173_4890_4C3B_A718_9A8F6E8ADDCE__INCLUDED_)
#define AFX_TABSIXDLG_H__DCA12173_4890_4C3B_A718_9A8F6E8ADDCE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabSixDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabSixDlg dialog

class CTabSixDlg : public CDialog
{
// Construction
public:
	CTabSixDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTabSixDlg)
	enum { IDD = IDD_TAB_SIX_DLG };
	double	m_dProbabilityOfMOrFKill;
	CString	m_strKilltype;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabSixDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabSixDlg)
	afx_msg void OnCalculateButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABSIXDLG_H__DCA12173_4890_4C3B_A718_9A8F6E8ADDCE__INCLUDED_)
