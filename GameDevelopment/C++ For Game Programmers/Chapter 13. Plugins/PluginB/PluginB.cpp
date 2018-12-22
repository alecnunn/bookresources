
#include "stdafx.h"
#include "PluginB.h"
#include "..\PluginManager.h"
#include "AboutBox.h"


//-----------------------------------------------------------------------------
//  Constructors
PluginB::PluginB( PluginManager & mgr ) :
    m_mgr ( mgr ),
    m_Name ("PluginB"),
    m_ExportName ("Format B")
{
}


bool PluginB::Initialize()
{
    return false;
}


bool PluginB::Shutdown()
{
    return false;
}


void PluginB::About(HWND hParent)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    AboutBox aboutBox(CWnd::FromHandle(hParent));
	aboutBox.DoModal();
}


const std::string & PluginB::GetName()
{
    return m_Name;
}

const std::string & PluginB::GetExportName()
{
    return m_ExportName;
}


bool PluginB::Export( const char * pData )
{
    ::MessageBox (NULL, pData, "Exporting data in Format B", MB_OK);
    return true;
}



PLUGINDECL IPlugin * CreatePlugin (PluginManager & mgr)
{
    return new PluginB(mgr);
}
