#if !defined(AFX_TABONEDLG_H__7636335E_0E2C_48E5_BDEF_40B06E33586E__INCLUDED_)
#define AFX_TABONEDLG_H__7636335E_0E2C_48E5_BDEF_40B06E33586E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabOneDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabOneDlg dialog

class CTabOneDlg : public CDialog
{
// Construction
public:
	CTabOneDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTabOneDlg)
	enum { IDD = IDD_TAB_ONE_DLG };
	double	m_dStandardDeviation;
	double	m_dWidthOfTarget;
	double	m_dProbabilityOfHit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabOneDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabOneDlg)
	afx_msg void OnCalculateButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABONEDLG_H__7636335E_0E2C_48E5_BDEF_40B06E33586E__INCLUDED_)
