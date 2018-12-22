/* Copyright (C) William van der Sterren, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William van der Sterren, 2002"
 */

/* Copyright (C) James Matthews, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) James Matthews, 2001"
 */

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
  CStatusBar* GetStatusBar();

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
