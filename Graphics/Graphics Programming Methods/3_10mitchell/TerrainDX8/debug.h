// GPM - Pixel Shader Optimizations for Terrain Rendering Demo
// Kenny Mitchell
// Copyright Electronic Arts 2002, 2003

#ifndef DEBUG_H
#define DEBUG_H

#ifndef _D3D8_H_
#include <d3d8.h>
#endif

#ifndef __D3DX8_H__
#include <d3dx8.h>
#endif

#ifndef __DINPUT_INCLUDED__
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#endif

#ifndef _INC_STDIO
#include <stdio.h>
#endif

// debug failure handling
inline void Fail_Message(char* msg, char* file, int line)
{
	char str[255];
	sprintf(str,"%s in %s at line %d\n", msg, file, line);
	OutputDebugString(str);
	DebugBreak();
}
#define FAILMSG(msg) if (FAILED(result)) { Fail_Message(msg,__FILE__,__LINE__); }

// used in fog set render states
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

// safe deallocation macros
#define Safe_Release(obj) if (obj) { obj->Release(); obj=NULL; }
#define Safe_Delete(obj) if (obj) { delete obj; obj=NULL; }
#define Safe_Delete_Array(obj) if (obj) { delete[] obj; obj=NULL; }

// d3d device access
extern LPDIRECT3DDEVICE8 DEV;
extern HWND hWnd;

#endif