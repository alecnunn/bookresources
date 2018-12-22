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


// Network header files (must be included before windows.h)
#include "..\NetLib\MTUDP.h"

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
#include "..\math3d\point3.h"
#include "..\math3d\matrix4.h"
#include "..\math3d\point3.h"
#include "..\math3d\mathd3d.h"
#include "..\math3d\plane3.h"
#include "..\math3d\polygon.h"
#include "..\math3d\tri.h"

#include "..\gamelib\GameGlobals.h"
#include "..\gamelib\GameErrors.h"
#include "..\gamelib\GameInterfaces.h"
#include "..\gamelib\GameTypes.h"
#include "..\gamelib\DxHelper.h"

#include "..\NetLib\cDataExtract.h"
#include "..\NetLib\cDataCompress.h"
#include "..\NetLib\cNetError.h"

#include "..\GameCommon\gamecell.h"
#include "..\GameCommon\gameworld.h"
#include "..\GameCommon\MsgDaemon.h"
#include "..\GameCommon\Ents\GamePlayerEnt.h"
#include "..\GameCommon\Ents\GameSpawnEnt.h"
#include "..\GameCommon\Ents\GameParaEnt.h"
#include "..\GameCommon\Ents\GameProjEnt.h"
#include "..\GameCommon\NetMsgs\NetMessages.h"


// Local Header Files

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
