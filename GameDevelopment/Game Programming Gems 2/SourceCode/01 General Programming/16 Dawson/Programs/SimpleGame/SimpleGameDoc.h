/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
// SimpleGameDoc.h : interface of the CSimpleGameDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPLEGAMEDOC_H__4D30C55C_0A7A_11D5_BA9B_000000000000__INCLUDED_)
#define AFX_SIMPLEGAMEDOC_H__4D30C55C_0A7A_11D5_BA9B_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CSimpleGameDoc : public CDocument
{
protected: // create from serialization only
	CSimpleGameDoc();
	DECLARE_DYNCREATE(CSimpleGameDoc)

// Attributes
public:

private:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimpleGameDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSimpleGameDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSimpleGameDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPLEGAMEDOC_H__4D30C55C_0A7A_11D5_BA9B_000000000000__INCLUDED_)
