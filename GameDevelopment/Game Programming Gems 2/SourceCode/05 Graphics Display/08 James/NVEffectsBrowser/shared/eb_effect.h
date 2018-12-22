/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

// EBEffect.h: interface for the EBEffect class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __EBEFFECT_H
#define __EBEFFECT_H

#include <windows.h>

#include <d3d8.h>
#include <d3dx8.h>
#include <dxerr8.h>

#include <GL/gl.h>

#pragma warning(disable: 4786)
#include <string.h>

#include "eb_string.h"
#include "eb_timer.h"
#include "eb_property.h"
#include "eb_file.h"

////////////////////////////////////////////////////////////////////////////////
// EBEffect enums

typedef enum tagButtonID
{
	MOUSE_LEFTBUTTON = 0,
	MOUSE_RIGHTBUTTON = 1,
	MOUSE_MIDDLEBUTTON = 2
} eButtonID;

typedef enum tagEBGFXAPI
{
        GFXAPI_D3D = 0,
        GFXAPI_OPENGL = 1
} EBGFXAPI;

typedef enum tagEBSHADERTYPE
{
	SHADERTYPE_VERTEX = 0,
	SHADERTYPE_PIXEL = 1,
	SHADERTYPE_FORCEDWORD = 0xFFFFFFFF
} EBSHADERTYPE;

typedef enum tagEBKeyAction
{
	// Required
	EB_HELP = 0,		// Show help
	EB_WIREFRAME,		// toggle wireframe
	EB_RESET,			// Reset to start conditions

	// Reserved
	EB_QUIT,			// Quits - not used in effects browser
	EB_HUD,				// toggles HUD
	EB_CONSOLE,			// toggles console window
	EB_PAUSE,			// toggles Animation
	EB_ADD,				// Increase something
	EB_SUBTRACT,		// Decrease something
	EB_MULTIPLY,		// Multiply something
	EB_DIVIDE,			// Divide something
	EB_PAGEUP,			// Page up to move through options
	EB_PAGEDOWN,		// Page down to move through options
	EB_POINTS,			// sets point rendering mode

	// Suggested
	EB_CUBEMAP,			// cycles cubemaps			
	EB_TEXTURES,		// toggles textures
	EB_NORMALMAP,		// cycles normal maps
	EB_MIPMAP,			// toggles mipmapping
	EB_LIGHTING,		// toggles lighting

	// Not recognized
	EB_UNKNOWN = 0xFFFFFFFF
} eEBKeyAction;

#define EBEFFECT_DIRTY_PUBLICSTATE (1 << 0)
#define EBEFFECT_DIRTY_PIXELSHADERS (1 << 1)
#define EBEFFECT_DIRTY_VERTEXSHADERS (1 << 2)

#define EBEFFECT_VERSION 2

////////////////////////////////////////////////////////////////////////////////
// EBEffect class definition

class EBEffect : public EBPropertySet
{
public:
	EBEffect::EBEffect()
	: m_pD3DDev(NULL),
		m_strLastError(""),
		m_strEffectName("Change EffectName property"),
		m_strEffectLocation("Change EffectLocation property"),
		m_strEffectVertexShader("Change EffectVertexShader property"),
		m_strEffectPixelShader("Change EffectPixelShader property"),
		m_dwEffectDirtyFlags(0),
		m_dwEffectVersion(EBEFFECT_VERSION)
		{}

	virtual ~EBEffect()
		{}

	// Destroy this effect (deletes it from memory)	
	virtual void DestroyEffect()
		{ delete this; }

	// ------------------------------------------------------------
	// These methods should be overridden in each effect

	// This method informs the browser of which graphics API the effect supports
	//
	// It returns D3D by default so that all of the current D3D
	// effects will continue to function without code changes.
	//
	virtual EBGFXAPI API() const
		{ return GFXAPI_D3D; };

	// Must override for D3D effects
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
	  { return E_FAIL; };
	virtual HRESULT Initialize(LPDIRECT3DDEVICE8 pDev)
	  { return E_FAIL; };

	// Must override for OpenGL effects
	virtual HRESULT ConfirmDevice(PIXELFORMATDESCRIPTOR *pFD)
	  { return E_FAIL; };
	virtual HRESULT Initialize(HGLRC hRC)
	  { return E_FAIL; };

	// Must override for both D3D and OpenGL effects
	virtual HRESULT Free() = 0;
	virtual HRESULT Start() = 0;
	virtual HRESULT Tick(EBTimer* pTimer) = 0;

	// ------------------------------------------------------------
	// These methods may be overridden in each effect

	// May override for mouse/keyboard messages
	virtual void MouseMove(HWND hWnd, int x, int y)
		{ return; }
	virtual void MouseButton(HWND hWnd, eButtonID Button, bool bDown, int x, int y)
		{ return; }
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
		{ return; }

	// Window resizing callback.  The default implementation does two things.  It saves the window's
	// width, height and aspect in local member variables for use by derived classes.  By default it
	// also updates the graphics viewport for OpenGL.  If you wish to call the parent class but not
	// update the viewport, pass false in the fourth argument (currently unimplemented, see below).
	//
	// You are guaranteed to get at least one Resize call after Initialize() and before Tick().
	virtual void Resize(HWND hWnd, int w, int h, bool fUpdateViewport)
	{
		if ((w <= 0) || (h <= 0))
			return;
		
		m_iWidth  = (UINT) w;
		m_iHeight = (UINT) h;
		m_fAspect = ((float) w ) / ((float) h);
		
#if 0
		// XXX - Not yet implemented.
		//
		// This should be implemented after this code is migrated out of this header and into
		// its own CPP file.  Otherwise every effect must link against both OpenGL and D3D...
		// yuck!  Moving this (and some other code) out of this header file and into a DLL will
		// also mean that every effect does not have to include this code in its DLL.

		if (fUpdateViewport) {
			
			// OpenGL viewport
			if (API() == GFXAPI_OPENGL) {
				glViewport(0, 0, w, h);
			}

			if (API() == GFXAPI_D3D) {
				// XXX - What to do here?
			}
		}
#endif
	};

	// Update the properties for this effect (must call this base class version if you inherit from it)
	// The default properties listed below are the 'interface' to the effects browser
	// It's easy to extend the interface to the browser by exporting new properties from the EBEffect
	// and having the browser recognise them - this doesn't break the class interface between the two.
	virtual void UpdateProperties()
	{	
		// Discard existing properties
		EBPropertySet::UpdateProperties();
		
		// Add the effect name and last error properties
		AddProperty(new EBProperty("EffectName", OBJECT_MEMBER(m_strEffectName), EBTYPE_STRING_PROP, PROPFLAG_PRIVATE));
		AddProperty(new EBProperty("EffectLocation", OBJECT_MEMBER(m_strEffectLocation), EBTYPE_STRING_PROP, PROPFLAG_PRIVATE));
		
		m_pPixelShaderEnum = new EBEnumProperty("EffectPixelShader", OBJECT_MEMBER(m_strEffectPixelShader), EBTYPE_STRING_PROP, PROPFLAG_PRIVATE | PROPFLAG_CALLBACK);
		AddProperty(m_pPixelShaderEnum);
		
		m_pVertexShaderEnum = new EBEnumProperty("EffectVertexShader", OBJECT_MEMBER(m_strEffectVertexShader), EBTYPE_STRING_PROP, PROPFLAG_PRIVATE | PROPFLAG_CALLBACK);
		AddProperty(m_pVertexShaderEnum);
		
		AddProperty(new EBProperty("EffectVersion", OBJECT_MEMBER(m_dwEffectVersion), EBTYPE_DWORD_PROP, PROPFLAG_PRIVATE));
		AddProperty(new EBProperty("EffectDirtyFlags", OBJECT_MEMBER(m_dwEffectDirtyFlags), EBTYPE_DWORD_PROP, PROPFLAG_PRIVATE));
		
		AddProperty(new EBProperty("LastError", OBJECT_MEMBER(m_strLastError), EBTYPE_STRING_PROP, PROPFLAG_PRIVATE));
		
	};

	// Loads a .vso file and creates a vertex shader for it
	virtual HRESULT LoadAndCreateShader(const std::string& strFilePath, const DWORD* pDeclaration,
		                                DWORD Usage, EBSHADERTYPE ShaderType, DWORD* pHandle)
	{
		assert(m_pD3DDev);
		try	{
			HANDLE hFile;
			HRESULT hr;
			
			hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
			if(hFile == INVALID_HANDLE_VALUE) {
				m_strLastError = "Could not find file " + strFilePath;
				return E_FAIL;
			}
			
			DWORD dwFileSize = GetFileSize(hFile, NULL);
			
			const DWORD* pShader = (DWORD*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwFileSize);
			if (!pShader) {
				m_strLastError = "Failed to allocate memory to load shader " + strFilePath;
				return E_FAIL;
			}
			
			ReadFile(hFile, (void*)pShader, dwFileSize, &dwFileSize, NULL);
			
			CloseHandle(hFile);
			
			if (ShaderType == SHADERTYPE_VERTEX) {
				hr = m_pD3DDev->CreateVertexShader(pDeclaration, pShader, pHandle, Usage);
			} else if (ShaderType == SHADERTYPE_PIXEL) {
				hr = m_pD3DDev->CreatePixelShader(pShader, pHandle);
			}
			
			HeapFree(GetProcessHeap(), 0, (void*)pShader);
			
			if (FAILED(hr))	{
				m_strLastError = "Failed to create shader " + strFilePath + '\n'; 
				return E_FAIL;
			}
		} catch(...) {
			m_strLastError = "Error opening file " + strFilePath;
			return E_FAIL;
		}
		
		return S_OK;
	};
	
	// This function tries really hard to find the file you have asked for.
	// It looks in the current .dll's directory, it's media directory, 
	// the source process directory (nveffectsbrowser), and it's media directory.
	virtual std::string GetFilePath(const std::string& strFileName)
	{
		// This is just a convenient accessor to the real method
		return effect_api::GetFilePath(strFileName);
	}

	// Return an action from a key
	virtual eEBKeyAction TranslateEffectKey(DWORD dwKeyCode, DWORD nFlags, bool bDown)
	{
		if (!bDown)
			return EB_UNKNOWN;
		
		switch (dwKeyCode) {
		case 'H':
		case VK_F1:			return EB_HELP;
		case 'W':			return EB_WIREFRAME;
		case VK_HOME :
		case VK_NUMPAD7 :
		case '7' :			return EB_RESET;
		case VK_ESCAPE:		return EB_QUIT;
		case VK_F12:
		case VK_TAB:		return EB_HUD;
		case '`':
		case '¬':			return EB_CONSOLE;
		case VK_PAUSE:
		case VK_SPACE:		return EB_PAUSE;
		case VK_ADD:
		case 0xBB:
		case '+':
		case '=':			return EB_ADD;
		case VK_SUBTRACT:
		case 0xBD:
		case '-':
		case '_':			return EB_SUBTRACT;
		case VK_MULTIPLY:	return EB_MULTIPLY;
		case VK_DIVIDE:		return EB_DIVIDE;
		case VK_PRIOR:		return EB_PAGEUP;
		case VK_NEXT:		return EB_PAGEDOWN;
		case 'P':			return EB_POINTS;
		case 'C':			return EB_CUBEMAP;
		case 'T':			return EB_TEXTURES;
		case 'N':			return EB_NORMALMAP;
		case 'M':			return EB_MIPMAP;
		case 'L':			return EB_LIGHTING;
		default:			return EB_UNKNOWN;
		}
	};

protected:

	// Accessors for width/height/aspect of window to subclasses
	UINT  GetWndWidth() const  { return m_iWidth; };
	UINT  GetWndHeight() const { return m_iHeight; };
	float GetWndAspect() const { return m_fAspect; };

	UINT   m_iWidth, m_iHeight;
	float  m_fAspect;

	// This should technically not be hanging around in the EBEffect base classes, but to avoid
	// modifying every effect, we just leave it here.  This wastes memory when loading an OpenGL
	// effect...
	LPDIRECT3DDEVICE8 m_pD3DDev;

	EBString m_strLastError;			// A string holding the last error the effect had
	EBString m_strEffectName;			// A string holding the name of the effect
	EBString m_strEffectLocation;		// A string holding the location of the effect in the tree

	// Enumerated properties for pixel shaders
	EBEnumProperty* m_pPixelShaderEnum;
	EBEnumProperty* m_pVertexShaderEnum;

	EBString m_strEffectPixelShader;	// A string holding the current pixel shader
	EBString m_strEffectVertexShader;	// A string holding the current vertex shader

	DWORD m_dwEffectDirtyFlags;			// Flags to show dirty state.
	DWORD m_dwEffectVersion;			// A DWORD holding the version of this effect - Browser must be at least this to run it.
};

////////////////////////////////////////////////////////////////////////////////
// EBEffect function prototypes and macros

typedef EBEffect* (*LPCREATEEFFECT)(unsigned int);
typedef unsigned int (*LPGETEFFECTNUM)();

#define DECLARE_EFFECT_STATICS()															\
std::string effect_api::strStartPath ="";

#define DECLARE_EFFECT_MAIN()																\
DECLARE_EFFECT_STATICS()																	\
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)		\
{																							\
	switch (ul_reason_for_call)																\
	{																						\
		case DLL_PROCESS_ATTACH:															\
			{																				\
				std::string strProcessPath;													\
				strProcessPath.resize(MAX_PATH);											\
				DWORD dwReturnSize = GetModuleFileName((HINSTANCE)hModule, &strProcessPath[0], MAX_PATH);	\
				assert(dwReturnSize <= MAX_PATH); /*Should never happen*/									\
				effect_api::SetModulePath(strProcessPath);													\
			}																						\
		case DLL_THREAD_ATTACH:																		\
		case DLL_THREAD_DETACH:																		\
		case DLL_PROCESS_DETACH:																	\
			break;																					\
    }																								\
    return TRUE;																					\
}

#endif // __EBEffect_H

