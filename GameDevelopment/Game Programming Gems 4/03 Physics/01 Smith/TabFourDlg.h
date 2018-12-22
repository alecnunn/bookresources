#if !defined(AFX_TABFOURDLG_H__7E972F5E_8D25_49FF_B1EF_38C59894724D__INCLUDED_)
#define AFX_TABFOURDLG_H__7E972F5E_8D25_49FF_B1EF_38C59894724D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabFourDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabFourDlg dialog

class CTabFourDlg : public CDialog
{
// Construction
public:
	CTabFourDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTabFourDlg)
	enum { IDD = IDD_TAB_FOUR_DLG };
	double	m_dProbabilityOfHit;
	float	m_fLethalArea;
	double	m_dStandardDeviation;
	double	m_dNumberOfRounds;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabFourDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabFourDlg)
	afx_msg void OnCalculateButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABFOURDLG_H__7E972F5E_8D25_49FF_B1EF_38C59894724D__INCLUDED_)
