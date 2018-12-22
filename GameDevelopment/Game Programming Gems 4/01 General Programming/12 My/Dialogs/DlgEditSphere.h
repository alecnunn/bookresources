#if !defined(AFX_DLGEDITSPHERE_H__7A344541_232F_11D7_9CE0_444553540000__INCLUDED_)
#define AFX_DLGEDITSPHERE_H__7A344541_232F_11D7_9CE0_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEditSphere.h : header file
//

#include "..\resource.h"

namespace GPG4
  {
  class CEditorSceneItem;
  class CEngineMaterial;
  }

/////////////////////////////////////////////////////////////////////////////
// CDlgEditSphere dialog

class CDlgEditSphere : public CDialog
{
// Construction
public:
	CDlgEditSphere(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgEditSphere)
	enum { IDD = IDD_EDITSPHERE };
	UINT	m_uiLOD;
	BOOL	m_boNewMat;
	float	m_fPosX;
	float	m_fPosY;
	float	m_fPosZ;
	float	m_fRadius;
	CString	m_strMaterial;
	//}}AFX_DATA

  bool m_boInitFromStatic;
  GPG4::CEngineMaterial*    m_pMat;
  GPG4::CEditorSceneItem*   m_pItem;

  static float fPosX;
  static float fPosY;
  static float fPosZ;

  static float fRadius;
  static UINT  uiLOD;

  static BOOL  boNewMat;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEditSphere)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  void    UpdateMatName       (void);

	// Generated message map functions
	//{{AFX_MSG(CDlgEditSphere)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChoosemat();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITSPHERE_H__7A344541_232F_11D7_9CE0_444553540000__INCLUDED_)
