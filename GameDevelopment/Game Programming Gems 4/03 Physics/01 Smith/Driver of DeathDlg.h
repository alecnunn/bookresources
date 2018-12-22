// Driver of DeathDlg.h : header file
//

#if !defined(AFX_DRIVEROFDEATHDLG_H__CE206C8F_CF9A_45D7_8B68_916066F7BBF7__INCLUDED_)
#define AFX_DRIVEROFDEATHDLG_H__CE206C8F_CF9A_45D7_8B68_916066F7BBF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyTabCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CDriverofDeathDlg dialog

class CDriverofDeathDlg : public CDialog
{
// Construction
public:
	CDriverofDeathDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDriverofDeathDlg)
	enum { IDD = IDD_DRIVEROFDEATH_DIALOG };
	CMyTabCtrl	m_ctrlTabControl;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDriverofDeathDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDriverofDeathDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRIVEROFDEATHDLG_H__CE206C8F_CF9A_45D7_8B68_916066F7BBF7__INCLUDED_)
