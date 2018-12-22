#include "stdafx.h"
#include "atomos.h"

atomos::IEngine* g_pEngine = NULL;

// InitializeAtomos will load the atomos base library 
// and create the singleton engine object
// use this engine to load the needed plugins
void InitializeAtomos()
{

	// define the lib names

	ATOM_CHAR* pszBaseLib = _T("atombase");
	ATOM_CHAR* pszAudioLib = _T("atomgaud");

#ifdef _DEBUG
	pszBaseLib = _T("atombased");
	pszAudioLib = _T("atomgaudd");
#endif

	// load the base library and create the engine
	HINSTANCE hLib = ::LoadLibrary (pszBaseLib);
	if (hLib)
	{
		CREATE_ENGINE paddr = (CREATE_ENGINE)::GetProcAddress(hLib, "CreateEngine");
		g_pEngine = (atomos::IEngine*) (*paddr)();
	}
			
	// load the plugin(s)
	if(g_pEngine != NULL) {
		g_pGASystem = (atomos::IGASystem*)g_pEngine->LoadPlugin(pszAudioLib);

	}
}

