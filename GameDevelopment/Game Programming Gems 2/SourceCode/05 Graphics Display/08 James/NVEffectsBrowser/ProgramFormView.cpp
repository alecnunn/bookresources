/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
// ProgramFormView.cpp : implementation file
//

#include "stdafx.h"
#include "nveffectsbrowser.h"
#include "ProgramFormView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
/////////////////////////////////////////////////////////////////////////////
// CProgramFormView

IMPLEMENT_DYNCREATE(CProgramFormView, CFormView)

CProgramFormView::CProgramFormView()
	: CFormView(CProgramFormView::IDD),
	m_pProgramSheet(NULL)
{
	//{{AFX_DATA_INIT(CProgramFormView)
	//}}AFX_DATA_INIT
}

CProgramFormView::~CProgramFormView()
{
}

void CProgramFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgramFormView)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgramFormView, CFormView)
	//{{AFX_MSG_MAP(CProgramFormView)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgramFormView diagnostics

#ifdef _DEBUG
void CProgramFormView::AssertValid() const
{
	CFormView::AssertValid();
}

void CProgramFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CProgramFormView message handlers


bool CProgramFormView::SetEffect(EBEffect* pEffect)
{
	CNVEffectsDoc* pDoc = (CNVEffectsDoc*)GetDocument();

	if (!m_pProgramSheet)
		return false;


	m_pProgramSheet->SetEffect(pDoc, m_ViewType, pEffect);

	return true;
}

void CProgramFormView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);

	if (m_hWnd && m_pProgramSheet)
		m_pProgramSheet->Resize();
	
}

void CProgramFormView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

	// create and asociated the property sheet with the "place holder" window
	CWnd* pwndPropSheetHolder = GetDlgItem(IDC_PROPSHEET_PLACEHOLDER);
	m_pProgramSheet = new CProgramSheet(pwndPropSheetHolder);

	if (!m_pProgramSheet->Create(pwndPropSheetHolder, WS_CHILD | WS_VISIBLE, 0))
	{
		delete m_pProgramSheet;
		m_pProgramSheet = NULL;
		return;
	}
	
	// fit the property sheet into the place holder window, and show it
	CRect rectPropSheet;
	pwndPropSheetHolder->GetWindowRect(rectPropSheet);
	m_pProgramSheet->SetWindowPos(NULL, 0, 0, rectPropSheet.Width(), rectPropSheet.Height(), SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE);

}

void CProgramFormView::OnDestroy() 
{
	m_pProgramSheet->DestroyWindow();
	delete m_pProgramSheet;

	CFormView::OnDestroy();
	
	
}

BOOL CProgramFormView::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	return CFormView::PreTranslateMessage(pMsg);
}
