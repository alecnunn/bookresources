/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#include "stdafx.h"
#include "NVEffectsBrowser.h"

#include "Doc.h"
#include "ProgramView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
using namespace nv_objects;
/////////////////////////////////////////////////////////////////////////////
// CProgramView

IMPLEMENT_DYNCREATE(CProgramView, CListView)

BEGIN_MESSAGE_MAP(CProgramView, CListView)
	//{{AFX_MSG_MAP(CProgramView)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgramView construction/destruction

CProgramView::CProgramView()
: m_chColumn(NULL)
{
}

CProgramView::~CProgramView()
{
	delete [] m_chColumn;
}

BOOL CProgramView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= LVS_REPORT | LVS_OWNERDRAWFIXED | LVS_SINGLESEL;
	cs.style &= ~LVS_AUTOARRANGE;

	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CProgramView drawing

void CProgramView::OnDraw(CDC* pDC)
{
	CNVEffectsDoc* pDoc = (CNVEffectsDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	CListCtrl& refCtrl = GetListCtrl();
	refCtrl.InsertItem(0, "Item!");
}

void CProgramView::OnInitialUpdate()
{
	int i;
	CListView::OnInitialUpdate();
	CListCtrl& ListCtrl = GetListCtrl();

	// insert columns
	if (m_ViewType == VIEWTYPE_PIXEL)
	{
		m_vecColumns.push_back(ColumnDescriptor("Pixel Shader", LVCFMT_LEFT, 200));
	}
	else
	{
		m_vecColumns.push_back(ColumnDescriptor("Vertex Shader", LVCFMT_LEFT, 200));
	}
	
	// Restore the widths from the registry
	for (i = 0; i < m_vecColumns.size(); i++)
	{
		CString strReg;

		
		strReg.Format("ProgramView_Column%d", i);

		m_vecColumns[i].m_Width = theApp.GetProfileInt("Sizes", strReg, m_vecColumns[i].m_Width);
		CHECK_RANGE(m_vecColumns[i].m_Width, 0, -1);
	}

	for (i = 0; i < m_vecColumns.size(); i++)
	{
		// insert columns
		ListCtrl.InsertColumn(i, m_vecColumns[i].m_Name.c_str(), m_vecColumns[i].m_Format, m_vecColumns[i].m_Width, 0);
	}

	CRect ClientRect;
	GetClientRect(&ClientRect);

	OnSize(SIZE_RESTORED, ClientRect.Width(), ClientRect.Height());
}
/////////////////////////////////////////////////////////////////////////////
// CProgramView diagnostics

#ifdef _DEBUG
void CProgramView::AssertValid() const
{
	CListView::AssertValid();
}

void CProgramView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CProgramView message handlers
void CProgramView::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
}

bool CProgramView::SetEffect(EBEffect* pEffect)
{
	string strShaderFile;
	EBStringProperty* pShaderFile;
	CListCtrl& ListCtrl = GetListCtrl();
	std::string strFilePath;
	std::string strError;
	std::string strShader;
	int nItem = 0;
	CNVEffectsDoc* pDoc = (CNVEffectsDoc*)GetDocument();

	ListCtrl.DeleteAllItems();

	if (!pDoc->IsEffectValid())
	{
		// Only show the error in one of the windows
		if (m_ViewType == VIEWTYPE_VERTEX)
		{
			EBStringProperty* pErr = (EBStringProperty*)pEffect->GetProperty("LastError");
			if (pErr && pErr->IsKindOf(EBTYPE_STRING_PROP))
			{
				ListCtrl.InsertItem(0, pErr->ReadValue(pEffect).c_str());
			}
			else
			{
				ListCtrl.InsertItem(0, "Unknown error creating test!");
			}
		}
		return true;
	}

	if (m_ViewType == VIEWTYPE_VERTEX)
	{
		pShaderFile = (EBStringProperty*)pEffect->GetProperty("EffectVertexShader");
	}
	else
	{
		pShaderFile = (EBStringProperty*)pEffect->GetProperty("EffectPixelShader");
	}

	if (pShaderFile && pShaderFile->IsKindOf(EBTYPE_STRING_PROP))
	{
		HANDLE hFile;

		std::string strFilePath = pShaderFile->ReadValue(pEffect);
		if (strFilePath == "")
		{
			strError = "No ";
			strError += ((m_ViewType == VIEWTYPE_PIXEL) ? "Pixel Shader" : "Vertex Shader");
			ListCtrl.InsertItem(nItem++, strError.c_str());
			return true;
		}

		try
		{
			hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
			if(hFile == INVALID_HANDLE_VALUE)
			{
				strError = "Could not find file: " + strFilePath;
				ListCtrl.InsertItem(nItem++, strError.c_str());
				return false;
			}
	
			DWORD dwFileSize = GetFileSize(hFile, NULL);
			strShader.resize(dwFileSize);

			ReadFile(hFile, (void*)&strShader[0], dwFileSize, &dwFileSize, NULL);
			CloseHandle(hFile);
		}
		catch(...)
		{
			strError = "Could not load file: " + strFilePath;
			ListCtrl.InsertItem(nItem++, strError.c_str());
			return false;
		}

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
			ListCtrl.InsertItem(nItem++, strInsert.c_str());
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
				ListCtrl.InsertItem(nItem++, "");
				NewLines--;
			};

			StartPos = EndPos;
		}
		while(1);
	}
		
	return true;
}

bool CProgramView::ChangeTitle(const string& strTitle)
{
	char* pNewColumn = NULL;

	CListCtrl& ListCtrl = GetListCtrl();

	pNewColumn = new char[strTitle.size() + 1];
	if (!pNewColumn)
		return false;

	memcpy(pNewColumn, strTitle.c_str(), strTitle.size());
	pNewColumn[strTitle.size()] = 0;

	LVCOLUMN Column;

	ZeroMemory(&Column, sizeof(Column));
	Column.mask = LVCF_TEXT;
	Column.pszText = pNewColumn;
	ListCtrl.SetColumn(1, &Column);

	delete [] m_chColumn;
	m_chColumn = pNewColumn;

	return true;
}

void CProgramView::OnSize(UINT nType, int cx, int cy) 
{
	CListView::OnSize(nType, cx, cy);

	// Size the program to fit the window
	if (!m_vecColumns.empty())
	{
		int Width = 0;
		vector<ColumnDescriptor>::iterator itrColumns = m_vecColumns.begin();
		while(itrColumns != m_vecColumns.end())
		{
			Width += (*itrColumns).m_Width;	
			itrColumns++;
		};
		Width -= ((*(itrColumns - 1)).m_Width);

		CListCtrl& ListCtrl = GetListCtrl();
		ListCtrl.SetColumnWidth(m_vecColumns.size() - 1, cx - 2 - Width);
	}	
	
}

void CProgramView::OnDestroy() 
{
	CListView::OnDestroy();
	
	CListCtrl& ListCtrl = GetListCtrl();

	// Store the widths in the registry
	for (int i = 0; i < m_vecColumns.size(); i++)
	{
		CString strReg;
		strReg.Format("ProgramView_Column%d", i);

		int Width = ListCtrl.GetColumnWidth(i);
		theApp.WriteProfileInt("Sizes", strReg, Width);
	}	
}

