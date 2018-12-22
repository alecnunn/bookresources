// SphinxMMOSPlayerDoc.cpp : Implementierung der Klasse CSphinxMMOSPlayerDoc
//

#include "stdafx.h"
#include "SphinxMMOSPlayer.h"

#include "SphinxMMOSPlayerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PCREATE(pPatch, id) (IGA##id*)CreateProcessor(pPatch, CID_GA##id)
/////////////////////////////////////////////////////////////////////////////
// CSphinxMMOSPlayerDoc

IMPLEMENT_DYNCREATE(CSphinxMMOSPlayerDoc, CDocument)

BEGIN_MESSAGE_MAP(CSphinxMMOSPlayerDoc, CDocument)
	//{{AFX_MSG_MAP(CSphinxMMOSPlayerDoc)
	ON_COMMAND(ID_FILE_RELOAD, OnFileReload)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSphinxMMOSPlayerDoc Konstruktion/Destruktion

CSphinxMMOSPlayerDoc::CSphinxMMOSPlayerDoc()
{
	m_pfFaderSignal=NULL;
	m_pWaveTable = NULL;
	m_nSampleCount = 0;
}

CSphinxMMOSPlayerDoc::~CSphinxMMOSPlayerDoc()
{
	ATOM_FREE(m_pWaveTable);
	if(g_pEngine != NULL)
		g_pEngine->Terminate();
}

using namespace atomos;


/*
// create a new patch and append to the systems patches list
IGAPatch* CSphinxMMOSPlayerDoc::CreatePatch(ATOM_CHAR* pszName)
{
	IGAPatches* pPatches = g_pGASystem->GetPatches();
	IGAPatch* pPatch = ATOM_CREATE(GAPatch);
	pPatch->SetName(pszName);
	pPatches->Append(pPatch);

	return(pPatch);
}

// create a new processor and append to the patch
IGAProcessor* CSphinxMMOSPlayerDoc::CreateProcessor(IGAPatch* pPatch, UINT32 idClass)
{
	IGAProcessor* pProcessor = ATOM_CREATE(GAProcessor);
	if(pPatch != NULL) {
		IGAProcessors* pProcessors = pPatch->GetProcessors();
		pProcessors->Append(pProcessor);
	}

	return(pProcessor);
}
*/

/////////////////////////////////////////////////////////////////////////////
// CSphinxMMOSPlayerDoc Serialisierung

void CSphinxMMOSPlayerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSphinxMMOSPlayerDoc Diagnose

#ifdef _DEBUG
void CSphinxMMOSPlayerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSphinxMMOSPlayerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSphinxMMOSPlayerDoc Befehle


BOOL CSphinxMMOSPlayerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	
	if(g_pEngine != NULL && g_pGASystem != NULL) {
	// create a stream
		IGAStream* pStream = ATOM_CREATE(GAStream);
		IClass* pClass = (IClass*)pStream->GetClass();

	//	pStream->SetChannelCount(2); // stereo
		pStream->SetChannelCount(1); // mono
		pStream->Open(0);
		g_pGASystem->AppendStream(pStream);


		// read default patches
		// @todo read NULL should read default patch
		IGAObjectReader* pReader = ATOM_CREATE(GAObjectReader);
		
		if(pReader != NULL) {
			pReader->Read("combustion01.txt"); // ("test01.txt");  //
			UpdateAllViews(NULL, CMD_UPDATE_PATCHES, this);
			m_strCurrentFile = pReader->GetPathName();
			pReader->Terminate();
		}
	}

	return TRUE;
}


BOOL CSphinxMMOSPlayerDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	m_strCurrentFile = lpszPathName;

	if(g_pEngine != NULL && g_pGASystem != NULL) {
	
		// remove any previous loaded patches:
		g_pGASystem->GetPatches()->RemoveAll();

		// read patch file
		IGAObjectReader* pReader = ATOM_CREATE(GAObjectReader);
		pReader->Read(lpszPathName);
		UpdateAllViews(NULL, CMD_UPDATE_PATCHES, this);
		pReader->Terminate();
	}

	return TRUE;
}

BOOL CSphinxMMOSPlayerDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
	
	return CDocument::OnSaveDocument(lpszPathName);
}



void CSphinxMMOSPlayerDoc::OnFileReload() 
{
	OnOpenDocument(m_strCurrentFile) ;
}
