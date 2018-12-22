#if !defined(AFX_ABOUTBOX_H__2DEA5D1D_4989_4A93_8C30_F21D72313B00__INCLUDED_)
#define AFX_ABOUTBOX_H__2DEA5D1D_4989_4A93_8C30_F21D72313B00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AboutBox.h : header file
//


#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// AboutBox dialog

class AboutBox : public CDialog
{
// Construction
public:
	AboutBox(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AboutBox)
	enum { IDD = IDD_ABOUTBOX };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AboutBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AboutBox)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABOUTBOX_H__2DEA5D1D_4989_4A93_8C30_F21D72313B00__INCLUDED_)
