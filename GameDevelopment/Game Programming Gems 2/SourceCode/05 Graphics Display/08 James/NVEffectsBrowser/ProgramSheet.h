/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#if !defined(AFX_PROGRAMSHEET_H__7E4CCE28_D36A_4F3D_844C_0DE4F5AA9931__INCLUDED_)
#define AFX_PROGRAMSHEET_H__7E4CCE28_D36A_4F3D_844C_0DE4F5AA9931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgramSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProgramSheet
#define WM_RESIZEPAGE (WM_USER + 1000)
class CProgramFormView;
class CProgramSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CProgramSheet)

// Construction
public:
	CProgramSheet(CWnd* pParentWnd = NULL);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgramSheet)
	public:
	virtual BOOL OnInitDialog();
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CProgramSheet();
	void Resize();
	bool SetEffect(CNVEffectsDoc* pDoc, VIEWTYPE ViewType, EBEffect* pEffect);


private:

	std::vector<CProgramPage*> m_vecProgramPages;
	CRect m_rectPage;
	CProgramFormView* m_pFormView;
	VIEWTYPE m_ViewType;
	CProgramPage* m_pDummyPage;
	EBEffect* m_pEffect;

	// Generated message map functions
protected:
	//{{AFX_MSG(CProgramSheet)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg LRESULT OnResizePage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRAMSHEET_H__7E4CCE28_D36A_4F3D_844C_0DE4F5AA9931__INCLUDED_)
