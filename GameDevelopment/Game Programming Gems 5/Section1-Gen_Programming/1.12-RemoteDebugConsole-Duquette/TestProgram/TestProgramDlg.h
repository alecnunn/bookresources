// TestProgramDlg.h : header file
//

#pragma once
#include "RemoteLogger.h"
#include "afxwin.h"


// CTestProgramDlg dialog
class CTestProgramDlg : public CDialog
{
// Construction
public:
	CTestProgramDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTPROGRAM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON   m_hIcon;
    UINT    m_uiTimer;
    CRemoteLogger m_RemoteLogger;

	// Generated message map functions
	virtual BOOL OnInitDialog();
    virtual void OnTimer(UINT_PTR nIDEvent);
    virtual void OnCancel();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedStartlog();
    afx_msg void OnBnClickedStoplog();
    afx_msg void OnBnClickedPause();
    afx_msg void OnBnClickedAddpage();

    CString m_Edit_PageName;
    CString m_Edit_LogText;
    CString m_Edit_ValueName;
    CString m_Edit_Value;
    afx_msg void OnBnClickedBtnAddlog();
    afx_msg void OnBnClickedBtnAddvalue();
    CComboBox m_Combo_PageName;
};
