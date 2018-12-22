#if !defined(AFX_DLGITEMNAME_H__36268A61_F3C5_11D5_9CD7_444553540000__INCLUDED_)
#define AFX_DLGITEMNAME_H__36268A61_F3C5_11D5_9CD7_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgitemname.h : header file
//

namespace GPG4
  {
  class CEditorScene;
  }

/////////////////////////////////////////////////////////////////////////////
// CDlgItemName dialog

class CDlgItemName : public CDialog
{
// Construction
public:
	CDlgItemName(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgItemName)
	enum { IDD = IDD_ITEMNAME };
	CString	m_strName;
	//}}AFX_DATA

  GPG4::CEditorScene*         m_pEditorScene;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgItemName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgItemName)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChangeName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGITEMNAME_H__36268A61_F3C5_11D5_9CD7_444553540000__INCLUDED_)
