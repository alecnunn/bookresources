
#ifndef _AFX_ASE_MAINFRM_H_
#define _AFX_ASE_MAINFRM_H_

#if _MSC_VER > 1000
#pragma once
#endif

class CAseDoc;
class CAseView;
class CNodeView;

class CMainFrame : public CFrameWnd
{
protected:
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

public:
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

public:
	virtual ~CMainFrame();
	CAseView* GetRightPane();
	CNodeView *GetLeftPane();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CStatusBar   m_wndStatusBar;
	CToolBar     m_wndToolBar;
	CReBar       m_wndReBar;
	CDialogBar   m_wndDlgBar;
	CSplitterWnd m_wndSplitter;

	CAseDoc		*GetAseDocument();

	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPathingDisplayGoalNode();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}

#endif
