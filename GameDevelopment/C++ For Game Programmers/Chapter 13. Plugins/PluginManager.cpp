
#include "stdafx.h"
#include "PluginManager.h"
#include "Plugin.h"
#include "MainFrame.h"
#include <io.h>


using std::string;


PluginManager * PluginManager::s_pInstance = NULL;



PluginManager::PluginManager(){}

PluginManager & PluginManager::GetInstance()
{
    if ( s_pInstance == NULL )
        s_pInstance = new PluginManager();

    return *s_pInstance;
}


void PluginManager::LoadPlugins ( const string & strDir )
{
    std::vector<string> filenames;
    GetFilenames(strDir, filenames);

    std::vector<std::string>::const_iterator it;
    for (it = filenames.begin(); it != filenames.end(); ++it)
    {
        const string & filename = *it;
        string & fullName = strDir + string("\\") + filename;
        LoadPlugin (fullName);
    }
}


void PluginManager::GetFilenames ( const string & dir, 
                                  std::vector<string> & filenames ) const
{
    string mask = dir + string("\\*.plug");

    struct _finddata_t fileinfo;
    long handle = ::_findfirst(mask.c_str(), &fileinfo);
    long file   = handle;

	while (file >= 0)
	{
        filenames.push_back(string(fileinfo.name));
        file = ::_findnext(handle, &fileinfo);
	}

    ::_findclose(handle);
}


bool PluginManager::LoadPlugin ( const string & filename )
{
    HMODULE hDll = ::LoadLibrary (filename.c_str());
    if ( hDll == NULL )
    {
        LPVOID lpMsgBuf;
        ::FormatMessage( 
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM | 
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPTSTR) &lpMsgBuf,
            0,
            NULL 
        );

        ::OutputDebugString((char *)lpMsgBuf);

        LocalFree( lpMsgBuf );
        return false;
    }


    CREATEPLUGIN pFunc = (CREATEPLUGIN)::GetProcAddress (hDll, _T("CreatePlugin"));
    if ( pFunc == NULL )
        return false;


    IPlugin * pPlugin = pFunc(*this);
    if ( pPlugin == NULL )
        return false;


    PluginInfo info;
    info.pPlugin = pPlugin;
    info.hDll    = hDll;

    m_plugins.push_back(info);


    MainFrame * pMainFrame = static_cast<MainFrame*>(::AfxGetMainWnd());
    pMainFrame->AddExporter(pPlugin->GetExportName());

    return true;
}


void PluginManager::UnloadAll ( void )
{
    MainFrame * pMainFrame = static_cast<MainFrame*>(::AfxGetMainWnd());

    std::vector<PluginInfo>::iterator it;
    for (it=m_plugins.begin(); it!=m_plugins.end(); ++it)
    {
        PluginInfo & info = *it;
            
        pMainFrame->RemoveExporter(info.pPlugin->GetExportName());

        delete info.pPlugin;
        ::FreeLibrary (info.hDll);
    }

    m_plugins.clear();
}


int PluginManager::GetNumPlugins ( void ) const
{
    return m_plugins.size();
}


IPlugin * PluginManager::GetPlugin ( int nIndex )
{
    if ( nIndex < 0 || nIndex >= GetNumPlugins() )
        return NULL;

    return m_plugins[nIndex].pPlugin;
}

