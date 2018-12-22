#if !defined(AFX_PLUGINVIEW_H__6A064FEC_F4DB_4152_A65B_EBAE02473144__INCLUDED_)
#define AFX_PLUGINVIEW_H__6A064FEC_F4DB_4152_A65B_EBAE02473144__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PluginView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PluginView dialog

class PluginView : public CDialog
{
// Construction
public:
	PluginView(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(PluginView)
	enum { IDD = IDD_PLUGINS };
	CListBox	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PluginView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PluginView)
	virtual BOOL OnInitDialog();
	afx_msg void OnAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLUGINVIEW_H__6A064FEC_F4DB_4152_A65B_EBAE02473144__INCLUDED_)
