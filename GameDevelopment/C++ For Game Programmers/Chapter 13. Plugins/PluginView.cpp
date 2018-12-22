// PluginView.cpp : implementation file
//

#include "stdafx.h"
#include "pluginsample.h"
#include "PluginView.h"
#include "Plugin.h"
#include "PluginManager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PluginView dialog


PluginView::PluginView(CWnd* pParent /*=NULL*/)
	: CDialog(PluginView::IDD, pParent)
{
	//{{AFX_DATA_INIT(PluginView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void PluginView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PluginView)
	DDX_Control(pDX, IDC_LIST2, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PluginView, CDialog)
	//{{AFX_MSG_MAP(PluginView)
	ON_BN_CLICKED(IDABOUT, OnAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL PluginView::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

    PluginManager & mgr = PluginManager::GetInstance();
    for (int i=0; i < mgr.GetNumPlugins(); ++i)
    {
        IPlugin * pPlugin = mgr.GetPlugin(i);
        m_list.InsertString(i, pPlugin->GetName().c_str());
    }
	
	return TRUE;
}


void PluginView::OnAbout() 
{
    int nIdx = m_list.GetCurSel();
    if (nIdx == LB_ERR)
        return;

    IPlugin * pPlugin = PluginManager::GetInstance().GetPlugin(nIdx);
    if ( pPlugin == NULL )
        return;

    pPlugin->About(GetSafeHwnd());
}
