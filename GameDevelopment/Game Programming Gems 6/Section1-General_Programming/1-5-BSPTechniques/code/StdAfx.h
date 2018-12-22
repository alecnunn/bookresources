/*
    Game Programming Gems: Vol 6, General Programming
    Gem:    BSP Techniques
    Author: Octavian Marius Chincisan
*/

#ifndef _STDAFX_H__
#define _STDAFX_H__

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
    #include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define	AKEY(k_)	(GetAsyncKeyState(k_) & 0x8000)
typedef	BYTE		RGBA[4];

extern RGBA	CLR_WHITE;
extern RGBA	CLR_GREY;
extern RGBA	CLR_PORTALS1;
extern RGBA	CLR_PORTALS;
extern RGBA	CLR_PORTALSLINE;


#endif // !_STDAFX_H__
