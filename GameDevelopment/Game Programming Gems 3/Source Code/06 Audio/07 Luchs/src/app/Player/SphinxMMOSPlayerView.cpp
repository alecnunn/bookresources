// SphinxMMOSPlayerView.cpp : Implementierung der Klasse CSphinxMMOSPlayerView
//

#include "stdafx.h"
#include "SphinxMMOSPlayer.h"

#include "SphinxMMOSPlayerDoc.h"
#include "SphinxMMOSPlayerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSphinxMMOSPlayerView

IMPLEMENT_DYNCREATE(CSphinxMMOSPlayerView, CFormView)

BEGIN_MESSAGE_MAP(CSphinxMMOSPlayerView, CFormView)
	//{{AFX_MSG_MAP(CSphinxMMOSPlayerView)
	ON_COMMAND(ID_ENGINE_START, OnEngineStart)
	ON_UPDATE_COMMAND_UI(ID_ENGINE_START, OnUpdateEngineStart)
	ON_COMMAND(ID_ENGINE_STOP, OnEngineStop)
	ON_UPDATE_COMMAND_UI(ID_ENGINE_STOP, OnUpdateEngineStop)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PATCHES, OnSelchangedTreePatches)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_BTN_TRIGGER, OnBtnTrigger)
	ON_COMMAND(ID_ENGINE_RECORD, OnEngineRecord)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSphinxMMOSPlayerView Konstruktion/Destruktion

CSphinxMMOSPlayerView::CSphinxMMOSPlayerView()
	: CFormView(CSphinxMMOSPlayerView::IDD)
{
	//{{AFX_DATA_INIT(CSphinxMMOSPlayerView)
		// HINWEIS: Der Klassenassistent fügt hier Member-Initialisierung ein
	//}}AFX_DATA_INIT
}

CSphinxMMOSPlayerView::~CSphinxMMOSPlayerView()
{
}

void CSphinxMMOSPlayerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSphinxMMOSPlayerView)
	DDX_Control(pDX, IDC_STATIC1, m_static1);
	DDX_Control(pDX, IDC_SLIDER1, m_slider1);
	DDX_Control(pDX, IDC_EDIT1, m_edit1);
	DDX_Control(pDX, IDC_STATIC_PATCH, m_staticPatch);
	DDX_Control(pDX, IDC_TREE_PATCHES, m_treePatches);
	//}}AFX_DATA_MAP
}

BOOL CSphinxMMOSPlayerView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFormView::PreCreateWindow(cs);
}

void CSphinxMMOSPlayerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	if(m_Images.m_hImageList == NULL) {
		m_Images.Create(IDR_TB_OBJECTS, 16,10, RGB(255,0,255));
		m_treePatches.SetImageList(&m_Images, TVSIL_NORMAL);
	}

	m_fFaderValue = 0.0f;
	m_slider1.SetRange(-100,+100);
	m_slider1.SetPos(0);


	UpdatePatchList() ;

}

/////////////////////////////////////////////////////////////////////////////
// CSphinxMMOSPlayerView Diagnose

#ifdef _DEBUG
void CSphinxMMOSPlayerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSphinxMMOSPlayerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CSphinxMMOSPlayerDoc* CSphinxMMOSPlayerView::GetDocument() 
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSphinxMMOSPlayerDoc)));
	return (CSphinxMMOSPlayerDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSphinxMMOSPlayerView Nachrichten-Handler

void CSphinxMMOSPlayerView::OnEngineStart() 
{
	if(g_pGASystem != NULL)
		g_pGASystem->Start();
	
}

void CSphinxMMOSPlayerView::OnUpdateEngineStart(CCmdUI* pCmdUI) 
{
	
}

void CSphinxMMOSPlayerView::OnEngineStop() 
{
	if(g_pGASystem != NULL)
		g_pGASystem->Stop();
	
}

void CSphinxMMOSPlayerView::OnUpdateEngineStop(CCmdUI* pCmdUI) 
{
	
}

void CSphinxMMOSPlayerView::OnEngineRecord() 
{
	if(g_pGASystem != NULL)
		g_pGASystem->Record();
}


void CSphinxMMOSPlayerView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{

	if(lHint == CMD_UPDATE_PATCHES) {
		UpdatePatchList();

		if(m_static1.m_hWnd != NULL)
			m_static1.SetWindowText(_T(""));
		if(m_staticPatch.m_hWnd != NULL)
			m_staticPatch.SetWindowText(_T("<no patch selected>"));
	}
}

using namespace atomos;




// update the current object
void CSphinxMMOSPlayerView::OnSelchangedTreePatches(NMHDR* pNMHDR, LRESULT* pResult) 
{

	if(g_pGASystem != NULL) {

		NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
		IGAPatches* pPatches = g_pGASystem->GetPatches();

		HTREEITEM hItem = pNMTreeView->itemNew.hItem;
		IGAObject* pob = (IGAObject*)m_treePatches.GetItemData(hItem);
		
		if(pob != NULL) {
			
			ATOM_CHAR* pszName = pob->GetName();
			IClass* pClass = (IClass*)pob->GetClass();

			if(pClass->IsA(CGAPatch::CID)) {

				if(pszName != NULL)
					m_staticPatch.SetWindowText(pszName);
				pPatches->SetCurrent((IGAPatch*)pob);
			}
			else if(pClass->IsA(CGAPin::CID)) {

				if(pszName != NULL)
					m_static1.SetWindowText(pszName);
			}
		
		}
	}
	*pResult = 0;
}




void CSphinxMMOSPlayerView::UpdatePatchList() 
{

	if(m_treePatches.m_hWnd == NULL || g_pGASystem == NULL)
		return;

	m_static1.SetWindowText("");

	IGAPatches* pPatches = g_pGASystem->GetPatches();

	UINT32 nPatchCount = pPatches->GetCount(); // count of loaded patches
	
	m_treePatches.DeleteAllItems();

	HTREEITEM hPatches = m_treePatches.InsertItem("Patches", 0,0);
	m_treePatches.SetItemData(hPatches, (DWORD) pPatches);

	if(nPatchCount==0) {

			HTREEITEM hPatch = m_treePatches.InsertItem(_T("Please load a patch file"), 1,1, hPatches);
		

		
	}
	else {

		for(int i=0;i < nPatchCount; i++) {

			IGAPatch* pPatch = (IGAPatch*)pPatches->GetAt(i);
			ATOM_CHAR* pszName = pPatch->GetName();
			HTREEITEM hPatch = m_treePatches.InsertItem(pszName, 1,1, hPatches);
			m_treePatches.SetItemData(hPatch, (DWORD) pPatch);

			IGAProcessors* pProcessors = pPatch->GetProcessors();
			UINT32 nProcessorCount = pProcessors->GetCount();
			for(int ip=0;ip < nProcessorCount; ip++) {

				IGAProcessor* pProcessor = pProcessors->GetAt(ip);
				ATOM_CHAR* pszName = pProcessor->GetName();
				HTREEITEM hProcessor = m_treePatches.InsertItem(pszName, 2,2, hPatch);
				m_treePatches.SetItemData(hProcessor, (DWORD) pProcessor);


				IGAPins* pPins = pProcessor->GetPins();
				UINT32 nPinCount = pPins->GetCount();

				for(int iPin=0;iPin < nPinCount; iPin++) {

					IGAPin* pPin = pPins->GetAt(iPin);
					ATOM_CHAR* pszName = pPin->GetName();
					HTREEITEM hPin = m_treePatches.InsertItem(pszName, 3,3, hProcessor);
					m_treePatches.SetItemData(hPin, (DWORD) pPin);

				}

			}
		}

	} // if patches available

	m_treePatches.Expand(hPatches, TVE_EXPAND);

}



void CSphinxMMOSPlayerView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if((CSliderCtrl*)pScrollBar == &m_slider1) {

		CString str;
		m_fFaderValue =  (FLOAT32)(m_slider1.GetPos()  / -100.0f);
		GetDocument()->m_pfFaderSignal = &m_fFaderValue;

		str.Format("%.3f", m_fFaderValue);
		m_edit1.SetWindowText(str);

	}
	
	CFormView::OnVScroll(nSBCode, nPos, pScrollBar);
}



BOOL CSphinxMMOSPlayerView::PreTranslateMessage(MSG* pMsg) 
{
	if(g_pGASystem != NULL) {

		IGAPatches* pPatches = g_pGASystem->GetPatches();
		UINT32 nCount = pPatches->GetCount();

		if(nCount > 0) {

			IGAPatch* pPatch = pPatches->GetCurrent();
			IGAProcessors* pProcessors = NULL;
			IGAProcessor* pProcessor = NULL;

			if(pPatch != NULL)
				pProcessors = pPatch->GetProcessors();


			if(pProcessors != NULL)
				pProcessor = pProcessors->GetAt(0);


			// handle keyboard, the keyboard object is always available
			// --------------------------------------------------------------
			IGAKeyboard* pKeyboard = g_pGASystem->GetKeyboard(); 

			int nMsg = pMsg->message;
			int w = pMsg->wParam;

			if( nMsg == WM_KEYDOWN )
			{
				pKeyboard->SetKey(w,1);
				if( w >= 32 && w < 127) 
					return(1);// no further handling for these keys
			}

			else if( nMsg == WM_KEYUP )
			{
				pKeyboard->SetKey(w,0);
				if( w >= 32 && w < 127)
					return(1);// no further handling for these keys
			}
			// --------------------------------------------------------------


			if(pMsg->message == WM_CHAR) {

				int nChar = pMsg->wParam;
				g_pGASystem->GetKeyboard()->SetKey(nChar,1);

				if(nChar ==  VK_SPACE) {

					if(pProcessors != NULL)
						pProcessors->Trigger();	// trigger all processors
				}
				else if(nChar ==  49) {

					if(pProcessor != NULL)
						pProcessor->SetParameter("Frequency", "200.0f");

				}
				else if(nChar ==  50) {

					if(pProcessor != NULL)
						pProcessor->SetParameter("Frequency", "400.0f");

				}
			}
		} // if patches
	} // if gasystem
	return CFormView::PreTranslateMessage(pMsg);
}

void CSphinxMMOSPlayerView::OnBtnTrigger() 
{
	if(g_pGASystem != NULL) {
		IGAPatches* pPatches = g_pGASystem->GetPatches();
		if(pPatches != NULL) {
			IGAPatch* pPatch = pPatches->GetCurrent();

			if(pPatch != NULL) {
				pPatch->Trigger();
			}
		}
	}
}


