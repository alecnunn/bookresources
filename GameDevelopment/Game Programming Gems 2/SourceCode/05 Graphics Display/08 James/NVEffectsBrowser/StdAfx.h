/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__CC3B786B_C14A_4202_B481_62376FF6321E__INCLUDED_)
#define AFX_STDAFX_H__CC3B786B_C14A_4202_B481_62376FF6321E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#pragma warning(disable: 4786)
#include <list>
#pragma warning(disable: 4786)
#include <vector>
#pragma warning(disable: 4786)
#include <iterator>
#pragma warning(disable: 4786)
#include <map>
#pragma warning(disable: 4786)
#include <string>
#pragma warning(disable: 4786)
#include <stack>
#pragma warning(disable: 4786)
#include <assert.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <strstream>
#include <fstream>

#include <tchar.h>
#include <stdio.h>
#include <tchar.h>

#define SAFE_RELEASE(a) \
	if ((a)) (a)->Release(); (a) = NULL;

#define SAFE_DELETE(a) \
	if ((a)) delete(a); a = NULL;

#define SAFE_DELETE_ARRAY(a) \
	if ((a)) delete [] (a); a = NULL;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__CC3B786B_C14A_4202_B481_62376FF6321E__INCLUDED_)
