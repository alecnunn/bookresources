/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
// ProgramSheet.cpp : implementation file
//

#include "stdafx.h"
#include "nveffectsbrowser.h"
#include "ProgramSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
/////////////////////////////////////////////////////////////////////////////
// CProgramSheet

IMPLEMENT_DYNAMIC(CProgramSheet, CPropertySheet)

CProgramSheet::CProgramSheet(CWnd* pParentWnd)
	:CPropertySheet(AFX_IDS_APP_TITLE, pParentWnd),
	m_pFormView((CProgramFormView*)pParentWnd->GetParent()),
	m_pEffect(NULL)
{
	m_pDummyPage = new CProgramPage();
	AddPage(m_pDummyPage);

}

CProgramSheet::~CProgramSheet()
{
	// Remove all the pages
	vector<CProgramPage*>::iterator itrProgramPage = m_vecProgramPages.begin();
	while (itrProgramPage != m_vecProgramPages.end())
	{
		RemovePage(*itrProgramPage);
		delete (*itrProgramPage);
		itrProgramPage++;
	}
	m_vecProgramPages.clear();

	RemovePage(m_pDummyPage);
	delete m_pDummyPage;

}

BEGIN_MESSAGE_MAP(CProgramSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CProgramSheet)
	ON_WM_SIZE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_RESIZEPAGE, OnResizePage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgramSheet message handlers

void CProgramSheet::Resize()
{
// Find parent
	CWnd* pParent;
	CRect rectParent;
	CRect rectForm;

	CSize sizeRelChange;
	CRect rectWindow;

	// Get the parent 
	pParent = GetParent();
	ASSERT(pParent);
	ASSERT(m_pFormView);

	// First diff the dummy control with the parent formview
	// Get forms client area
	m_pFormView->GetClientRect(&rectParent);
	pParent->GetWindowRect(&rectWindow);
	pParent->ScreenToClient(&rectWindow);
	sizeRelChange.cx = rectWindow.Width() - rectParent.Width();
	sizeRelChange.cy = rectWindow.Height() - rectParent.Height();

	rectWindow.right -= sizeRelChange.cx;
	rectWindow.bottom -= sizeRelChange.cy;
	pParent->MoveWindow(&rectWindow);

	
	// Now get the diff between the dummy control and the property sheet
	pParent->GetClientRect(&rectParent);

	// Resize the sheet
	// First find relative change
	GetWindowRect(&rectWindow);
	ScreenToClient(&rectWindow);
	sizeRelChange.cx = rectWindow.Width() - rectParent.Width();
	sizeRelChange.cy = rectWindow.Height() - rectParent.Height();

	rectWindow.right -= sizeRelChange.cx;
	rectWindow.bottom -= sizeRelChange.cy;
	GetParent()->MoveWindow(&rectWindow);

	// Then resize the sheet
	MoveWindow(&rectWindow);

	// Resize the CTabCtrl
	CTabCtrl* pTab = GetTabControl();
	ASSERT(pTab);
	pTab->GetWindowRect(&rectWindow);
	ScreenToClient(&rectWindow);
	rectWindow.right -= sizeRelChange.cx;
	rectWindow.bottom -= sizeRelChange.cy;
	pTab->MoveWindow(&rectWindow);

	// Resize the active page
	CPropertyPage* pPage = GetActivePage();
	ASSERT(pPage);
	// Store page size in m_rectPage
	pPage->GetWindowRect(&m_rectPage);
	ScreenToClient(&m_rectPage);
	m_rectPage.right -= sizeRelChange.cx;
	m_rectPage.bottom -= sizeRelChange.cy;
	pPage->MoveWindow(&m_rectPage);

}

bool CProgramSheet::SetEffect(CNVEffectsDoc* pDoc, VIEWTYPE ViewType, EBEffect* pEffect)
{
	m_ViewType = ViewType;
	m_pEffect = pEffect;

	string strShaderFile;
	CTabCtrl* pTabControl = GetTabControl();
	EBEnumProperty* pShaderFileEnum;
	EBProperty* pShaderFileProp;
	CProgramPage* pActivePage = m_pDummyPage;

	// Remove all the pages
	vector<CProgramPage*>::iterator itrProgramPage = m_vecProgramPages.begin();
	while (itrProgramPage != m_vecProgramPages.end())
	{
		RemovePage(*itrProgramPage);
		delete(*itrProgramPage);
		itrProgramPage++;
	}
	m_vecProgramPages.clear();
	
	
	// Find the relevent shader file details
	if (m_ViewType == VIEWTYPE_VERTEX)
	{
		pShaderFileProp = (pEffect->GetProperty("EffectVertexShader"));
	}
	else
	{
		pShaderFileProp = (pEffect->GetProperty("EffectPixelShader"));
	}

	if (!pShaderFileProp)
	{
		m_pDummyPage->Setup(pDoc, m_ViewType, pEffect, NULL);
		SetActivePage(m_pDummyPage);
		return false;
	}

	// An old-style shader
	if (pShaderFileProp->IsKindOf(EBTYPE_STRING_PROP))
	{
		m_pDummyPage->Setup(pDoc, m_ViewType, pEffect, NULL);
		SetActivePage(m_pDummyPage);

		return false;
	}
	else if (pShaderFileProp->IsKindOf(EBTYPE_ENUM_PROP))
	{
		// Walk the list of shaders.
		std::string strFilePath = "";

		pShaderFileEnum = static_cast<EBEnumProperty*>(pShaderFileProp);

		t_listEnum listEnumerants = pShaderFileEnum->GetEnumerants();
		t_listEnum::const_iterator itrEnum = listEnumerants.begin();
		DWORD Page = 0;
		bool bFirstPage = true;

		// If there are no enumerants, return
		if (listEnumerants.empty())
		{
			m_pDummyPage->Setup(pDoc, m_ViewType, pEffect, NULL);
			SetActivePage(m_pDummyPage);
			return true;
		}

		while (itrEnum != listEnumerants.end())
		{
			assert(*itrEnum);

			CProgramPage* pPage = NULL;

			if (bFirstPage)
			{
				pPage = m_pDummyPage;
			}
			else
			{
				pPage = new CProgramPage;
				// Add the page for this shader
				AddPage(pPage);

				// Keep a record of it
				m_vecProgramPages.push_back(pPage);
			}

			TCITEM TabItem;
			TabItem.mask = TCIF_TEXT;
			EBString strName = (*itrEnum)->GetPropertyName().c_str();
			TabItem.pszText = const_cast<char*>(strName.c_str());
			pTabControl->SetItem(Page, &TabItem);

			// Setup the page so it can display
			pPage->Setup(pDoc, m_ViewType, pEffect, (*itrEnum));

			// Is this the current page?
			if (pShaderFileEnum->ReadValue(pEffect) == (*itrEnum)->GetValue())
			{
				pActivePage = pPage;
			}
		
			itrEnum++;
			Page++;
			bFirstPage = false;

			// Don't add any more if the effect is broken
			if (!pDoc->IsEffectValid())
				break;
		}
	}
	else
	{
		assert(0);
	}

	SetActivePage(pActivePage);

	return true;
}

BOOL CProgramSheet::OnInitDialog() 
{
	ModifyStyleEx (0, WS_EX_CONTROLPARENT);

	BOOL bRet = CPropertySheet::OnInitDialog();

	Resize();

	return bRet;
}

LONG CProgramSheet::OnResizePage(UINT, LONG)
{
	// Resize the page using m_rectPage, which was set in
	// Resize().
	CPropertyPage* pPage = GetActivePage();
	ASSERT(pPage);
	pPage->MoveWindow(&m_rectPage);

	return 0;
}


BOOL CProgramSheet::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	NMHDR* pNMHDR = (LPNMHDR) lParam;

	// Somebody selected a different tab
	if (pNMHDR->code == TCN_SELCHANGE)
	{
		int iSelection = GetTabControl()->GetCurSel();
		if (m_pEffect && (iSelection != -1))
		{
			EBEnumProperty* pShaderFileEnum;
			EBProperty* pShaderFileProp;

			if (m_ViewType == VIEWTYPE_VERTEX)
			{
				pShaderFileProp = m_pEffect->GetProperty("EffectVertexShader");
			}
			else
			{
				pShaderFileProp = m_pEffect->GetProperty("EffectPixelShader");
			}

			if (pShaderFileProp && pShaderFileProp->IsKindOf(EBTYPE_ENUM_PROP))
			{
				pShaderFileEnum = static_cast<EBEnumProperty*>(pShaderFileProp);

				t_listEnum listEnumerants = pShaderFileEnum->GetEnumerants();
				t_listEnum::const_iterator itrEnum = listEnumerants.begin();

				int iPage = 0;
				while (itrEnum != listEnumerants.end())
				{
					assert(*itrEnum);

					if (iPage == iSelection)
					{
						pShaderFileEnum->WriteValue(m_pEffect, (*itrEnum)->GetValue());
					}

					iPage++;
					itrEnum++;
				}

			}
		}
	}

	
	return CPropertySheet::OnNotify(wParam, lParam, pResult);
}

void CProgramSheet::OnSize(UINT nType, int cx, int cy) 
{
	CPropertySheet::OnSize(nType, cx, cy);
	
}

int CProgramSheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	EnableStackedTabs(false);

	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	
	
	return 0;
}
