#if !defined(AFX_TABTWODLG_H__EEF83027_F198_4B3D_B67C_DF7D73512FE4__INCLUDED_)
#define AFX_TABTWODLG_H__EEF83027_F198_4B3D_B67C_DF7D73512FE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabTwoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabTwoDlg dialog

class CTabTwoDlg : public CDialog
{
// Construction
public:
	CTabTwoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTabTwoDlg)
	enum { IDD = IDD_TAB_TWO_DLG };
	double	m_dProbabilityOfHit;
	double	m_dStandardDeviation;
	double	m_dRadius;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabTwoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabTwoDlg)
	afx_msg void OnCalculateButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABTWODLG_H__EEF83027_F198_4B3D_B67C_DF7D73512FE4__INCLUDED_)
