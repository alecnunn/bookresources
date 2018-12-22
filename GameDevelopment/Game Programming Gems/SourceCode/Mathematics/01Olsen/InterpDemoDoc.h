/* Copyright (C) John Olsen, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) John Olsen, 2000"
 */
// InterpDemoDoc.h : interface of the CInterpDemoDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTERPDEMODOC_H__04D7808A_FEA4_11D3_9A3F_004F49077001__INCLUDED_)
#define AFX_INTERPDEMODOC_H__04D7808A_FEA4_11D3_9A3F_004F49077001__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CInterpDemoDoc : public CDocument
{
protected: // create from serialization only
	CInterpDemoDoc();
	DECLARE_DYNCREATE(CInterpDemoDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInterpDemoDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInterpDemoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CInterpDemoDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INTERPDEMODOC_H__04D7808A_FEA4_11D3_9A3F_004F49077001__INCLUDED_)
