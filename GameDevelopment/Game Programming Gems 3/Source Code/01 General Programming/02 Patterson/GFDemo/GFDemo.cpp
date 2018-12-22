/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// GFDemo.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "GameApp_t.h"
#include "GFInclude/Systems/Win32/ProcessInfoWin32Impl_t.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{

#if GFX_MEMORY_CHECKER
	// memory allocation checking
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); 
#endif

	ProcessInfoWin32Impl_t *pProcessInfo = new ProcessInfoWin32Impl_t;
	pProcessInfo->m_hInst = hInstance;
	pProcessInfo->m_hPrevInst = hPrevInstance;
	pProcessInfo->m_lpCmdLine = lpCmdLine;
	pProcessInfo->m_nCmdShow = nCmdShow;

	GameApp_t *pGameApp = new GameApp_t;
	pGameApp->SetProcessInfo( pProcessInfo );
	pGameApp->Run();

	delete pGameApp;

	delete pProcessInfo;

#if GFX_MEMORY_CHECKER
	// dump the memory leak results
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}

