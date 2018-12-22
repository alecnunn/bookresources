#if !defined(AFX_DLGEDITBOX_H__4DFC0201_D231_11D6_9CD9_B454EB9B8B20__INCLUDED_)
#define AFX_DLGEDITBOX_H__4DFC0201_D231_11D6_9CD9_B454EB9B8B20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEditBox.h : header file
//

#include "..\resource.h"

namespace GPG4
  {
  class CEditorSceneItem;
  class CEngineMaterial;
  }

/////////////////////////////////////////////////////////////////////////////
// CDlgEditBox dialog

class CDlgEditBox : public CDialog
{
// Construction
public:
	CDlgEditBox(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgEditBox)
	enum { IDD = IDD_EDITBOX };
	float	m_fDimX;
	float	m_fDimY;
	float	m_fDimZ;
	float	m_fPosX;
	float	m_fPosY;
	float	m_fPosZ;
	int		m_iPosX;
	int		m_iPosY;
	int		m_iPosZ;
	BOOL	m_boNewMat;
	CString	m_strMaterial;
	//}}AFX_DATA

  bool m_boInitFromStatic;
  GPG4::CEngineMaterial*    m_pMat;
  GPG4::CEditorSceneItem*   m_pItem;

  static float fDimX;
  static float fDimY;
  static float fDimZ;

  static float fPosX;
  static float fPosY;
  static float fPosZ;

  static int   iPosX;
  static int   iPosY;
  static int   iPosZ;

  static BOOL  boNewMat;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEditBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  void    UpdateMatName       (void);

	// Generated message map functions
	//{{AFX_MSG(CDlgEditBox)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChoosemat();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITBOX_H__4DFC0201_D231_11D6_9CD9_B454EB9B8B20__INCLUDED_)
