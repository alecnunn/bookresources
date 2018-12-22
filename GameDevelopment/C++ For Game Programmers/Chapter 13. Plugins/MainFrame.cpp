// MainFrame.cpp : implementation of the MainFrame class
//

#include "stdafx.h"
#include "MainFrame.h"
#include "pluginsample.h"
#include "pluginview.h"
#include "PluginManager.h"
#include "Plugin.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MainFrame

IMPLEMENT_DYNAMIC(MainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(MainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(MainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_PLUGINS_VIEWLOADEDPLUGINS, OnPluginsView)
	ON_COMMAND(ID_PLUGINS_RELOADPLUGINS, OnPluginsReload)
	ON_COMMAND(ID_PLUGINS_UNLOADPLUGINS, OnPluginsUnload)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
    ON_COMMAND_RANGE(ID_EXPORTER_START, ID_EXPORTER_START + 100, OnExporter)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// MainFrame construction/destruction

MainFrame::MainFrame()
{
    m_nNumExporterPlugins = 0;
}

MainFrame::~MainFrame()
{
}

int MainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	
	if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar))
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);


    PluginManager::GetInstance().LoadPlugins( GetPluginDirectory() );

	return 0;
}

BOOL MainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	cs.cx = 700;
	cs.cy = 400;
	return TRUE;
}


std::string MainFrame::GetPluginDirectory ( void )
{
    char fullfilename[_MAX_PATH];
    ::GetModuleFileName ( ::AfxGetInstanceHandle(),
	                      fullfilename,
                          _MAX_PATH );
    
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char filename[_MAX_FNAME];
    char ext[_MAX_EXT];
    ::_splitpath ( fullfilename, drive, dir, filename, ext );

    return std::string(drive) + std::string(dir) + std::string("plugins");
}




/////////////////////////////////////////////////////////////////////////////
// MainFrame diagnostics

#ifdef _DEBUG
void MainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void MainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// MainFrame message handlers
void MainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL MainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}



void MainFrame::OnPluginsView() 
{
	PluginView dlg;
	dlg.DoModal();
}

void MainFrame::OnPluginsReload() 
{
    PluginManager::GetInstance().UnloadAll();
    PluginManager::GetInstance().LoadPlugins( GetPluginDirectory() );	
}

void MainFrame::OnPluginsUnload() 
{
    PluginManager::GetInstance().UnloadAll();
}

void MainFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();
	
    PluginManager::GetInstance().UnloadAll();	
}


void MainFrame::OnExporter(UINT nID)
{
    int nIdx = nID - ID_EXPORTER_START;

    IPlugin * pPlugin = PluginManager::GetInstance().GetPlugin(nIdx);
    if ( pPlugin != NULL )
        pPlugin->Export("Random data to export...");
}


void MainFrame::AddExporter ( const std::string & strExporterName )
{
    CMenu * pMenu = GetExporterMenu();

    if (m_nNumExporterPlugins == 0)
    {
        pMenu->RemoveMenu(0, MF_BYPOSITION);
    }

    int nCommand = ID_EXPORTER_START + m_nNumExporterPlugins;
    pMenu->InsertMenu(-1, MF_BYPOSITION, nCommand, strExporterName.c_str());
    ++m_nNumExporterPlugins;
}


void MainFrame::RemoveExporter ( const std::string & strExporterName )
{
    CMenu * pMenu = GetExporterMenu();

    for ( int i=0; i < (int)pMenu->GetMenuItemCount(); ++i )
    {
        char strName[256];
        if (pMenu->GetMenuString (i, strName, 256, MF_BYPOSITION))
        {
            if ( !::stricmp(strName, strExporterName.c_str()) )
            {
                pMenu->RemoveMenu (i, MF_BYPOSITION);
                --m_nNumExporterPlugins;
                break;
            }
        }        
    }
}



CMenu * MainFrame::GetExporterMenu ()
{
    CMenu * pMenu = GetMenu();
    pMenu = pMenu->GetSubMenu(0);
    pMenu = pMenu->GetSubMenu(0);
    return pMenu;
}