// PennyMatchingDlg.h : header file
//

#if !defined(AFX_PENNYMATCHINGDLG_H__16AEA787_A0B4_11D5_B5E8_0010A7018204__INCLUDED_)
#define AFX_PENNYMATCHINGDLG_H__16AEA787_A0B4_11D5_B5E8_0010A7018204__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPennyMatchingDlg dialog

#include "predictor.h"

class CPennyMatchingDlg : public CDialog
{
// Construction
public:
	CPennyMatchingDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPennyMatchingDlg)
	enum { IDD = IDD_PENNYMATCHING_DIALOG };
	CButton	m_Tail;
	CButton	m_Head;
	CButton	m_ToggleStatistics;
	CComboBox	m_Predictors;
	CString	m_MachineScore;
	CString	m_NumberOfPlays;
	CString	m_SuccessRatio;
	CString	m_YourScore;
	CString	m_ThresholdRatio;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPennyMatchingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bExpanded;
	int m_nSuccessfulPredictions;
	int m_nTotalPredictions;
	void ProcessSelection(int Selection);
	void UpdateDisplay();
	int m_nMachineWins;
	int m_nPlayerWins;
	int m_nRatio;
	int m_nPlays;
	HBITMAP m_hBitmapHead;
	HBITMAP m_hBitmapTail;
	void ResetGame();
	CPredictor * m_pPredictor;
	void RegisterPredictor(const CString & PredictorName, CPredictor * pPredictor);
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPennyMatchingDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelChangePredictor();
	afx_msg void OnDestroy();
	afx_msg void OnHead();
	afx_msg void OnTail();
	afx_msg void OnToggleStats();
	afx_msg void OnAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PENNYMATCHINGDLG_H__16AEA787_A0B4_11D5_B5E8_0010A7018204__INCLUDED_)
