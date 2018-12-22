/* Copyright (C) Eric Dybsand, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eric Dybsand, 2001"
 */
#if !defined(AFX_TESTFUSMDLG_H__7C661141_EEA7_11D4_8C04_0080C8FE83CE__INCLUDED_)
#define AFX_TESTFUSMDLG_H__7C661141_EEA7_11D4_8C04_0080C8FE83CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestFuSMDlg.h : header file
//

// forward reference
class FuSMclass;

/////////////////////////////////////////////////////////////////////////////
// TestFuSMDlg dialog

class TestFuSMDlg : public CDialog
{
	FuSMclass *m_pFuSMclass;	// pointer to parent's instance of FuSM

// Construction
public:
	TestFuSMDlg(CWnd* pParent, FuSMclass *pFuSMclass);   // standard constructor

// Dialog Data
	//{{AFX_DATA(TestFuSMDlg)
	enum { IDD = IDD_FUSMDLG };
	CListBox	m_lbStates;
	CComboBox	m_cbInputs;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TestFuSMDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TestFuSMDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnSelchangeInputscombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTFUSMDLG_H__7C661141_EEA7_11D4_8C04_0080C8FE83CE__INCLUDED_)
