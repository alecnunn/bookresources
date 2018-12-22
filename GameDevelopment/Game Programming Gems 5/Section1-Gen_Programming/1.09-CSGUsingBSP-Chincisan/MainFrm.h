/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#ifndef __MAINFRAME_G_H__
#define __MAINFRAME_G_H__

//---------------------------------------------------------------------------------------
#define WM_USR_REPAINT  WM_USER+1002

//---------------------------------------------------------------------------------------
// MFC mainframe class
class CMainFrame : public CFrameWnd
{
public:

    CMainFrame();
    DECLARE_DYNCREATE(CMainFrame)
    CSplitterWnd m_wndSplitter;
    static CMainFrame* PFrame;
public:
    virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

    CStatusBar  m_wndStatusBar;
    CToolBar    m_wndToolBar;
    CToolBar    m_wndAlignTbar;
    CReBar      m_wndReBar;
    CFont		_cfont;
    void        OnBushowdlg();

protected:
    LRESULT OnPostRepaint(WPARAM, LPARAM);          //  delayed paint
    int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnTimer(UINT nIDEvent);
    void OnDestroy();
    void OnUpdateBushowdlg(CCmdUI* pCmdUI);
    void OnGlobals();
    void OnClose();
    void OnSettingsPreferences();
	void OnHelp();
    DECLARE_MESSAGE_MAP()
};

//---------------------------------------------------------------------------------------
#define FRM()   CMainFrame::PFrame

//---------------------------------------------------------------------------------------
// Sets status bar text 'pTxt' in pane 'nPane'
inline void  SBT(int nPane,LPCTSTR pTxt)
{
    int index = CMainFrame::PFrame->m_wndStatusBar.CommandToIndex(nPane);
    CMainFrame::PFrame->m_wndStatusBar.SetPaneText(index,pTxt);
}

//---------------------------------------------------------------------------------------
// Logging functions
#define CLRLOG()    CMainFrame::PFrame->_dlgProgress.Clear()
#define LOG         CMainFrame::PFrame->_dlgProgress.Log
#define LOGERR      CMainFrame::PFrame->_dlgProgress.LogPos

//---------------------------------------------------------------------------------------
// Opearation id's for copiling phase
enum {
    E_COMP  = 1  ,
    E_HSR        ,
};


#endif // !__MAINFRAME_G_H__
