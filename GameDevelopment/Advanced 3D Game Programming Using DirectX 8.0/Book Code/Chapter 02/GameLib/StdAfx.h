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

// disable the annoying 255 character limit warning
#pragma warning( disable : 4786 )

// Windows Header Files:
#include <windows.h>
#include <mmsystem.h>



#include <d3d8.h>
#include <d3dx8.h>
#include <dsound.h>
#include <dinput.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>

// Local Header Files
#include "GameGlobals.h"
#include "GameErrors.h"
#include "GameTypes.h"
#include "File.h"

#include "Application.h"
#include "Window.h"
#include "GraphicsLayer.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "InputLayer.h"
#include "SoundLayer.h"
#include "wavread.h"
#include "Sound.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__9C89AC16_48BC_11D3_B2BB_00C04F685D9A__INCLUDED_)
