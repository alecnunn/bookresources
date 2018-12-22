/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#include "stdafx.h"
#include "csg_bsp.h"
#include "MainFrm.h"
#include "Z_ed3View.h"
#include "Z_ed2View.h"
#include "compiler.h"
#include "z_ed2Doc.h"


//---------------------------------------------------------------------------------------
CMainFrame* CMainFrame::PFrame;

//---------------------------------------------------------------------------------------
// Message maps of window messages with function handlers for Mainframe object
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
ON_MESSAGE(WM_USR_REPAINT, OnPostRepaint)
    ON_WM_CREATE()
    ON_WM_TIMER()
    ON_WM_DESTROY()
    ON_UPDATE_COMMAND_UI(ID_BUSHOWDLG, OnUpdateBushowdlg)
    ON_COMMAND(MN_GLOBALS, OnGlobals)
    ON_WM_CLOSE()
    ON_COMMAND(ID_SETTINGS_PREFERENCES, OnSettingsPreferences)
    ON_COMMAND(ID_BUSHOWDLG, OnBushowdlg)
    ON_COMMAND(MN_HELP, OnHelp)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------
static UINT indicators[] =
{
        ID_SEPARATOR,
        SB_CAMPOS,
        SB_MOUSEPOS,
        SB_2DPOS,
        SB_BRUSHBBOX,
};

//---------------------------------------------------------------------------------------
// Default constructor
CMainFrame::CMainFrame()
{
    CMainFrame::PFrame = this;
}

//---------------------------------------------------------------------------------------
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    if (!m_wndToolBar.CreateEx(this) ||
            !m_wndToolBar.LoadToolBar(IDR_MAINFRAME)){
        return -1;      // fail to create
    }
    if (!m_wndStatusBar.Create(this) ||
            !m_wndStatusBar.SetIndicators(indicators,
                                          sizeof(indicators)/sizeof(UINT))){
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }
	
    // Sets separators width
    m_wndStatusBar.SetPaneInfo(0, ID_SEPARATOR, SBPS_NOBORDERS  ,190);
    m_wndStatusBar.SetPaneInfo(1, SB_CAMPOS, SBPS_NORMAL,   200) ;
    m_wndStatusBar.SetPaneInfo(2, SB_MOUSEPOS, SBPS_NORMAL,120);
    m_wndStatusBar.SetPaneInfo(3, SB_2DPOS, SBPS_NORMAL,150);
    m_wndStatusBar.SetPaneInfo(4, SB_BRUSHBBOX, SBPS_NORMAL,130);

    SBT(ID_SEPARATOR,"Ready...");
    SBT(SB_MOUSEPOS,"");
    SBT(SB_2DPOS,"");
    SBT(SB_BRUSHBBOX,"");

    // Creates the font for statusbar couse I cannot see it without my glasses on
    _cfont.CreateFont(17,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DRAFT_QUALITY,DEFAULT_PITCH,"arial"	);
    m_wndStatusBar.SetFont(&_cfont);
    m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |CBRS_TOOLTIPS | CBRS_FLYBY);
    m_wndAlignTbar.SetBarStyle(m_wndAlignTbar.GetBarStyle() |CBRS_TOOLTIPS | CBRS_FLYBY);
    SetTimer(333,50,0);
    RecalcLayout();
    OnHelp();
    return 0;
}

//---------------------------------------------------------------------------------------
// Create the splitters
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
                                CCreateContext* pContext)
{
    RECT r;
    if (!m_wndSplitter.CreateStatic(this, 2, 2))
    {
        return FALSE;
    }
    GetClientRect(&r);
    int x = r.right  / 2;
    int y = r.bottom / 2;

    if (!m_wndSplitter.CreateView(0, 0,
                                  RUNTIME_CLASS(z_ed3View), CSize(x+x/4, y+y/4), pContext))
    {
        return FALSE;
    }

    if (!m_wndSplitter.CreateView(0, 1,
                                  RUNTIME_CLASS(CZ_ed2View), CSize(x, y), pContext))
    {
        return FALSE;
    }

    if (!m_wndSplitter.CreateView(1, 0,
                                  RUNTIME_CLASS(CZ_ed2View), CSize(x, y), pContext))
    {
        return FALSE;
    }

    if (!m_wndSplitter.CreateView(1, 1,
                                  RUNTIME_CLASS(CZ_ed2View), CSize(x, y), pContext))
    {
        return FALSE;
    }

    ((CZ_ed2View *)m_wndSplitter.GetPane(0,1))->_vt = 'y';
    ((CZ_ed2View *)m_wndSplitter.GetPane(1,0))->_vt = 'x';
    ((CZ_ed2View *)m_wndSplitter.GetPane(1,1))->_vt = 'z';

    return TRUE;
}

//---------------------------------------------------------------------------------------
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CFrameWnd::PreCreateWindow(cs) )
        return FALSE;
    return TRUE;
}

//---------------------------------------------------------------------------------------
void CMainFrame::OnTimer(UINT nIDEvent)
{
    ((CZ_ed2View *)m_wndSplitter.GetPane(0,0))->Invalidate();
    CFrameWnd::OnTimer(nIDEvent);
}

//---------------------------------------------------------------------------------------
void CMainFrame::OnDestroy()
{
    KillTimer(333);
    CFrameWnd::OnDestroy();
}

//---------------------------------------------------------------------------------------
void CMainFrame::OnBushowdlg()
{
    this->RecalcLayout();
}

//---------------------------------------------------------------------------------------
void CMainFrame::OnUpdateBushowdlg(CCmdUI* pCmdUI)
{
}

//---------------------------------------------------------------------------------------
void CMainFrame::OnGlobals()
{
}

//---------------------------------------------------------------------------------------
void CMainFrame::OnClose()
{
    // release any opengl and gdi contexts from all views
    ((z_ed3View *)m_wndSplitter.GetPane(0,0))->ReleaseRDI();
    ((CZ_ed2View *)m_wndSplitter.GetPane(1,0))->ReleaseRDI();
    ((CZ_ed2View *)m_wndSplitter.GetPane(0,1))->ReleaseRDI();
    ((CZ_ed2View *)m_wndSplitter.GetPane(1,1))->ReleaseRDI();
    CFrameWnd::OnClose();
}

//---------------------------------------------------------------------------------------
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//---------------------------------------------------------------------------------------
LRESULT CMainFrame::OnPostRepaint(WPARAM pbr1, LPARAM pbr2)
{
    static int staticLoop = 0;

    if(pbr1){
        ((Brush*) pbr1)->Select();
        ((Brush*) pbr2)->Select();
        DOC()->_pSelBrushes << ((Brush*) pbr1);
        DOC()->_pSelBrushes << ((Brush*) pbr2);
    }
    else{
        DOC()->DeselectAll();
    }

    switch(staticLoop++)
    {
    case 0:
        m_wndSplitter.GetPane(1,0)->SendMessage(WM_USR_REPAINT);
        break;
    case 1:
        m_wndSplitter.GetPane(0,1)->SendMessage(WM_USR_REPAINT);
        break;
    case 2:
        m_wndSplitter.GetPane(1,1)->SendMessage(WM_USR_REPAINT);
        staticLoop = 0;
        break;
    }
    return 0;
}


//---------------------------------------------------------------------------------------
BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
    return CFrameWnd::OnCommand(wParam, lParam);
}

//---------------------------------------------------------------------------------------
void CMainFrame::OnSettingsPreferences()
{
}

//---------------------------------------------------------------------------------------
#include "DlgHelp.h"
void CMainFrame::OnHelp() 
{
    DlgHelp().DoModal();
}
