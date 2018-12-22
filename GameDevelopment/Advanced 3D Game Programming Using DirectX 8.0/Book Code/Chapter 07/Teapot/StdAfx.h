// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__9C89AC16_48BC_11D3_B2BB_00C04F685D9A__INCLUDED_)
#define AFX_STDAFX_H__9C89AC16_48BC_11D3_B2BB_00C04F685D9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define STRICT

// TODO: reference additional headers your program requires here

/**
 * warning C4786: <some extremely long name> : 
 * identifier was truncated to '255' characters in the 
 * debug information
 */
#pragma warning( disable : 4786 )  


// Windows Header Files:
#include <windows.h>
#include <mmsystem.h>

#include <d3dx8.h>
#include <d3d8.h>
#include <dsound.h>
#include <dinput.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>

// Local Header Files
#include "..\math3d\point3.h"
#include "..\math3d\matrix4.h"
#include "..\math3d\point3.h"
#include "..\math3d\mathd3d.h"
#include "..\math3d\plane3.h"
#include "..\math3d\polygon.h"
#include "..\math3d\tri.h"

#include "..\gamelib\GameGlobals.h"
#include "..\gamelib\GameErrors.h"
#include "..\gamelib\GameTypes.h"
#include "..\gamelib\File.h"
#include "..\gamelib\Application.h"
#include "..\gamelib\Window.h"
#include "..\gamelib\GraphicsLayer.h"
#include "..\gamelib\Model.h"
#include "..\gamelib\Keyboard.h"
#include "..\gamelib\Mouse.h"
#include "..\gamelib\InputLayer.h"
#include "..\gamelib\SoundLayer.h"
#include "..\gamelib\Sound.h"
#include "..\gameLib\DxHelper.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__9C89AC16_48BC_11D3_B2BB_00C04F685D9A__INCLUDED_)
