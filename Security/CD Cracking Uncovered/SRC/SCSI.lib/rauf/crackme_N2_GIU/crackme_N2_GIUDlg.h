// crackme_N2_GIUDlg.h : header file
//

#if !defined(AFX_CRACKME_N2_GIUDLG_H__3EE8B0FF_27EE_4631_BEEA_59A0FA7422EC__INCLUDED_)
#define AFX_CRACKME_N2_GIUDLG_H__3EE8B0FF_27EE_4631_BEEA_59A0FA7422EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCrackme_N2_GIUDlg dialog

class CCrackme_N2_GIUDlg : public CDialog
{
// Construction
public:
	CCrackme_N2_GIUDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCrackme_N2_GIUDlg)
	enum { IDD = IDD_CRACKME_N2_GIU_DIALOG };
	CButton	m_chk;
	CButton	m_ok;
	CProgressCtrl	m_progress;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCrackme_N2_GIUDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCrackme_N2_GIUDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRACKME_N2_GIUDLG_H__3EE8B0FF_27EE_4631_BEEA_59A0FA7422EC__INCLUDED_)
