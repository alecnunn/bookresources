/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#if !defined(AFX_PROGRAMFORMVIEW_H__0A07682E_3B39_4D2C_AF41_D458ABE26E19__INCLUDED_)
#define AFX_PROGRAMFORMVIEW_H__0A07682E_3B39_4D2C_AF41_D458ABE26E19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgramFormView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProgramFormView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CProgramFormView : public CFormView
{
protected:
	CProgramFormView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CProgramFormView)

// Form Data
public:
	//{{AFX_DATA(CProgramFormView)
	enum { IDD = IDD_NVEFFECTSBROWSER_FORM };
	//}}AFX_DATA

// Attributes
public:

// Operations
public:	
	// Setup the data
	bool SetEffect(EBEffect* pEffect);
	bool ChangeTitle(const std::string& strTitle);
	void SetViewType(VIEWTYPE ViewType) { m_ViewType = ViewType; }
	VIEWTYPE GetViewType() const { return m_ViewType; }
	
private:
	void ResizeControl(CWnd *pControl);
	VIEWTYPE m_ViewType;
	CProgramSheet* m_pProgramSheet;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgramFormView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CProgramFormView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CProgramFormView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRAMFORMVIEW_H__0A07682E_3B39_4D2C_AF41_D458ABE26E19__INCLUDED_)
