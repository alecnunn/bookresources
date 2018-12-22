#if !defined(AFX_DLGCHOOSETEXTURE_H__34DD3101_5543_11D7_9CE1_444553540000__INCLUDED_)
#define AFX_DLGCHOOSETEXTURE_H__34DD3101_5543_11D7_9CE1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgChooseTexture.h : header file
//

namespace GPG4
{
  class CEditorSceneItem;
  class CEngineTexture;
}

#include  "..\resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgChooseTexture dialog

class CDlgChooseTexture : public CDialog
{
// Construction
public:
	CDlgChooseTexture(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgChooseTexture)
	enum { IDD = IDD_CHOOSETEX };
	CListCtrl	m_TexList;
	//}}AFX_DATA

  GPG4::CEditorSceneItem*   m_pSceneItem;
  GPG4::CEngineTexture*     m_pSelected;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChooseTexture)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  void    FillList            (GPG4::CEditorSceneItem* pItem);

	// Generated message map functions
	//{{AFX_MSG(CDlgChooseTexture)
	virtual BOOL OnInitDialog();
	afx_msg void OnNotex();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHOOSETEXTURE_H__34DD3101_5543_11D7_9CE1_444553540000__INCLUDED_)
