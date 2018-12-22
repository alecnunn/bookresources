/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
// linkerdialog.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "LinkerDialog.h"
#include "resource.h"
#include "linkercommon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLinkerDialog theApp;

// Exported functions
extern "C"
{

BOOL LinkerDialog_ShowDialog(bool bShow)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.ShowDialog(bShow);

}

BOOL LinkerDialog_Initialise(HWND hWndParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.Initialise(hWndParent);

}

BOOL LinkerDialog_Free()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.Free();

}

BOOL LinkerDialog_GetSettings(DialogSettings* pSettings)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.GetSettings(pSettings);
}

BOOL LinkerDialog_SetSettings(DialogSettings* pSettings)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.SetSettings(pSettings);
}

}


/////////////////////////////////////////////////////////////////////////////
// CLinkerdialogApp

BEGIN_MESSAGE_MAP(CLinkerDialog, CWinApp)
	//{{AFX_MSG_MAP(CLinkerDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkerdialogApp construction

CLinkerDialog::CLinkerDialog()
: m_bShow(false)
{
	
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CLinkerdialogApp object

BOOL CLinkerDialog::ShowDialog(bool bShow)
{
	m_bShow = bShow;
	m_TLOptions.ShowWindow(bShow);
	return true;
}

BOOL CLinkerDialog::Initialise(HWND hWnd)
{
	m_ParentWnd.Attach(hWnd);
	m_TLOptions.Create(MAKEINTRESOURCE(IDD_TLOPTIONS), &m_ParentWnd);
	ZeroMemory(&m_Settings, sizeof(DialogSettings));
	return true;
}

BOOL CLinkerDialog::Free()
{
	m_TLOptions.DestroyWindow();
	m_ParentWnd.Detach();
	return true;
}
	
BOOL CLinkerDialog::GetSettings(DialogSettings* pSettings) 
{
	assert(pSettings);

	DialogSettings Temp = m_Settings;
	
	m_TLOptions.UpdateData(TRUE);

	m_Settings.m_dwNumDirectionalLights = m_TLOptions.m_dwNumDirectionalLights;
	m_Settings.m_dwNumPointLights = m_TLOptions.m_dwNumPointLights;
	m_Settings.m_TexGen = (eTexGenOptions)m_TLOptions.m_TexGen;
	m_Settings.m_bSpecular = m_TLOptions.m_bSpecular;
	m_Settings.m_bEyeSpace = m_TLOptions.m_bEyeSpace;
	m_Settings.m_bLocalViewer = m_TLOptions.m_bLocalViewer;
	m_Settings.m_bLighting = m_TLOptions.m_bLighting;
	m_Settings.m_bTextures = m_TLOptions.m_bTextures;
	m_Settings.m_bWhiteLight = m_TLOptions.m_bWhiteLight;
	m_Settings.m_bSpecularRDotL = m_TLOptions.m_bSpecularRDotL;
	m_Settings.m_Fog = (eFogOptions)m_TLOptions.m_Fog;

	if (memcmp(&Temp, &m_Settings, sizeof(DialogSettings)) != 0)
	{
		m_Settings.m_bDirty = true;
	}
	else
	{
		m_Settings.m_bDirty = false;
	}

	*pSettings = m_Settings;

	UpdateEnables();

	return true;
}

BOOL CLinkerDialog::SetSettings(DialogSettings* pSettings) 
{
	assert(pSettings); 
	m_Settings = *pSettings;
	
	m_TLOptions.m_dwNumDirectionalLights = m_Settings.m_dwNumDirectionalLights;
	m_TLOptions.m_dwNumPointLights = m_Settings.m_dwNumPointLights;
	m_TLOptions.m_TexGen = (DWORD)m_Settings.m_TexGen;
	m_TLOptions.m_bSpecular = m_Settings.m_bSpecular;
	m_TLOptions.m_bEyeSpace = m_Settings.m_bEyeSpace;
	m_TLOptions.m_bLocalViewer = m_Settings.m_bLocalViewer;
	m_TLOptions.m_bLighting = m_Settings.m_bLighting;
	m_TLOptions.m_bTextures = m_Settings.m_bTextures;
	m_TLOptions.m_bWhiteLight = m_Settings.m_bWhiteLight;
	m_TLOptions.m_bSpecularRDotL = m_Settings.m_bSpecularRDotL;
	m_TLOptions.m_Fog = (DWORD)m_Settings.m_Fog;

	UpdateEnables();
	
	m_TLOptions.UpdateData(FALSE);
	return true;
}

BOOL CLinkerDialog::UpdateEnables()
{
	CWnd* pBlinnRadio = m_TLOptions.GetDlgItem(IDC_TEXGEN_BUMPMAPBASIS);
	CWnd* pCameraReflect = m_TLOptions.GetDlgItem(IDC_TEXGEN_CUBEMAPREFLECT);
	CWnd* pTexGenNone = m_TLOptions.GetDlgItem(IDC_TEXGEN_NONE);
	pBlinnRadio->EnableWindow(m_Settings.m_bTextures && m_Settings.m_bEnableBump);
	pCameraReflect->EnableWindow(m_Settings.m_bTextures && m_Settings.m_bEnableReflect);
	pTexGenNone->EnableWindow(m_Settings.m_bTextures);

	CWnd* pDirectional = m_TLOptions.GetDlgItem(IDC_EDIT_NUMDIRECTIONAL);
	CWnd* pPoint = m_TLOptions.GetDlgItem(IDC_EDIT_NUMPOINT);
	CWnd* pSpecular = m_TLOptions.GetDlgItem(IDC_LIGHTING_SPECULAR);
	CWnd* pLocalViewer = m_TLOptions.GetDlgItem(IDC_LIGHTING_LOCALVIEWER);
	CWnd* pEyeSpace = m_TLOptions.GetDlgItem(IDC_LIGHTING_EYESPACE);
	CWnd* pSpecularRDotL = m_TLOptions.GetDlgItem(IDC_LIGHTING_SPECULARREFLECT);
	pDirectional->EnableWindow(m_Settings.m_bLighting);
	pPoint->EnableWindow(m_Settings.m_bLighting);
	pSpecular->EnableWindow(m_Settings.m_bLighting);
	pLocalViewer->EnableWindow(m_Settings.m_bLighting && m_Settings.m_bSpecular && !m_Settings.m_bSpecularRDotL);
	pEyeSpace->EnableWindow(m_Settings.m_bLighting);
	pSpecularRDotL->EnableWindow(m_Settings.m_bLighting && m_Settings.m_bSpecular);
	
	CWnd* pTexture = m_TLOptions.GetDlgItem(IDC_ENABLETEXTURES);
	pTexture->EnableWindow(m_Settings.m_bEnableTextures);


	return TRUE;
}


// Standard init/close
BOOL CLinkerDialog::InitInstance() 
{
	return CWinApp::InitInstance();
}

int CLinkerDialog::ExitInstance() 
{
	return CWinApp::ExitInstance();
}





