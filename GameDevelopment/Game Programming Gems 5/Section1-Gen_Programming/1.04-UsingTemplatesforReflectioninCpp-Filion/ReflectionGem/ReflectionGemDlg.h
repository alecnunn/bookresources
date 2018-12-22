// ReflectionGemDlg.h : header file
//

#pragma once

#include "afxwin.h"
#include "ReflectiveUI.h"
#include "TestClasses.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CReflectionGemDlg
//
// MFC main dialog class.
//
// Just a standard dialog, really. We use a CXTPPropertyGrid which is a commercial control to
// display property pages similar to what is seen the Visual Studio .NET environment. This control
// can be evaluated at http://www.codejock.com
//
// The code that builds the dynamic property page is in CReflectiveUI.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

class CReflectionGemDlg : public CDialog
{

public:

	enum { IDD = IDD_REFLECTIONGEM_DIALOG };

	CReflectionGemDlg(CWnd* pParent = NULL);			// standard constructor


protected :

	HICON				m_hIcon;			// Minimized MFC icon
	CXTPPropertyGrid	m_PropertyGrid;		// Our property grid UI
	CStatic				m_PlaceHolder;		// A placeholder put in the window - the property grid will be placed
											// on top of the placehold
	CReflectiveUI		m_UI;				// The class handling the property grid UI

	CMonsterStats		m_TestObject;		// Our demonstration object

	//----------------------------------------------------------------------------------------------
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	//----------------------------------------------------------------------------------------------
	afx_msg LRESULT OnGridNotify(WPARAM, LPARAM);	// Messages received from the property grid UI

	//----------------------------------------------------------------------------------------------
	virtual BOOL	OnInitDialog();					// Dialog initialization

	DECLARE_MESSAGE_MAP()
};
