// MainFrame.h : interface of the MainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRAME_H__CC3E79CC_919D_411D_B552_9B7E4EC4F8CA__INCLUDED_)
#define AFX_MAINFRAME_H__CC3E79CC_919D_411D_B552_9B7E4EC4F8CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChildView.h"

class MainFrame : public CFrameWnd
{
	
public:
	MainFrame();
protected: 
	DECLARE_DYNAMIC(MainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~MainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

    void AddExporter ( const std::string & strExporterName );
    void RemoveExporter ( const std::string & strExporterName );

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CReBar      m_wndReBar;
	ChildView    m_wndView;

// Generated message map functions
protected:
	//{{AFX_MSG(MainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnPluginsView();
	afx_msg void OnPluginsReload();
	afx_msg void OnPluginsUnload();
	afx_msg void OnDestroy();
	//}}AFX_MSG
    afx_msg void OnExporter(UINT nID);
	DECLARE_MESSAGE_MAP()

private:
    CMenu * GetExporterMenu ();
    std::string GetPluginDirectory ();

    int m_nNumExporterPlugins;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRAME_H__CC3E79CC_919D_411D_B552_9B7E4EC4F8CA__INCLUDED_)
