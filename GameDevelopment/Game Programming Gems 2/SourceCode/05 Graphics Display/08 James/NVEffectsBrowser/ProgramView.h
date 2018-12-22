/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#if !defined(AFX_PROGRAMVIEW_H__46527D48_E98E_4E39_BC35_498FFA32CAB2__INCLUDED_)
#define AFX_PROGRAMVIEW_H__46527D48_E98E_4E39_BC35_498FFA32CAB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef enum tagVIEWTYPE
{
	VIEWTYPE_VERTEX = 0,
	VIEWTYPE_PIXEL = 1
} VIEWTYPE;

class CProgramView : public CListViewEx
{
protected: // create from serialization only
	CProgramView();
	DECLARE_DYNCREATE(CProgramView)

// Attributes
public:


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgramView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	std::vector<ColumnDescriptor> m_vecColumns;

	virtual ~CProgramView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:	
	// Setup the data
	bool SetEffect(EBEffect* pEffect);
	bool ChangeTitle(const std::string& strTitle);
	void SetViewType(VIEWTYPE ViewType) { m_ViewType = ViewType; }
	VIEWTYPE GetViewType() const { return m_ViewType; }

private:

	char* m_chColumn;
	VIEWTYPE m_ViewType;

// Generated message map functions
protected:
	//{{AFX_MSG(CProgramView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRAMVIEW_H__46527D48_E98E_4E39_BC35_498FFA32CAB2__INCLUDED_)
