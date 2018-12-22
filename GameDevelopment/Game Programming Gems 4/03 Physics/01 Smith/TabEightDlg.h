#if !defined(AFX_TABEIGHTDLG_H__58096303_22C4_422D_817E_8B32A91F5CBD__INCLUDED_)
#define AFX_TABEIGHTDLG_H__58096303_22C4_422D_817E_8B32A91F5CBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabEightDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabEightDlg dialog

class CTabEightDlg : public CDialog
{
// Construction
public:
	CTabEightDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTabEightDlg)
	enum { IDD = IDD_TAB_EIGHT_DLG };
	double	m_dAngleToFarEdge;
	double	m_dAngleToNearEdge;
	int		m_nNumberOfProjectiles;
	double	m_dProbabilityOfHit;
	double	m_dRangeToTargetFromDet;
	double	m_dVulnerableArea;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabEightDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabEightDlg)
	afx_msg void OnCalculateButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABEIGHTDLG_H__58096303_22C4_422D_817E_8B32A91F5CBD__INCLUDED_)
