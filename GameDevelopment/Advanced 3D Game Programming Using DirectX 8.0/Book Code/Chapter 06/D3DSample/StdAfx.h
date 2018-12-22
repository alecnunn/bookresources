// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// disable the annoying 255 character limit warning
#pragma warning( disable : 4786 )

#include <windows.h>

// Local Header Files
#define D3D_OVERLOADS

#include <d3d8.h>
#include <d3dx8.h>

#include "..\gamelib\dxhelper.h"

#include "..\math3d\point3.h"
#include "..\math3d\matrix4.h"
#include "..\math3d\mathD3D.h"
#include "..\math3d\color3.h"

#include "..\gamelib\GameErrors.h"
#include "..\gamelib\GameTypes.h"
#include "..\gamelib\GameGlobals.h"
#include "..\gamelib\file.h"
#include "..\gamelib\model.h"
#include "..\gamelib\GraphicsLayer.h"
#include "..\gamelib\Application.h"
#include "..\gamelib\Window.h"
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
