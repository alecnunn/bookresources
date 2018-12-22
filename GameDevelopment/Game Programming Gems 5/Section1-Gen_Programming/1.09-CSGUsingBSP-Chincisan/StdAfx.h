/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/


#ifndef __STDAFX_H__
#define __STDAFX_H__

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#pragma warning (disable: 4786)
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif                      // _AFX_NO_AFXCMN_SUPPORT
#include "baseos.h"
#include "TexRef.h"
#include <set>
typedef basic_string<char> tstring;
// Globals
extern REAL      GEpsilon;
#define	AKEY(k_) (GetAsyncKeyState(k_) & 0x8000)
typedef	BYTE	 RGBA[4];
extern REAL	     GGridSnap;
extern int       GMinDetDim;
extern int       GMaxDetDim;
extern int       GMinDetPolys;
extern	RGBA     CLR_NORMAL;
extern	RGBA     CLR_DARK;
extern	RGBA     CLR_CUT;
extern	RGBA	 CLR_WHITE;
extern	RGBA	 CLR_SELECT;
extern	RGBA	 CLR_SELBOX;
extern	RGBA	 CLR_CAMSTART;
extern	RGBA	 CLR_CAMEND;
extern	RGBA	 CLR_CAMFRUST;
extern	RGBA	 CLR_SCRSURFACE;
extern	RGBA	 CLR_CUT3DBRUSH;
extern	RGBA	 CLR_CUT3DBRUSHREV;
extern	RGBA	 CLR_PORTALS;
extern	RGBA	 CLR_PORTALS1;
extern	RGBA	 CLR_PORTALSLINE;
extern	RGBA	 CLR_DETAIL; 
#endif // !__STDAFX_H__









