/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__4D30C558_0A7A_11D5_BA9B_000000000000__INCLUDED_)
#define AFX_STDAFX_H__4D30C558_0A7A_11D5_BA9B_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
//#include <afxdisp.h>        // MFC OLE automation classes
//#ifndef _AFX_NO_AFXCMN_SUPPORT
//#include <afxcmn.h>			// MFC support for Windows Common Controls
//#endif // _AFX_NO_AFXCMN_SUPPORT

// I want to have exception handling disabled, and micro threads practically
// require it. The STL expects exceptions to be enabled, and complains if
// they are disabled. This will only cause a problem if something happens
// that would trigger an exception - in which case I'm SOL anyway.
// The only thing that should throw an exception is out of memory, and I
// don't plan to run out of memory.
#pragma warning(disable : 4530)

// Disable the STL-in-use warning.
#pragma warning(disable : 4786)

// My tests have shown that in STL intensive applications it is very beneficial
// to include a couple of STL header files from stdafx.h. It doesn't really
// matter what ones, since they bring in a lot of shared stuff, but my
// favourites are vector and string. The code should still compile with
// these two includes removed.
#include <vector>
#include <string>

// Bring in the core stuff that I always want available.
#include "core/core.h"

// Make sure I don't accidentally use functions that might make my game
// 'unpredictable'.
#define	rand "Don't use rand() - use the RandomNumber class instead"
#define	GetKeyState			Please do not use this function
#define	GetAsyncKeyState	Please do not use this function

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__4D30C558_0A7A_11D5_BA9B_000000000000__INCLUDED_)
