/* Copyright (C) John Olsen, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) John Olsen, 2000"
 */
// InterpDemoDoc.cpp : implementation of the CInterpDemoDoc class
//

#include "stdafx.h"
#include "InterpDemo.h"

#include "InterpDemoDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInterpDemoDoc

IMPLEMENT_DYNCREATE(CInterpDemoDoc, CDocument)

BEGIN_MESSAGE_MAP(CInterpDemoDoc, CDocument)
	//{{AFX_MSG_MAP(CInterpDemoDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInterpDemoDoc construction/destruction

CInterpDemoDoc::CInterpDemoDoc()
{
	// TODO: add one-time construction code here

}

CInterpDemoDoc::~CInterpDemoDoc()
{
}

BOOL CInterpDemoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CInterpDemoDoc serialization

void CInterpDemoDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CInterpDemoDoc diagnostics

#ifdef _DEBUG
void CInterpDemoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CInterpDemoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CInterpDemoDoc commands
