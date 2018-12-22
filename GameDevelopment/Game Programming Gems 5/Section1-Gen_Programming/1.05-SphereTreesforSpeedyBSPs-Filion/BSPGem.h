//
// BSPGem.h
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

// BSP Gem aplication
//
// This is simply a default implementation of the standard MFC application

class CBSPGemApp : public CWinApp
{

public:

	CBSPGemApp();

	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()

};

extern CBSPGemApp theApp;