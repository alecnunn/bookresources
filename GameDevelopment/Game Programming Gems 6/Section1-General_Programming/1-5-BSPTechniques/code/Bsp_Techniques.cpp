// Bsp_Techniques.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Bsp_Techniques.h"
#include "Bsp_TechniquesDlg.h"


//------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CBsp_TechniquesApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------
CBsp_TechniquesApp::CBsp_TechniquesApp()
{
}

CBsp_TechniquesApp theApp;

//------------------------------------------------------------------------------
BOOL CBsp_TechniquesApp::InitInstance()
{
	AfxEnableControlContainer();
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

    WNDCLASS wndclsGL;
    ::memset(&wndclsGL, 0, sizeof(WNDCLASS));   // start with NULL defaults
    wndclsGL.style         = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW|CS_OWNDC;
    wndclsGL.lpfnWndProc   = ::DefWindowProc;
    wndclsGL.hInstance     = AfxGetInstanceHandle();
    wndclsGL.hIcon         = 0;
    wndclsGL.hbrBackground = (HBRUSH) 0;
    wndclsGL.lpszMenuName  = 0;
    wndclsGL.hCursor       = ::LoadCursor(0,IDC_CROSS);
    wndclsGL.lpszClassName = "BSP_TECH_GL3D_WND";
    AfxRegisterClass(&wndclsGL);
    
    CBsp_TechniquesDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}


