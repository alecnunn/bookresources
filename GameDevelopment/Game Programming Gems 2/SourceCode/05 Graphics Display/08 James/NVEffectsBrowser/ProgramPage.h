/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#if !defined(AFX_PROGRAMPAGE_H__3F4609F6_B4F6_499B_9A04_64AC9174ED66__INCLUDED_)
#define AFX_PROGRAMPAGE_H__3F4609F6_B4F6_499B_9A04_64AC9174ED66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgramPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProgramPage dialog

typedef enum tagVIEWTYPE
{
	VIEWTYPE_VERTEX = 0,
	VIEWTYPE_PIXEL = 1
} VIEWTYPE;

class CProgramPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CProgramPage)

// Construction
public:
	CProgramPage();
	virtual ~CProgramPage();

// Dialog Data
	//{{AFX_DATA(CProgramPage)
	enum { IDD = IDD_PROPPAGE_MEDIUM };
	CListCtrl	m_ProgramList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProgramPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:	
	// Setup the data
	bool Setup(CNVEffectsDoc* pDoc, VIEWTYPE ViewType, EBEffect* pEffect, EBEnumValue* pShader);

private:
	bool SetEffect();
	void ClearItems();
	std::vector<std::string> m_vecListBoxStrings;

	TCITEM m_TabItem;
	bool m_bValid;
	EBEffect* m_pEffect;
	VIEWTYPE m_ViewType;
	int m_ColumnWidth;

	std::string m_strEffectName;
	std::string m_strFilePath;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CProgramPage)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRAMPAGE_H__3F4609F6_B4F6_499B_9A04_64AC9174ED66__INCLUDED_)
