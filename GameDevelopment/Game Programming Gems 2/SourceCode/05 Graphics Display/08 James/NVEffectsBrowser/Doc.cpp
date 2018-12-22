/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
// Doc.cpp : implementation of the CNVEffectsDoc class
//

#include "stdafx.h"
#include "NVEffectsBrowser.h"

#include "Doc.h"

using namespace nv_objects;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNVEffectsDoc

IMPLEMENT_DYNCREATE(CNVEffectsDoc, CDocument)

BEGIN_MESSAGE_MAP(CNVEffectsDoc, CDocument)
	//{{AFX_MSG_MAP(CNVEffectsDoc)
	ON_COMMAND(IDM_CAPTURE_BEGINMOVIE, OnCaptureBeginmovie)
	ON_UPDATE_COMMAND_UI(IDM_CAPTURE_BEGINMOVIE, OnUpdateCaptureBeginmovie)
	ON_COMMAND(IDM_CAPTURE_ENDMOVIE, OnCaptureEndmovie)
	ON_UPDATE_COMMAND_UI(IDM_CAPTURE_ENDMOVIE, OnUpdateCaptureEndmovie)
	ON_COMMAND(IDM_CAPTURE_SAVEIMAGE, OnCaptureSaveimage)
	ON_UPDATE_COMMAND_UI(IDM_CAPTURE_SAVEIMAGE, OnUpdateCaptureSaveimage)
	ON_COMMAND(IDM_DEVICE_FORCESOFTWAREVP, OnDeviceForcesoftwarevp)
	ON_UPDATE_COMMAND_UI(IDM_DEVICE_FORCESOFTWAREVP, OnUpdateDeviceForcesoftwarevp)
	ON_COMMAND(IDM_DEVICE_REFERENCE, OnDeviceReference)
	ON_UPDATE_COMMAND_UI(IDM_DEVICE_REFERENCE, OnUpdateDeviceReference)
	ON_COMMAND(ID_FILE_HARDWAREDEVICE, OnFileHardwaredevice)
	ON_UPDATE_COMMAND_UI(ID_FILE_HARDWAREDEVICE, OnUpdateFileHardwaredevice)
	ON_COMMAND(IDM_OPTIONS_EXPANDALL, OnOptionsExpandall)
	ON_UPDATE_COMMAND_UI(IDM_OPTIONS_EXPANDALL, OnUpdateOptionsExpandall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNVEffectsDoc construction/destruction

CNVEffectsDoc::CNVEffectsDoc()
: m_pMovie(NULL),
	m_pTimer(NULL),
	m_pRenderView(NULL),
	m_pTreeView(NULL),
	m_bEffectMenu(false),
	m_bValidEffect(false),
	m_bStarted(false),
	m_pCurrentEffect(NULL),
	m_bMovieRecording(false)
{
	m_pProgramFormView[0] = NULL;
	m_pProgramFormView[1] = NULL;
}

CNVEffectsDoc::~CNVEffectsDoc()
{
}

bool CNVEffectsDoc::FileFoundCallback(const WIN32_FIND_DATA& FindData, const std::string& strDirectory)
{
	EffectFileData EffectData;

	memcpy(&EffectData.m_FindData, &FindData, sizeof(WIN32_FIND_DATA));
	EffectData.m_strDirectory = strDirectory;
	EffectData.hLib = NULL;

	tmapEffectFileData::iterator itrFound;
	
	itrFound = m_EffectFileData.find(FindData.cFileName);
	if (itrFound != m_EffectFileData.end())
	{
		DISPDBG(0, "Found alternate version of " << FindData.cFileName << " in :" << strDirectory);
		ULARGE_INTEGER LastWriteNew = *(ULARGE_INTEGER*)(&EffectData.m_FindData.ftLastWriteTime);
		ULARGE_INTEGER LastWriteOld = *(ULARGE_INTEGER*)(&(*itrFound).second.m_FindData.ftLastWriteTime);

		if (LastWriteNew.QuadPart > LastWriteOld.QuadPart)
		{
			DISPDBG(0, "Keeping newer dll in :" << strDirectory);
			(*itrFound).second = EffectData;
		}
		else
		{
			DISPDBG(0, "Keeping older dll in :" << (*itrFound).second.m_strDirectory);
		}
	}
	else
	{
		DISPDBG(0, "Found new dll, " << FindData.cFileName << " ,in :" << strDirectory);
		m_EffectFileData.insert(make_pair<std::string, EffectFileData>(FindData.cFileName, EffectData));
	}


	return true;
}

bool CEffectSearch::FileFoundCallback(const WIN32_FIND_DATA& FindData, const std::string& strDirectory)
{
	if (m_pDoc)
		return m_pDoc->FileFoundCallback(FindData, strDirectory);
	
	return false;
} 

BOOL CNVEffectsDoc::OnNewDocument()
{

	if (!CDocument::OnNewDocument())
		return FALSE;

	// Update the message string
	if (m_pRenderView)
		m_pRenderView->SetMsg("Loading effects...");

	// Find and register the dll's
	HMODULE hProcess = GetModuleHandle(NULL);
	string strProcessPath;
	strProcessPath.resize(MAX_PATH);
	DWORD dwReturnSize = GetModuleFileName((HINSTANCE)hProcess, &strProcessPath[0], MAX_PATH);
	assert(dwReturnSize <= MAX_PATH); // Should never happen
	string::size_type Pos = strProcessPath.find_last_of("\\", strProcessPath.size());
	if (Pos != strProcessPath.npos)
	{
		strProcessPath = strProcessPath.substr(0, Pos);
	}
	else
	{
		strProcessPath = ".\\";
	}

	CEffectSearch EffectSearch;
	EffectSearch.m_pDoc = this;
	EffectSearch.FindFile("*.dll", strProcessPath, true);

	// Walk the list of loaded .dll's and create them.
	tmapEffectFileData::iterator itrEffects;
	itrEffects = m_EffectFileData.begin();
	while (itrEffects != m_EffectFileData.end())
	{
		(*itrEffects).second.hLib = NULL;

		string strLoad = (*itrEffects).second.m_strDirectory + "\\" + (*itrEffects).second.m_FindData.cFileName; 
		HINSTANCE hLib = LoadLibrary(strLoad.c_str());
		if (hLib != NULL)
		{
			LPCREATEEFFECT pCreate = (LPCREATEEFFECT)GetProcAddress(hLib, "CreateEffect");
			LPGETEFFECTNUM pEffectNum = (LPGETEFFECTNUM)GetProcAddress(hLib, "GetNumEffects");

			if (pEffectNum && pCreate)
			{
				// Store the loaded library
				(*itrEffects).second.hLib = hLib;

				unsigned int EffectNum = pEffectNum();
				for (unsigned int i = 0; i < EffectNum; i++)
				{
					EBEffect* pEffect;
					pEffect = pCreate(i);
					if (pEffect)
					{
					    ostringstream strError;

						// Update the properties so that we have basic configuration support
						pEffect->UpdateProperties();

						EBProperty* pVerProp = pEffect->GetProperty("EffectVersion");
						if (pVerProp && pVerProp->IsKindOf(EBTYPE_DWORD_PROP))
						{
							DWORD dwVer = pVerProp->ReadValue(pEffect);
							if (dwVer <= theApp.GetVersion())
							{
								// Quick hack to try to start on the simplest shader so we don't get annoyed
								// by a shader that can't run on the current device.
								if (strcmp((*itrEffects).second.m_FindData.cFileName, "quad.dll") == 0)
									m_listEffects.push_front(pEffect);
								else
									m_listEffects.push_back(pEffect);
							}
							else
							{
								pEffect->DestroyEffect();
								pEffect = NULL;
								strError << "Can't load effect " << (*itrEffects).second.m_FindData.cFileName << " , Unsupported Version: " << dwVer;
								MessageBox(NULL, strError.str().c_str(), "Warning", MB_ICONEXCLAMATION | MB_OK);
							}
						}
						else
						{
							pEffect->DestroyEffect();
							pEffect = NULL;
							strError << "Can't find effect version in " << (*itrEffects).second.m_FindData.cFileName;
							MessageBox(NULL, strError.str().c_str(), "Error", MB_ICONEXCLAMATION | MB_OK);
						}
					}	
				}
			}
			else
			{
				FreeLibrary(hLib);
			}
		}
		itrEffects++;
	}
			
	m_pTimer = new EBTimer();

	m_pMovie = new NVMovie();

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CNVEffectsDoc serialization

void CNVEffectsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNVEffectsDoc diagnostics

#ifdef _DEBUG
void CNVEffectsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CNVEffectsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void CNVEffectsDoc::OnIdle()
{
	if (m_pRenderView)
		GetRenderWnd()->Render();

}

/////////////////////////////////////////////////////////////////////////////
// CNVEffectsDoc commands

void CNVEffectsDoc::Tick()
{
	m_pTimer->Frame();

	if (m_bValidEffect)
	{
		EBProperty* pProp = m_pCurrentEffect->GetProperty("EffectDirtyFlags");
		if (pProp && pProp->IsKindOf(EBTYPE_DWORD_PROP))
		{
			DWORD dwDirtyFlags = pProp->ReadValue(m_pCurrentEffect);
			if (dwDirtyFlags != 0)
			{
				RebuildEffect(dwDirtyFlags);
				
				// Must re-get the effectdirty property because we caused the 
				// effect to rebuild it's property list.
				pProp = m_pCurrentEffect->GetProperty("EffectDirtyFlags");
				if (pProp)
					pProp->WriteValue(m_pCurrentEffect, (DWORD)0);
			}
		}

		m_pCurrentEffect->Tick(m_pTimer);	

		if (m_bMovieRecording)
		{	
			m_pMovie->AddFrame();
		}
	}
}

void CNVEffectsDoc::OnChangedViewList() 
{
	DWORD dwProgramView = 0;
	CDocument::OnChangedViewList();

	if (!m_pRenderView)
	{
		POSITION pos = GetFirstViewPosition();
		// Find the display view
		while(pos != NULL)
		{
			CView* pView = GetNextView( pos );
			if (pView->IsKindOf(RUNTIME_CLASS(CRenderView)))
			{
				m_pRenderView = DYNAMIC_DOWNCAST(CRenderView, pView);
				DISPDBG(3, "Found RenderView");
			}
			else if(pView->IsKindOf(RUNTIME_CLASS(CProgramFormView)))
			{
				CProgramFormView* pProgramFormView = DYNAMIC_DOWNCAST(CProgramFormView, pView);
				m_pProgramFormView[dwProgramView++] = pProgramFormView;
			}
			else if (pView->IsKindOf(RUNTIME_CLASS(CShaderTree)))
			{
				m_pTreeView = DYNAMIC_DOWNCAST(CShaderTree, pView);
				DISPDBG(3, "Found Tree View");
			}
		}
	}
}

bool CNVEffectsDoc::BuildEffectMenu(EBEffect* pEffect)
{
	bool bPendingSeperator = false;
	DWORD dwMenuID = IDM_MENUFREE1;

	if (m_bEffectMenu)
	{
		// Find a better way to get the last menu position
		AfxGetMainWnd()->GetMenu()->DeleteMenu(4, MF_BYPOSITION);
	}

	m_EffectMenu.DestroyMenu();
	m_mapMenuItem.clear();

	if (m_bValidEffect)
	{
		// Rebuild menus
		m_EffectMenu.CreatePopupMenu();

		// Update the Effect's properties
		pEffect->UpdateProperties();

		// Walk the property list and add any we recognise to the menu
		EBProperty* pProp = pEffect->GetFirstProperty();
		while (pProp != NULL)
		{
			string strName = pProp->GetPropertyName();
			
			// not interested in private properties
			if (pProp->GetFlags() & PROPFLAG_PRIVATE)
			{
				pProp = pEffect->GetNextProperty();
				continue;
			}


			if (bPendingSeperator)
			{
				m_EffectMenu.AppendMenu(MF_SEPARATOR);
				bPendingSeperator = false;
			}

			// Boolean properties
			if (pProp->IsKindOf(EBTYPE_BOOL_PROP))
			{
				m_EffectMenu.AppendMenu(MF_STRING, dwMenuID, strName.c_str());
				
				bool bValue = static_cast<EBProperty*>(pProp)->ReadValue(pEffect);
				CheckMenuItem(m_EffectMenu.m_hMenu, dwMenuID, bValue ? MF_CHECKED : MF_UNCHECKED);

				m_mapMenuItem.insert(make_pair<DWORD, CEffectMenuItem>(dwMenuID, CEffectMenuItem(pProp, m_EffectMenu.m_hMenu, dwMenuID)));
				dwMenuID++;
			}
			else if (pProp->IsKindOf(EBTYPE_TRIGGER_PROP))
			{
				m_EffectMenu.AppendMenu(MF_STRING, dwMenuID, strName.c_str());

				m_mapMenuItem.insert(make_pair<DWORD, CEffectMenuItem>(dwMenuID, CEffectMenuItem(pProp, m_EffectMenu.m_hMenu, dwMenuID)));
				dwMenuID++;
			}
			// Enum properties.  More work, need to add a popup
			else if (pProp->IsKindOf(EBTYPE_ENUM_PROP))
			{
				EBEnumProperty* pEnumProp = static_cast<EBEnumProperty*>(pProp);
				t_listEnum mapEnumerants = pEnumProp->GetEnumerants();

				if (!mapEnumerants.empty())
				{
					t_listEnum::const_iterator itrEnum = mapEnumerants.begin();
					
					if (dwMenuID != IDM_MENUFREE1)
						m_EffectMenu.AppendMenu(MF_SEPARATOR);

					while (itrEnum != mapEnumerants.end())
					{
						assert(*itrEnum);
						m_EffectMenu.AppendMenu(MF_STRING, dwMenuID, (*itrEnum)->GetPropertyName().c_str());
						if (pEnumProp->ReadValue(pEffect) == (*itrEnum)->GetValue())
						{
							CheckMenuItem(m_EffectMenu.m_hMenu, dwMenuID, MF_CHECKED);
						}
						else
						{
							CheckMenuItem(m_EffectMenu.m_hMenu, dwMenuID, MF_UNCHECKED);
						}

						m_mapMenuItem.insert(make_pair<DWORD, CEffectMenuItem>(dwMenuID, CEffectMenuItem((*itrEnum), m_EffectMenu.m_hMenu, dwMenuID)));
						dwMenuID++;
						itrEnum++;
					}

					bPendingSeperator = true;
				}
			}

			pProp = pEffect->GetNextProperty();
		}
		
		// Create the menu
		if (dwMenuID != IDM_MENUFREE1)
		{
			string strTitle;
			EBProperty* pEffectName = pEffect->GetProperty("EffectName");
			if (pEffectName && pEffectName->IsKindOf(EBTYPE_STRING_PROP))
			{
				strTitle = pEffectName->ReadValue(pEffect);
			}
			else
			{
				strTitle = ("Unknown!");
			}
			
			AfxGetMainWnd()->GetMenu()->AppendMenu(MF_POPUP, (UINT)m_EffectMenu.m_hMenu, strTitle.c_str());
			m_bEffectMenu = true;
		}
		AfxGetMainWnd()->DrawMenuBar();
	}
	return true;
}

bool CNVEffectsDoc::SetCurrentEffect(EBEffect* pEffect)
{
	string strTitle;
	assert(pEffect);
	assert(m_pRenderView);

	if (m_pCurrentEffect && m_bValidEffect)
		m_pCurrentEffect->Free();

	m_pCurrentEffect = pEffect;
	m_bValidEffect = true;

	HRESULT hr = S_OK;
	string strError;
	
	{
		// Put up the busy wait cursor while we get ready
		CWaitCursor Wait;

		// Select the appropriate type of rendering window
		switch (pEffect->API()) {
		case GFXAPI_D3D:
			hr = m_pRenderView->SetRenderMode(CRenderView::D3DRenderMode);
			break;
		case GFXAPI_OPENGL	:
			hr = m_pRenderView->SetRenderMode(CRenderView::OGLRenderMode);
			break;
		default:
			strError = "Effect uses unknown rendering API.";
			hr = E_FAIL;
		}
		
		// Check if the effect can use this rendering window
		if (SUCCEEDED(hr))
			hr = GetRenderWnd()->SetupEffect(pEffect);
	}

	// Get the error string if the 
	if (hr == NVEFF_ERR_NOTINITIALIZED) {

		strError = "Rendering window not initialized.";

	} else if (FAILED(hr)) {

		EBProperty* pErr = pEffect->GetProperty("LastError");
		if (pErr->IsKindOf(EBTYPE_STRING_PROP))	{

			strError = pErr->ReadValue(pEffect);
			strError += "\nTry selecting a different device from the file menu.";

		} else {

			strError = "Unknown problem (test does not export LastError property).";
		}
	}

	if (FAILED(hr)) {

		// Unable to setup effect
		MessageBox(m_pRenderView->GetSafeHwnd(), strError.c_str(),
			"Error Selecting Effect", MB_OK | MB_ICONEXCLAMATION);

		pEffect->Free();
		return m_bValidEffect = false;
	}

	// Set the effect title
	EBProperty* pEffectName = pEffect->GetProperty("EffectName");
	if (pEffectName && pEffectName->IsKindOf(EBTYPE_STRING_PROP))
	{
		strTitle = pEffectName->ReadValue(pEffect);
	}
	else
	{
		strTitle = ("Unknown!");
	}
	SetTitle(strTitle.c_str());

	// Show the current program
	m_pProgramFormView[0]->SetEffect(pEffect);
	m_pProgramFormView[1]->SetEffect(pEffect);

	// Build the effect menu
	BuildEffectMenu(pEffect);

	// Start rendering if the effect is valid
	if (m_bValidEffect) {
		m_pTimer->Start(30.0f);
		pEffect->Start();
		GetRenderWnd()->Clear();
		GetRenderWnd()->ShowWindow(SW_SHOW);
	}

	return m_bValidEffect = true;
}

bool CNVEffectsDoc::RebuildEffect(DWORD dwDirtyFlags)
{
	if (dwDirtyFlags & EBEFFECT_DIRTY_PUBLICSTATE)
	{
		BuildEffectMenu(m_pCurrentEffect);
	}
	
	if (dwDirtyFlags & EBEFFECT_DIRTY_VERTEXSHADERS)
	{
		if (m_pProgramFormView[0]->GetViewType() == VIEWTYPE_VERTEX)
		{
			m_pProgramFormView[0]->SetEffect(m_pCurrentEffect);
		}
		else
		{
			m_pProgramFormView[1]->SetEffect(m_pCurrentEffect);
		}
	}

	if (dwDirtyFlags & EBEFFECT_DIRTY_PIXELSHADERS)
	{
		if (m_pProgramFormView[0]->GetViewType() == VIEWTYPE_PIXEL)
		{
			m_pProgramFormView[0]->SetEffect(m_pCurrentEffect);
		}
		else
		{
			m_pProgramFormView[1]->SetEffect(m_pCurrentEffect);
		}
	}

	// Update the menu
	BuildEffectMenu(m_pCurrentEffect);

	return true;
}
	
bool CNVEffectsDoc::Start()
{
	if (m_listEffects.empty())
	{
		MessageBox(NULL, "No effects found!", "ERROR", MB_ICONEXCLAMATION | MB_OK);
		return false;
	}

	SetCurrentEffect(m_listEffects.front());

	// Always have a valid effect pointer, even if the effect is not valid
	if (!m_pCurrentEffect)
		m_pCurrentEffect = m_listEffects.front();

	m_pTreeView->PopulateTree();
	
	m_bStarted = true;

	return true;
}

bool CNVEffectsDoc::MenuHandler(UINT nID)
{
	std::map<UINT, CEffectMenuItem>::const_iterator itrProperty = m_mapMenuItem.find(nID);
	
	if (itrProperty == m_mapMenuItem.end())
		return false;

	CEffectMenuItem MenuItem = (*itrProperty).second;
	EBProperty* pProp = MenuItem.m_pProperty;
	if (pProp == NULL)
		return false;

	// Find out what type of property we are looking at and handle it appropriately.
	if (pProp->IsKindOf(EBTYPE_BOOL_PROP))
	{
		bool bValue = pProp->ReadValue(m_pCurrentEffect);
		bValue = !bValue;

		pProp->WriteValue(m_pCurrentEffect, bValue);
		CheckMenuItem(MenuItem.m_hMenu, MenuItem.m_MenuID, bValue ? MF_CHECKED : MF_UNCHECKED);

	}
	else if (pProp->IsKindOf(EBTYPE_TRIGGER_PROP))
	{
		EBTriggerProperty* pTrigger = static_cast<EBTriggerProperty*>(pProp);
		pTrigger->Trigger(m_pCurrentEffect);
	}
	// Enumerated types have sub-menus with options in them.
	// Look at the flags to check if it's a static enum (can't do iskindof because
	// enums can be many different types
	else if (pProp->GetFlags() & PROPFLAG_ENUMSTATIC)
	{
		HMENU hEnumMenu = NULL;
		EBEnumValue* pEnumVal = static_cast<EBEnumValue*>(pProp);
		EBEnumProperty* pEnumProp = pEnumVal->GetParent();

		// Write the new property value
		pEnumProp->WriteValue(m_pCurrentEffect, pEnumVal->GetValue());
		
		// Walk the list of enumerants and set the menus checkboxes
		t_listEnum mapEnumerants = pEnumProp->GetEnumerants();
		t_listEnum::const_iterator itrEnum = mapEnumerants.begin();
		while (itrEnum != mapEnumerants.end())
		{	
			itrProperty = m_mapMenuItem.begin();
			while (itrProperty != m_mapMenuItem.end())
			{
				// If this item is the current enumerant in the list...
				if ((*itrProperty).second.m_pProperty == (*itrEnum))
				{
					CEffectMenuItem ThisItem = (*itrProperty).second;

					// .. Check/uncheck the value
					if ((*itrEnum)->GetValue() == pEnumVal->GetValue())
					{
						CheckMenuItem(ThisItem.m_hMenu, ThisItem.m_MenuID, MF_CHECKED);
					}
					else
					{
						CheckMenuItem(ThisItem.m_hMenu, ThisItem.m_MenuID, MF_UNCHECKED);
					}
					break;
				}
				itrProperty++;
			}			
			
			itrEnum++;
		}	
	}

	return true;
}

BOOL CNVEffectsDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	std::map<UINT, CEffectMenuItem>::const_iterator itrProperty = m_mapMenuItem.find(nID);
	if (itrProperty != m_mapMenuItem.end())
	{
		if (nCode == CN_COMMAND && !pHandlerInfo)
			MenuHandler(nID);
		return true;
	}
	
	return CDocument::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


HRESULT CNVEffectsDoc::InitDeviceObjects()
{
	if (m_bStarted && m_bValidEffect)
	{
		SetCurrentEffect(m_pCurrentEffect);
	}
    return S_OK;
}

HRESULT CNVEffectsDoc::DeleteDeviceObjects()
{
	if (m_bStarted)
	{
		m_pCurrentEffect->Free();
	}

    return S_OK;
}

HRESULT CNVEffectsDoc::ConfirmDevice(EBEffect *pEffect)
{
	if (m_bStarted)
	{
		return GetRenderWnd()->ConfirmDevice(pEffect);
	}

	return S_OK;
}

void CNVEffectsDoc::OnCaptureSaveimage() 
{
	HRESULT hr;
	LPDIRECT3DDEVICE8 pDevice;
	LPDIRECT3DSURFACE8 pBackBuffer;
	D3DLOCKED_RECT LockedRect;
	D3DSURFACE_DESC SurfaceDesc;
	BITMAPINFO bmi;
	BITMAPFILEHEADER bmf;
	DWORD Pitch;
	int y, x;
	WORD Color16;
	DWORD* ScanLine;
	DWORD ScanItem;
	
	// XXX - Shouldn't be force casting...clean this up!
	((CRenderD3DWnd*) GetRenderWnd())->GetDevice(&pDevice);

	const char* Filter = "Bitmap Files (*.bmp)|*.bmp||";
	CFileDialog FileDialog(false, ".bmp", "effect.bmp", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,  Filter);
	if (FileDialog.DoModal() != IDOK)
	{
		SAFE_RELEASE(pDevice);
		return;
	}

	CString PathName = FileDialog.GetPathName();
	
	CFile file;
	if( !file.Open(PathName, CFile::modeWrite|CFile::modeCreate) )
	{
		MessageBox(NULL, "Failed to open file!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	
	hr = pDevice->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, "Failed to get backbuffer!", "Error", MB_OK | MB_ICONEXCLAMATION);

		SAFE_RELEASE(pDevice);
		return;
	}

	pBackBuffer->GetDesc(&SurfaceDesc);

	// Get the pitch of the backbuffer
	switch(SurfaceDesc.Format)
	{
		case D3DFMT_A8R8G8B8:
			Pitch = SurfaceDesc.Width * 4;
			break;
		case D3DFMT_R5G6B5:
		case D3DFMT_A1R5G5B5:
			Pitch = SurfaceDesc.Width * 2;
			break;
	}

	ZeroMemory(&bmf, sizeof(BITMAPFILEHEADER));
	bmf.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPFILEHEADER) + ((SurfaceDesc.Width * 4) * SurfaceDesc.Height);
	bmf.bfType = ((WORD) ('M' << 8) | 'B');
	bmf.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);

	// Write out the bitmap info
	file.Write(&bmf, sizeof(BITMAPFILEHEADER));
	
	// Fill in the bitmapinfo structure
	ZeroMemory(&bmi, sizeof(BITMAPINFOHEADER));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = SurfaceDesc.Width;
    bmi.bmiHeader.biHeight = SurfaceDesc.Height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = 0;
    bmi.bmiHeader.biXPelsPerMeter = 4000;
    bmi.bmiHeader.biYPelsPerMeter = 4000;
    bmi.bmiHeader.biClrUsed = 0;
    bmi.bmiHeader.biClrImportant = 0;
	
	// Write out the bitmap info
	file.Write(&bmi, sizeof(BITMAPINFO));
		
	pBackBuffer->LockRect(&LockedRect, NULL, D3DLOCK_READONLY);
	if (LockedRect.Pitch != 0)
		Pitch = LockedRect.Pitch;

	ScanLine = (DWORD*)HeapAlloc(GetProcessHeap(), 0, SurfaceDesc.Width * 4);

	switch(SurfaceDesc.Format)
	{
		case D3DFMT_A8R8G8B8:
		case D3DFMT_X8R8G8B8:
			for (y = SurfaceDesc.Height - 1; y >= 0; y--)
			{
				file.Write(((BYTE*)LockedRect.pBits + (y * Pitch)), SurfaceDesc.Width * 4);
			}
			break;
		case D3DFMT_R5G6B5:
			for (y = SurfaceDesc.Height - 1; y >= 0; y--)
			{
				ScanItem = 0;
				for (x = 0; x < SurfaceDesc.Width; x++)
				{
					Color16 = *((WORD*)((BYTE*)LockedRect.pBits + (y * Pitch)) + x);

					ScanLine[ScanItem++] = (((Color16 & 0xF800) >> 8) << 16) |
								(((Color16 & 0x07E0) >> 3) << 8) |
								(((Color16 & 0x001F) << 3));
				}
				file.Write(ScanLine, ScanItem * 4);
			}
			break;
		case D3DFMT_A1R5G5B5:
			for (y = SurfaceDesc.Height - 1; y >= 0; y--)
			{
				ScanItem = 0;
				for (x = 0; x < SurfaceDesc.Width; x++)
				{
					Color16 = *((WORD*)((BYTE*)LockedRect.pBits + (y * Pitch)) + x);

					ScanLine[ScanItem++] = (((Color16 & 0x7C00) >> 8) << 16) |
								(((Color16 & 0x03E0) >> 2) << 8) |
								(((Color16 & 0x001F) << 3));

				}
				file.Write(ScanLine, ScanItem * 4);
			}

			break;
		default:
			assert(0 && "Error - unknown surface format during save");
			break;
	}

	HeapFree(GetProcessHeap(), 0, ScanLine);

	pBackBuffer->UnlockRect();
	
	SAFE_RELEASE(pBackBuffer);
	SAFE_RELEASE(pDevice);
}

void CNVEffectsDoc::OnUpdateCaptureSaveimage(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);	
}

void CNVEffectsDoc::OnCaptureBeginmovie() 
{
	HRESULT hr;
	LPDIRECT3DSURFACE8 pBackBuffer;
	LPDIRECT3DDEVICE8 pDevice;
	
	// XXX - Shouldn't be force casting...fix this!
	((CRenderD3DWnd*) GetRenderWnd())->GetDevice(&pDevice);

	hr = pDevice->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pBackBuffer);
		SAFE_RELEASE(pDevice);
		return;
	}

	const char* Filter = "AVI Files (*.avi)|*.avi||";
	CFileDialog FileDialog(false, ".avi", "out.avi", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,  Filter);
	if (FileDialog.DoModal() == IDOK)
	{

		CString PathName = FileDialog.GetPathName();
		if (m_pMovie->BeginRecord((LPCTSTR)PathName, pBackBuffer))
		{
			m_bMovieRecording = true;
		}

		SAFE_RELEASE(pBackBuffer);
		SAFE_RELEASE(pDevice);
	}

}

void CNVEffectsDoc::OnUpdateCaptureBeginmovie(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bMovieRecording);
	
}

void CNVEffectsDoc::OnCaptureEndmovie() 
{
	m_pMovie->EndRecord();
	m_bMovieRecording = false;
	
}

void CNVEffectsDoc::OnUpdateCaptureEndmovie(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bMovieRecording);
	
}

void CNVEffectsDoc::OnFileHardwaredevice() 
{
	theApp.SetDeviceType(D3DDEVTYPE_HAL);
	ChangeDevice();
}

void CNVEffectsDoc::OnUpdateFileHardwaredevice(CCmdUI* pCmdUI) 
{
	 pCmdUI->SetCheck((int)theApp.GetDeviceType() == D3DDEVTYPE_HAL);
}

void CNVEffectsDoc::OnDeviceReference() 
{
	theApp.SetDeviceType(D3DDEVTYPE_REF);
	ChangeDevice();
}

void CNVEffectsDoc::OnUpdateDeviceReference(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck((int)theApp.GetDeviceType() == D3DDEVTYPE_REF);
}

void CNVEffectsDoc::OnDeviceForcesoftwarevp() 
{
	theApp.SetForceSoftwareVP(!theApp.GetForceSoftwareVP());
	ChangeDevice();
}

void CNVEffectsDoc::OnUpdateDeviceForcesoftwarevp(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck((int)theApp.GetForceSoftwareVP());
	
}

void CNVEffectsDoc::ChangeDevice()
{
	m_pRenderView->UpdateDevice(m_pCurrentEffect);
	m_pTreeView->PopulateTree();
}

bool CNVEffectsDoc::DoTestPopup(int x, int y)
{
	if (m_EffectMenu.m_hMenu)
		m_EffectMenu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, x, y, (CWnd*)m_pRenderView);
	return true;
}


void CNVEffectsDoc::OnCloseDocument() 
{
	if (m_pCurrentEffect)
		m_pCurrentEffect->Free();

	while(!m_listEffects.empty())	
	{
		EBEffect* pEffect = m_listEffects.back();
		pEffect->DestroyEffect();
		m_listEffects.pop_back();
	};

	tmapEffectFileData::iterator itrEffects = m_EffectFileData.begin();
	while (itrEffects != m_EffectFileData.end())
	{
		HINSTANCE hLib = (*itrEffects).second.hLib;
		if (hLib)
			FreeLibrary(hLib);

		(*itrEffects).second.hLib = NULL;

		itrEffects++;
	}
	m_EffectFileData.clear();

	SAFE_DELETE(m_pTimer);
	SAFE_DELETE(m_pMovie);
		
	CDocument::OnCloseDocument();
}

void CNVEffectsDoc::OnOptionsExpandall() 
{
	theApp.SetExpandAll(!theApp.GetExpandAll());	
}

void CNVEffectsDoc::OnUpdateOptionsExpandall(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(theApp.GetExpandAll());	
}
