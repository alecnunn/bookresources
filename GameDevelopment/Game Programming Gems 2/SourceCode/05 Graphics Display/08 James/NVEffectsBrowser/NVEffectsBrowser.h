/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
// NVEffectsBrowser.h : main header file for the NVEFFECTSBROWSER application
//

#if !defined(AFX_NVEFFECTSBROWSER_H__F8B71024_0582_450B_80BA_19057862B6C6__INCLUDED_)
#define AFX_NVEFFECTSBROWSER_H__F8B71024_0582_450B_80BA_19057862B6C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

// Common header for nvidia bits and pieces
#include "shared\eb_effect.h"
#include "nvmovie.h"
#include "nvfilesearch.h"
#include "nvdebug.h"

#include "listvwex.h"

#include "RenderView.h"
#include "RenderD3DWnd.h"
#include "RenderOGLWnd.h"
#include "ProgramPage.h"
#include "ProgramSheet.h"
#include "ProgramFormView.h"
#include "TreeView.h"

#include "doc.h"


/////////////////////////////////////////////////////////////////////////////
// CNVEffects specific return codes

#define _NVEFF_FAC   0x356

#define NVEFF_ERR_NOTINITIALIZED   MAKE_HRESULT( 1, _NVEFF_FAC, 0x0001 )


/////////////////////////////////////////////////////////////////////////////
// CNVEffects:
// See NVEffectsBrowser.cpp for the implementation of this class
//

class CNVEffects : public CWinApp
{
public:
	CNVEffects();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNVEffects)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual BOOL InitApplication();
	virtual int ExitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CNVEffects)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	D3DDEVTYPE GetDeviceType() { return m_DevType; }
	void SetDeviceType(D3DDEVTYPE DevType) { m_DevType = DevType; }
	bool GetExpandAll() { return m_bExpandAll; }
	void SetExpandAll(bool bSet) { m_bExpandAll = bSet; }
	bool GetForceSoftwareVP() { return m_bForceSoftwareVP; }
	void SetForceSoftwareVP(bool bForce) { m_bForceSoftwareVP = bForce; }
	std::string GetFilePath(const std::string& strFileName) 
	{
		HMODULE hProcess = GetModuleHandle(NULL);
		std::string strProcessPath;
		strProcessPath.resize(MAX_PATH);
		DWORD dwReturnSize = GetModuleFileName((HINSTANCE)hProcess, &strProcessPath[0], MAX_PATH);
		assert(dwReturnSize <= MAX_PATH); // Should never happen
		std::string::size_type Pos = strProcessPath.find_last_of("\\", strProcessPath.size());
		if (Pos != strProcessPath.npos)
		{
			// Check the root directory of the running process
			return (strProcessPath.substr(0, Pos) + "\\" + strFileName);
		}
		
		return strFileName;
	}

	DWORD GetVersion() { return EBEFFECT_VERSION; }
private:
	D3DDEVTYPE m_DevType;
	bool m_bForceSoftwareVP;
	bool m_bExpandAll;

};

extern CNVEffects theApp;

/////////////////////////////////////////////////////////////////////////////

// Useful macros
#define CHECK_RANGE(val, lower, upper)	\
{										\
	if (lower != -1)					\
	{									\
		if (val < lower)				\
			val = lower;				\
	}									\
	if (upper != -1)					\
	{									\
		if (val > upper)				\
			val = upper;				\
	}									\
}



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NVEFFECTSBROWSER_H__F8B71024_0582_450B_80BA_19057862B6C6__INCLUDED_)
