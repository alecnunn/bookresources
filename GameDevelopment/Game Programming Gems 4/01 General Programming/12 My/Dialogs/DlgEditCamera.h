#if !defined(AFX_DLGEDITCAMERA_H__AEC57821_D24D_11D6_9CD9_B454EB9B8B20__INCLUDED_)
#define AFX_DLGEDITCAMERA_H__AEC57821_D24D_11D6_9CD9_B454EB9B8B20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEditCamera.h : header file
//

#include "..\resource.h"

/////////////////////////////////////////////////////////////////////////////
// DlgEditCamera dialog

class CDlgEditCamera : public CDialog
{
// Construction
public:
	CDlgEditCamera(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgEditCamera)
	enum { IDD = IDD_EDITCAMERA };
	int		m_iDir;
	float	m_fFar;
	float	m_fFov;
	float	m_fNear;
	float	m_fPosX;
	float	m_fPosY;
	float	m_fPosZ;
	//}}AFX_DATA

  bool m_boInitFromStatic;

  static int   iDir;

  static float fFov;
  static float fNear;
  static float fFar;

  static float fPosX;
  static float fPosY;
  static float fPosZ;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEditCamera)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEditCamera)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITCAMERA_H__AEC57821_D24D_11D6_9CD9_B454EB9B8B20__INCLUDED_)
