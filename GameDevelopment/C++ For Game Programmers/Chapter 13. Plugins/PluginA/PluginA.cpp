//  $Header:  $
//-----------------------------------------------------------------------------
//
//  PluginA.cpp
//
//      See PluginA.h for a detailed description of this class.
//
//      Thursday, October 17, 2002  --  10/17/2002
//
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "PluginA.h"
#include "..\PluginManager.h"
#include "AboutBox.h"


//-----------------------------------------------------------------------------
//  Constructors
PluginA::PluginA( PluginManager & mgr ) :
    m_mgr ( mgr ),
    m_Name ("PluginA"),
    m_ExportName ("Format A")
{
}


bool PluginA::Initialize()
{
    return false;
}


bool PluginA::Shutdown()
{
    return false;
}


void PluginA::About(HWND hParent)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    AboutBox aboutBox(CWnd::FromHandle(hParent));
	aboutBox.DoModal();
}


const std::string & PluginA::GetName()
{
    return m_Name;
}

const std::string & PluginA::GetExportName()
{
    return m_ExportName;
}


bool PluginA::Export( const char * pData )
{
    ::MessageBox (NULL, pData, "Exporting data in Format A", MB_OK);
    return true;
}



PLUGINDECL IPlugin * CreatePlugin (PluginManager & mgr)
{
    return new PluginA(mgr);
}
