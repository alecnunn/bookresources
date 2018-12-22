#if !defined(AFX_SCENEVIEW_H__CC2006E6_AFA3_11D7_9CE3_444553540000__INCLUDED_)
#define AFX_SCENEVIEW_H__CC2006E6_AFA3_11D7_9CE3_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SceneView.h : header file
//

#include  "TreeCtrlEx.h"

class CRTTIPropsDoc;

namespace GPG4
{
  class CEditorCamera;
  class CEditorObj;
  class CEditorSceneItem;
}

/////////////////////////////////////////////////////////////////////////////
// CSceneView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CSceneView : public CFormView
{
protected:
	CSceneView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSceneView)
  CRTTIPropsDoc*              GetDocument         (void);

// Form Data
public:
	//{{AFX_DATA(CSceneView)
	enum { IDD = IDD_SCENEVIEW };
	CTreeCtrlEx	m_Scene;
	//}}AFX_DATA

// Attributes
public:

protected:

  CImageList                  m_imgList;
  CStdString                  m_strInitSel;

  // drag & drop

  bool                        m_boDragging;
  CImageList*                 m_pDragImgList;
  HTREEITEM                   m_hDragItem;
  HTREEITEM                   m_hDragTarget;
  UINT                        m_nTimer;

// Operations
public:

protected:

  bool                        BuildTreeFromScene  (void);
  bool                        AddChildItems       (GPG4::CEditorSceneItem* pItem,HTREEITEM hParent);
  HTREEITEM                   AddSceneItem        (GPG4::CEditorSceneItem* pItem,HTREEITEM hParent);
           
  HTREEITEM                   CreateItem          (GPG4::CEditorSceneItem* pItem,const CStdString& strName="",HTREEITEM hParent=NULL);
  GPG4::CEditorSceneItem*     GetItemData         (HTREEITEM hItem);
  GPG4::CEditorObj*           GetTreeItemData     (HTREEITEM hItem);
  GPG4::CEditorCamera*        GetTreeItemCamera   (HTREEITEM hItem);
  bool                        SelectItem          (GPG4::CEditorSceneItem* pItem);
  bool                        SelectItem          (const CStdString& strName);
  void                        ClearSelection      (HTREEITEM hItem);
  void                        ChangeSelection     (HTREEITEM hItem,GPG4::CEditorObj* pObj);
  void                        OnCheckBox          (void);

  void                        SuccessfulDrag      (HTREEITEM hDest,HTREEITEM hSrc);
  void                        SetTreeHandles      (HTREEITEM hItem);
  void                        SetTreeHandle       (HTREEITEM hItem);

  GPG4::CEditorSceneItem*     OnCreateItem        (u32 u32Type,CStdString strItemType);
  void                        OnUpdateCreateItem  (CCmdUI* pCmdUI);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSceneView)
	protected:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSceneView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CSceneView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnItemexpandedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegindragTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuRename();
	afx_msg void OnUpdateMenuRename(CCmdUI* pCmdUI);
	afx_msg void OnCameraPerspective();
	afx_msg void OnUpdateCameraPerspective(CCmdUI* pCmdUI);
	afx_msg void OnMenuEdit();
	afx_msg void OnUpdateMenuEdit(CCmdUI* pCmdUI);
	afx_msg void OnCreateCamera();
	afx_msg void OnCreateFolder();
	afx_msg void OnCreateBox();
	afx_msg void OnCreateNode();
	afx_msg void OnCreateTexture();
	afx_msg void OnCreateSphere();
	afx_msg void OnCreateMaterial();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ConsoleView.cpp
inline CRTTIPropsDoc* CSceneView::GetDocument()
   { return (CRTTIPropsDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCENEVIEW_H__CC2006E6_AFA3_11D7_9CE3_444553540000__INCLUDED_)
