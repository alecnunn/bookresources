#if !defined(AFX_TREECTRLEX_H__7025C061_A6A3_11D5_9CD6_444553540000__INCLUDED_)
#define AFX_TREECTRLEX_H__7025C061_A6A3_11D5_9CD6_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeCtrlEx.h : header file
//

class CTreeCtrlEx;
typedef HRESULT (*ItemCallbackEx) (CTreeCtrl* pTree,HTREEITEM hItem);

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx window

class CTreeCtrlEx : public CTreeCtrl
{
// Construction
public:
	CTreeCtrlEx();

// Attributes
public:

  static					  ItemCallbackEx      m_pItemCallback;
  static            HTREEITEM           m_hParent;
  CImageList*                           m_pDragImgList;
          
// Operations
public:

	void							ParseBranch 				(HTREEITEM hItem,bool boRecurse=true);
  void              CopyItem            (HTREEITEM hDest,HTREEITEM hSrc);
  void              CopySubtree         (HTREEITEM hDest,HTREEITEM hSrc);
  HTREEITEM         InsertItemAndSubtree(HTREEITEM hDest,HTREEITEM hSrc);
  void  					  CopyItem            (HTREEITEM hDest,CTreeCtrlEx& treeSrc,HTREEITEM hSrc);
  void              CopySubtree         (HTREEITEM hDest,CTreeCtrlEx& treeSrc,HTREEITEM hSrc);
          
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeCtrlEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeCtrlEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeCtrlEx)
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREECTRLEX_H__7025C061_A6A3_11D5_9CD6_444553540000__INCLUDED_)
