/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

#if !defined(AFX_TREEVIEW_H__B78B008D_DA2D_11D3_B29E_00902752C506__INCLUDED_)
#define AFX_TREEVIEW_H__B78B008D_DA2D_11D3_B29E_00902752C506__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNVEffectsDoc;

class CShaderTree : public CTreeView
{
protected: // create from serialization only
	CShaderTree();
	DECLARE_DYNCREATE(CShaderTree)

// Attributes
public:
	CNVEffectsDoc* GetDocument();

// Operations
public:
	virtual void ExpandAll(HTREEITEM hParent);
	virtual bool FindAndSelect(HTREEITEM hParent, EBEffect* pCurrentTest);
	virtual void PopulateTree();

	CImageList m_ImageList;
	HTREEITEM m_hRoot;
	HTREEITEM m_hVertexShaderRoot;
	HTREEITEM m_hPixelShaderRoot;

protected:
	virtual HTREEITEM FindNamedEntry(HTREEITEM hParent, const std::string& strName);
	bool m_bPopulating;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShaderTree)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CShaderTree();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CShaderTree)
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TreeView.cpp
inline CNVEffectsDoc* CShaderTree::GetDocument()
   { return (CNVEffectsDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREEVIEW_H__B78B008D_DA2D_11D3_B29E_00902752C506__INCLUDED_)
