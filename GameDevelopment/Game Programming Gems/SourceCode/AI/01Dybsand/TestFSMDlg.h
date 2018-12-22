/* Copyright (C) Eric Dybsand, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eric Dybsand, 2000"
 */
#if !defined(AFX_TESTFSMDLG_H__73B0A300_D3DF_11D3_911B_0080C8FE83CE__INCLUDED_)
#define AFX_TESTFSMDLG_H__73B0A300_D3DF_11D3_911B_0080C8FE83CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestFSMDlg.h : header file
//

// forward reference
class FSMclass;

/////////////////////////////////////////////////////////////////////////////
// TestFSMDlg dialog

class TestFSMDlg : public CDialog
{
	FSMclass *m_pFSMclass;	// pointer to parent's instance of FSM
	int m_iCurrentState;	// IDs for current and output states
	int m_iOutputState;

// Construction
public:
	TestFSMDlg(CWnd* pParent, FSMclass *pFSMclass);   // standard constructor

// Dialog Data
	//{{AFX_DATA(TestFSMDlg)
	enum { IDD = IDD_TESTFSMDLG };
	CEdit	m_editOutputState;
	CEdit	m_editCurrentState;
	CComboBox	m_cbInputs;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TestFSMDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TestFSMDlg)
	afx_msg void OnAccept();
	afx_msg void OnExit();
	afx_msg void OnSelchangeInputscombo();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTFSMDLG_H__73B0A300_D3DF_11D3_911B_0080C8FE83CE__INCLUDED_)
