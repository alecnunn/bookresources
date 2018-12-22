#if !defined(AFX_DLGCHOOSEMATERIAL_H__B8916941_4B60_11D7_9CE1_F7C11BF76803__INCLUDED_)
#define AFX_DLGCHOOSEMATERIAL_H__B8916941_4B60_11D7_9CE1_F7C11BF76803__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgChooseMaterial.h : header file
//

namespace GPG4
{
  class CEditorSceneItem;
  class CEngineMaterial;
}

#include  "..\resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgChooseMaterial dialog

class CDlgChooseMaterial : public CDialog
{
// Construction
public:
	CDlgChooseMaterial(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgChooseMaterial)
	enum { IDD = IDD_CHOOSEMAT };
	CListCtrl	m_MatList;
	//}}AFX_DATA

  GPG4::CEditorSceneItem*   m_pSceneItem;
  GPG4::CEngineMaterial*    m_pSelected;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChooseMaterial)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  void    FillList            (GPG4::CEditorSceneItem* pItem);

	// Generated message map functions
	//{{AFX_MSG(CDlgChooseMaterial)
	virtual BOOL OnInitDialog();
	afx_msg void OnNomat();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHOOSEMATERIAL_H__B8916941_4B60_11D7_9CE1_F7C11BF76803__INCLUDED_)
