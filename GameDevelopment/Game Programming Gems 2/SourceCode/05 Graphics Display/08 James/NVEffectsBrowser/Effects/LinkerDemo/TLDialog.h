/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#if !defined(AFX_TLDialog_H__2FDB0163_0232_4026_9FC9_585C3D6095C4__INCLUDED_)
#define AFX_TLDialog_H__2FDB0163_0232_4026_9FC9_585C3D6095C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TLDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTLDialog dialog

class CTLDialog : public CDialog
{
// Construction
public:
	CTLDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTLDialog)
	enum { IDD = IDD_TLOPTIONS };
	CSpinButtonCtrl	m_PointSpin;
	CSpinButtonCtrl	m_DirectionalSpin;
	DWORD	m_dwNumDirectionalLights;
	DWORD	m_dwNumPointLights;
	int		m_TexGen;
	BOOL	m_bEyeSpace;
	BOOL	m_bSpecular;
	BOOL	m_bLocalViewer;
	BOOL	m_bLighting;
	BOOL	m_bTextures;
	BOOL	m_bWhiteLight;
	BOOL	m_bSpecularRDotL;
	int		m_Fog;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTLDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTLDialog)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TLDialog_H__2FDB0163_0232_4026_9FC9_585C3D6095C4__INCLUDED_)
