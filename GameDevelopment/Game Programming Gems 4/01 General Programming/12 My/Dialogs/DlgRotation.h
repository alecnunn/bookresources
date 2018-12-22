#if !defined(AFX_DLGROTATION_H__9201E301_4501_11D7_9CE1_444553540000__INCLUDED_)
#define AFX_DLGROTATION_H__9201E301_4501_11D7_9CE1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRotation.h : header file
//

#include  "..\resource.h"
#include  "Quaternion.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgRotation dialog

class CDlgRotation : public CDialog
{
// Construction
public:
	CDlgRotation(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgRotation)
	enum { IDD = IDD_ROTATION };
	float	m_fEulerX;
	float	m_fEulerY;
	float	m_fEulerZ;
	//}}AFX_DATA

  GPG4::CQuaternion           m_qRot;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRotation)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgRotation)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGROTATION_H__9201E301_4501_11D7_9CE1_444553540000__INCLUDED_)
