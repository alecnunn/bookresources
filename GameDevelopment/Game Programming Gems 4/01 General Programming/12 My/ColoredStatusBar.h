#if !defined(AFX_COLOREDSTATUSBAR_H__95120A64_CD7A_11D6_9CD9_D521C94B1621__INCLUDED_)
#define AFX_COLOREDSTATUSBAR_H__95120A64_CD7A_11D6_9CD9_D521C94B1621__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColoredStatusBar.h : header file
//

#include  "StdString.h"

/////////////////////////////////////////////////////////////////////////////
// CColoredStatusBar window

class CColoredStatusBar : public CStatusBar
{
// Construction
public:
	CColoredStatusBar();

// Attributes
public:

  CStdString        m_strText;

// Operations
public:

protected:

  void              DrawColoredString   (CDC& dc,const char* pszTxt);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColoredStatusBar)
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColoredStatusBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColoredStatusBar)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLOREDSTATUSBAR_H__95120A64_CD7A_11D6_9CD9_D521C94B1621__INCLUDED_)
