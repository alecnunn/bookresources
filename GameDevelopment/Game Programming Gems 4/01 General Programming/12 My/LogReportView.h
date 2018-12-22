#if !defined(AFX_LOGREPORTVIEW_H__160AA861_62DD_11D6_9CD8_444553540000__INCLUDED_)
#define AFX_LOGREPORTVIEW_H__160AA861_62DD_11D6_9CD8_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogReportView.h : header file
//

class CLogReportData;
class CRTTIPropsDoc;

/////////////////////////////////////////////////////////////////////////////
// CLogReportView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CLogReportView : public CFormView
{
protected:
	CLogReportView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CLogReportView)

// Form Data
public:
	//{{AFX_DATA(CLogReportView)
	enum { IDD = IDD_LOGREPORT };
	CListCtrl	m_Report;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogReportView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CLogReportView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

  CRTTIPropsDoc*    GetDocument         (void);
  void              AddMessage          (CLogReportData* pLogData);
  void              AddSubMessage       (const char* pszMsg);

	// Generated message map functions
	//{{AFX_MSG(CLogReportView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in LogReportView.cpp
inline CRTTIPropsDoc* CLogReportView::GetDocument()
   { return (CRTTIPropsDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGREPORTVIEW_H__160AA861_62DD_11D6_9CD8_444553540000__INCLUDED_)
