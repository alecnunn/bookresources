/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
// SimpleGameDoc.cpp : implementation of the CSimpleGameDoc class
//

#include "stdafx.h"
#include "SimpleGame.h"

#include "SimpleGameDoc.h"
#include "gamelibrary/objectfactories.h"
#include <vector>
using namespace std;

#include "gameworld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimpleGameDoc

IMPLEMENT_DYNCREATE(CSimpleGameDoc, CDocument)

BEGIN_MESSAGE_MAP(CSimpleGameDoc, CDocument)
	//{{AFX_MSG_MAP(CSimpleGameDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimpleGameDoc construction/destruction

CSimpleGameDoc::CSimpleGameDoc()
{
	// TODO: add one-time construction code here

}

CSimpleGameDoc::~CSimpleGameDoc()
{
}

BOOL CSimpleGameDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSimpleGameDoc serialization

void CSimpleGameDoc::Serialize(CArchive& ar)
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
// CSimpleGameDoc diagnostics

#ifdef _DEBUG
void CSimpleGameDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSimpleGameDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSimpleGameDoc commands

BOOL CSimpleGameDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// If pHandlerInfo is NULL, then handle the message
	if (pHandlerInfo == NULL)
	{
		if (nID >= k_firstDynamicID && nID < k_firstDynamicID + GetNumObjectTypes())
		{
			if (nCode == CN_COMMAND)
			{
				// It would be possible to support creation by integer ID, which could
				// be faster than creation by name, however I chose not to because
				// of the reduced flexibility. Under normal circumstances it is
				// the object name that you will have available anyway - not an
				// ID. This situation - creating objects from a menu - is an anomaly.
				int objectID = nID - k_firstDynamicID;
				vector<const char*> objectTypes = GetObjectTypes();
				GameObject* pObject = CreateGameObject(objectTypes[objectID]);
			}
			else if (nCode == CN_UPDATE_COMMAND_UI)
			{
				CCmdUI* pCmdUI = (CCmdUI*)pExtra;
				pCmdUI->Enable(TRUE);
			}
			return TRUE;
		}
	}

	// If we didn't process the command, call the base class
	// version of OnCmdMsg so the message-map can handle the message
	return CDocument::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
