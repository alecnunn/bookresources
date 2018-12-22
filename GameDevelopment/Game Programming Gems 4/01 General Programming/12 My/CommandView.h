#if !defined(AFX_COMMANDVIEW_H__CC2006E1_AFA3_11D7_9CE3_444553540000__INCLUDED_)
#define AFX_COMMANDVIEW_H__CC2006E1_AFA3_11D7_9CE3_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CommandView.h : header file
//

#include  "TreeCtrlEx.h"

class CRTTIPropsDoc;

/////////////////////////////////////////////////////////////////////////////
// CCommandView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCommandView : public CFormView
{
protected:
	CCommandView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCommandView)
  CRTTIPropsDoc*    GetDocument         (void);

// Form Data
public:
	//{{AFX_DATA(CCommandView)
	enum { IDD = IDD_COMMANDVIEW };
	CTreeCtrlEx	m_SceneGraph;
	CButton	m_btnMoveCam;
	CButton	m_btnMoveSel;
	BOOL	m_boReadOnlyProps;
	//}}AFX_DATA

// Attributes
public:

protected:

  CImageList                  m_imgList;

  // drag & drop

  bool                        m_boDragging;
  CImageList*                 m_pDragImgList;
  HTREEITEM                   m_hDragItem;
  HTREEITEM                   m_hDragTarget;
  UINT                        m_nTimer;

// Operations
public:

protected:

  void                        UpdateControls      (void);

  void                        UpdateGraph         (void);
  void                        BuildSubtree        (GPG4::CEngineNode* pNode,HTREEITEM hParent);
  void                        SuccessfulDrag      (HTREEITEM hDest,HTREEITEM hSrc);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommandView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCommandView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCommandView)
	afx_msg void OnClearsel();
	afx_msg void OnMovesel();
	afx_msg void OnMovecamera();
	afx_msg void OnReadonlyprops();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBegindragTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKeydownTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in 3DView.cpp
inline CRTTIPropsDoc* CCommandView::GetDocument()
   { return (CRTTIPropsDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMANDVIEW_H__CC2006E1_AFA3_11D7_9CE3_444553540000__INCLUDED_)
