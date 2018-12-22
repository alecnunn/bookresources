#if !defined(AFX_PROPVIEW_H__CC2006E4_AFA3_11D7_9CE3_444553540000__INCLUDED_)
#define AFX_PROPVIEW_H__CC2006E4_AFA3_11D7_9CE3_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropView.h : header file
//

#include "GridCtrl\BtnDatabase.h"
#include "StdString.h"

class CGridCtrl;
class CRTTIPropsDoc;

namespace GPG4
{
  class CPersistent;
  class CProperty;
  class CPropList;
}

/////////////////////////////////////////////////////////////////////////////
// CPropView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CPropView : public CFormView
{
protected:
	CPropView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPropView)
  CRTTIPropsDoc*                GetDocument         (void);

// Form Data
public:
	//{{AFX_DATA(CPropView)
	enum { IDD = IDD_PROPVIEW };
	CStatic	m_Name;
	CTabCtrl	m_Tab;
	CString	m_strName;
	//}}AFX_DATA

// Attributes
public:

protected:

  CGridCtrl*                  m_pGrid;
  GPG4::CPersistent*          m_pObj;

  CBtnDataBase                m_BtnDataBase;

// Operations
public:

  void                        ResetGridItemData   (void);
  void                        ResizeName          (void);

  bool                        SetObject           (GPG4::CPersistent* pObj);
  GPG4::CPersistent*          GetNextTab          (GPG4::CPersistent* pObj);
  void                        FillTab             (void);
  void                        FillProps           (GPG4::CPersistent* pObj,GPG4::CPropList* pDesc,DWORD* pdwRow);
  bool                        PropToBeShown       (GPG4::CPersistent* pObj,GPG4::CProperty* pProp);
  bool                        PropReadOnly        (GPG4::CPersistent* pObj,GPG4::CProperty* pProp);
  void                        Update              (void);
  bool                        ModifyProp          (GPG4::CPersistent* pObj,GPG4::CProperty* pProp,const CStdString& strNew);
  bool                        SpecialEditing      (GPG4::CPersistent* pObj,GPG4::CProperty* pProp,CStdString* pstrNew);
  CStdString                  SpecialGetValue     (GPG4::CPersistent* pObj,GPG4::CProperty* pProp);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPropView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CPropView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
  afx_msg void OnGridBeginEdit(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnGridEndEdit(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnGridClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ConsoleView.cpp
inline CRTTIPropsDoc* CPropView::GetDocument()
   { return (CRTTIPropsDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPVIEW_H__CC2006E4_AFA3_11D7_9CE3_444553540000__INCLUDED_)
