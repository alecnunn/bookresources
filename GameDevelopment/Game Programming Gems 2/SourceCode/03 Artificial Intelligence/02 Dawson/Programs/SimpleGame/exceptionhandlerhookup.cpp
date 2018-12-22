/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
#include "stdafx.h"

// Intercept the MFC startup, and wrap it in a structured exception handler
// to catch all crashes.

int HandledAfxWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
{
	ASSERT(hPrevInstance == NULL);
	
	int nReturnCode = -1;
	CWinApp* pApp = AfxGetApp();

	// AFX internal initialization
	if (!AfxWinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
		goto InitFailure;

	// App global initializations (rare)
	ASSERT_VALID(pApp);
	if (!pApp->InitApplication())
		goto InitFailure;
	ASSERT_VALID(pApp);

	// Perform specific initializations
	if (!pApp->InitInstance())
	{
		if (pApp->m_pMainWnd != NULL)
		{
			TRACE0("Warning: Destroying non-NULL m_pMainWnd\n");
			pApp->m_pMainWnd->DestroyWindow();
		}
		nReturnCode = pApp->ExitInstance();
		goto InitFailure;
	}
	ASSERT_VALID(pApp);

	nReturnCode = pApp->Run();
	ASSERT_VALID(pApp);

InitFailure:

	AfxWinTerm();
	return nReturnCode;
}

int AFXAPI AfxWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
{
	ASSERT(hPrevInstance == NULL);

	// Wrap WinMain in a structured exception handler (different from C++
	// exception handling) in order to make sure that all access violations
	// and other exceptions are displayed - regardless of when they happen.
	__try
	{
		return HandledAfxWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	}
	__except (HandleException(GetExceptionInformation(), "Main Thread"))
	{
		// We don't actually do anything inside the handler. All of the
		// work is done by HandleException()
	}
	return 0;
}
