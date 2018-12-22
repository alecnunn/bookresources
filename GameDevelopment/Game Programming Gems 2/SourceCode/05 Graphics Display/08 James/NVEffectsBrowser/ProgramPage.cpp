/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
// ProgramPage.cpp : implementation file
//

#include "stdafx.h"
#include "nveffectsbrowser.h"
#include "ProgramPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgramPage property page

using namespace std;
IMPLEMENT_DYNCREATE(CProgramPage, CPropertyPage)

CProgramPage::CProgramPage() 
: CPropertyPage(CProgramPage::IDD),
m_pEffect(NULL),
m_ColumnWidth(300)
{
	//{{AFX_DATA_INIT(CProgramPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

}

CProgramPage::~CProgramPage()
{
}

void CProgramPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgramPage)
	DDX_Control(pDX, IDC_PROGRAMLIST, m_ProgramList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProgramPage, CPropertyPage)
	//{{AFX_MSG_MAP(CProgramPage)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgramPage message handlers

void CProgramPage::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyPage::OnSize(nType, cx, cy);

	if (m_ProgramList.m_hWnd)
	{
		CRect rectList;
		CRect rectParent;
		CSize sizeRelChange;
		
		// First diff the list box with the parent page
		// Get forms client area
		GetWindowRect(&rectParent);
		ScreenToClient(&rectParent);
		m_ProgramList.GetClientRect(&rectList);
		sizeRelChange.cx = rectList.Width() - rectParent.Width();
		sizeRelChange.cy = rectList.Height() - rectParent.Height();

		rectList.right -= sizeRelChange.cx;
		rectList.bottom -= sizeRelChange.cy;
		m_ProgramList.MoveWindow(&rectList);
	}
}

// One-time setup
bool CProgramPage::Setup(CNVEffectsDoc* pDoc, VIEWTYPE ViewType, EBEffect* pEffect, EBEnumValue* pShader)
{
	m_strFilePath = "";
	m_strEffectName = "";
	m_bValid = true;

	assert(pDoc);
	assert(pEffect);

	m_pEffect = pEffect;
	m_ViewType = ViewType;

	m_vecListBoxStrings.clear();

	ZeroMemory(&m_TabItem, sizeof(m_TabItem));
	m_TabItem.mask = TCIF_TEXT;

	// Is the effect we are setting up valid?
	if (!pDoc->IsEffectValid())
	{
		// Only show the error in one of the windows
		if (ViewType == VIEWTYPE_VERTEX)
		{
			EBProperty* pErr = pEffect->GetProperty("LastError");
			if (pErr && pErr->IsKindOf(EBTYPE_STRING_PROP))
			{
				m_vecListBoxStrings.push_back((string)pErr->ReadValue(pEffect));
			}
			else
			{
				m_vecListBoxStrings.push_back("Unknown error creating test!");
			}
		}
		m_TabItem.pszText = "Error";

		m_bValid = false;

		return false;
	}
 
	// Get the name of the effect for the tab control
	if (pShader)
	{
		// The strings are embedded in the class, instead of local here, because we
		// pass a string pointer to the tab control, and we don't want
		// the contents of the string destructing while it is in use.

		m_strFilePath = pShader->GetValue();
		m_strEffectName = pShader->GetPropertyName();

		if (!m_strEffectName.empty())
		{
			m_TabItem.pszText = const_cast<char*>(m_strEffectName.c_str());
		}
		else
		{
			// Change the name on the tab control to indicate there is no shader here
			if (ViewType == VIEWTYPE_PIXEL)
			{
				m_TabItem.pszText = "No Pixel Shader";
			}
			else
			{
				m_TabItem.pszText = "No Vertex Shader";
			}
			m_bValid = false;
		}
	}
	else
	{
		if (ViewType == VIEWTYPE_PIXEL)
		{
			m_TabItem.pszText = "No Pixel Shader";
		}
		else
		{
			m_TabItem.pszText = "No Vertex Shader";
		}

		m_bValid = false;
	}


	if (m_bValid)
	{
		HANDLE hFile;
		string strError;
		string strShader;
		try
		{
			hFile = CreateFile(m_strFilePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
			if(hFile == INVALID_HANDLE_VALUE)
			{
				strError = "Could not find file: " + m_strFilePath;
				m_vecListBoxStrings.push_back(strError);
				m_bValid = false;
			}

			DWORD dwFileSize = GetFileSize(hFile, NULL);
			strShader.resize(dwFileSize);

			ReadFile(hFile, (void*)&strShader[0], dwFileSize, &dwFileSize, NULL);
			CloseHandle(hFile);
		}
		catch(...)
		{
			strError = "Could not load file: " + m_strFilePath;
			m_vecListBoxStrings.push_back(strError);
			m_bValid = false;
		}
		
		if (m_bValid)
		{
			string::size_type StartPos = 0;
			string::size_type EndPos;

			// Put the text in the box
			do
			{
				// Find each line and add it to the listbox
				int NewLines;
				EndPos = strShader.find_first_of("\r\n", StartPos);
				
				string strInsert = strShader.substr(StartPos, EndPos - StartPos);
				strInsert += '\0';

				string::size_type TempPos;

				// Replace tabs with spaces
				do
				{
					int nReplaced = 0;
					for (TempPos = 0; TempPos < strInsert.size(); TempPos++)
					{
						if (strInsert[TempPos] == '\t')
						{
							string strTemp = "    ";
							strInsert.replace(TempPos, 1, strTemp, 0, strTemp.size());
							continue;
						}
					}
					break;
				} while (1);


				// Do the add
				m_vecListBoxStrings.push_back(strInsert);
				if (EndPos == strShader.npos)
					break;

				NewLines = -1;

				// jump past the new lines and returns
				// Adding blank lines to the listbox if necessary
				while(strShader[EndPos] == '\n' || strShader[EndPos] == '\r')
				{
					if (strShader[EndPos] == '\n')
						NewLines++;

					EndPos++;
				};

				while (NewLines > 0)
				{
					m_vecListBoxStrings.push_back("");
					NewLines--;
				};

				StartPos = EndPos;
			}
			while(1);
		}
	}

	return m_bValid;
}

void CProgramPage::ClearItems()
{
	if (m_ProgramList.m_hWnd != NULL)
	{
		m_ProgramList.DeleteColumn(0);
		m_ProgramList.DeleteAllItems();
	}
}

bool CProgramPage::SetEffect()
{
	DWORD i;

	CTabCtrl* pTabControl = ((CPropertySheet*)GetParent())->GetTabControl();

	// Remove anything that is there
	if (m_ProgramList.m_hWnd != NULL)
	{
		m_ProgramList.DeleteAllItems();
		m_ProgramList.DeleteColumn(0);
	}

	// Remove the column header if we aren't valid
	if (m_bValid)
	{
		m_ProgramList.ModifyStyle(LVS_LIST, LVS_REPORT, 0);

		CRect rcInnerList;
		m_ProgramList.GetClientRect(&rcInnerList);
		m_ColumnWidth = rcInnerList.Width() - GetSystemMetrics(SM_CXVSCROLL);
		if (m_ColumnWidth < 20)
			m_ColumnWidth = 20;
		
		// Add the columns
		switch(m_ViewType)
		{
			case VIEWTYPE_VERTEX:
				m_ProgramList.InsertColumn(0, "Vertex Shader", LVCFMT_LEFT, m_ColumnWidth);
				break;
			case VIEWTYPE_PIXEL:
				m_ProgramList.InsertColumn(0, "Pixel Shader", LVCFMT_LEFT, m_ColumnWidth);
				break;
			default:
				assert(0);
				break;
		}
	}
	else
	{
		m_ProgramList.ModifyStyle(LVS_REPORT, LVS_LIST, 0);
	}

	// Add the items to the listbox
	for (i = 0; i < m_vecListBoxStrings.size(); i++)
	{
		m_ProgramList.InsertItem(i, m_vecListBoxStrings[i].c_str());
	}

	pTabControl->SetItem(pTabControl->GetCurSel(), &m_TabItem);
	return true;
}


BOOL CProgramPage::OnSetActive() 
{
	if (m_pEffect && m_ProgramList.m_hWnd)
		SetEffect();

	return CPropertyPage::OnSetActive();
}
