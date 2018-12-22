#if !defined(AFX_3DVIEW_H__930D3CC1_AFA2_11D7_9CE3_444553540000__INCLUDED_)
#define AFX_3DVIEW_H__930D3CC1_AFA2_11D7_9CE3_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 3DView.h : header file
//

#include  "EditorCamera.h"

enum
{
  CURSOR_ARROW,
  CURSOR_SIZEALL,
  CURSOR_VERTI,
  CURSOR_HORIZ,
};

namespace GPG4
{
  class CEditorTool;
  class CEngineNode;
}

class CRTTIPropsDoc;

/////////////////////////////////////////////////////////////////////////////
// C3DView view

class C3DView : public CView
{
protected:
	C3DView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(C3DView)
  CRTTIPropsDoc*              GetDocument         (void);

// Operations
public:

  GPG4::CEditorCamera*        GetCurrentCamera    (void) const;

protected:

  void                        DrawFrame           (void);
  bool                        CreateRenderer      (void);
  GPG4::CEditorTool*          SetTool             (GPG4::CEditorTool* pTool);
  void                        SetCurrentCamera    (GPG4::CEditorCamera* pCamera);

// Attributes
public:

protected:

  u32                         m_u32Renderer;
  GPG4::CEditorCameraSP       m_spCurrentCam;

  HCURSOR                     m_hCursor,m_hArrow;
  HCURSOR                     m_hSizeAll,m_hSizeNS,m_hSizeWE;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C3DView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~C3DView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(C3DView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void On3dMovecamera();
	afx_msg void OnUpdate3dMovecamera(CCmdUI* pCmdUI);
	afx_msg void On3dPerspective();
	afx_msg void OnUpdate3dPerspective(CCmdUI* pCmdUI);
	afx_msg void On3dMovesel();
	afx_msg void OnUpdate3dMovesel(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in 3DView.cpp
inline CRTTIPropsDoc* C3DView::GetDocument()
   { return (CRTTIPropsDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_3DVIEW_H__930D3CC1_AFA2_11D7_9CE3_444553540000__INCLUDED_)
