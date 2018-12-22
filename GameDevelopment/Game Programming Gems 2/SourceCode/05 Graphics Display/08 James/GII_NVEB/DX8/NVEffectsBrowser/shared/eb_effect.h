/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
//
// EBEffect.h: interface for the EBEffect class
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __EBEFFECT_H
#define __EBEFFECT_H

#include <windows.h>

#include <d3d8.h>
#include <d3dx8.h>
#include <dxerr8.h>

#include <GL/gl.h>

#pragma warning(disable: 4786)
#include <string.h>
#include <stdarg.h>

#include "eb_string.h"
#include "eb_timer.h"
#include "eb_property.h"
#include "eb_file.h"


////////////////////////////////////////////////////////////////////////////////
// EBEffect API versioning
//
// These defines declare the current major and minor version of the effect API.
// When changing the EBEffect class (the API), if your change is backward
// compatible, only increment the minor version number.  If your change requires
// that effects compiled against the prior API be recompiled or updated, zero
// the minor version number and increment the major version number.  That is,
// if an effect is compiled with API version X.Y, then it will run with browsers
// supporting any API numbered X.Z where Y <= Z.  Also remember that you needn't
// change the API at all, if your change will not disrupt the EBEffect interface
// at all.
//
// NOTE: The versioning information will be 'burned into' each DLL as it is
// compiled (see __DECLARE_EFFECT_VERSION() below).
//
// This versioning system outdates the old EBEFFECT_VERSION system.  This
// version provides the ability update the API and explicitly retain or remove
// backwards compatibility.  It also changes the version to be associated with
// the entire DLL rather than each individual effect defined therein.  This
// allows for more optimal DLL loading in the browser.  Finally, this
// implementation is not upset by changes to the EBEffect class structure or
// virtual function table.

#define EBEFFECT_APIVERSION_MAJOR 6
#define EBEFFECT_APIVERSION_MINOR 0


////////////////////////////////////////////////////////////////////////////////
// EBEffect public function prototypes and macros
//
// Each effect deriving from EBEffect must include the DECLARE_EFFECT_MAIN()
// macro.  This macro defines some mandatory exports and the DLL's DllMain()
// function.  Additionally, each effect must export "C" style functions named
// 'CreateEffect' and 'GetEffectNum' that match the LPCREATEEFFECT and
// LPGETEFFECTNUM prototypes respectively.  The four macros named below can be
// used to simplify this process.

typedef class EBEffect* (*LPCREATEEFFECT)(unsigned int);
typedef unsigned int (*LPGETEFFECTNUM)();

#define DECLARE_EFFECT_MAIN()				\
	__DECLARE_EFFECT_STATICS()				\
	__DECLARE_EFFECT_APIVERSION()			\
	__DECLARE_EFFECT_SCANFUNCS()			\
	__DECLARE_EFFECT_DLLMAIN()


// These macros provide a simple way to define the list of effects that can
// be created by your DLL.  These macros can be used as follows:
//
// DECLARE_EFFECT_COUNT(3)
// DECLARE_EFFECT_CREATE_BEG()
// DECLARE_EFFECT_CREATE(0, MyFirstEffectClass())
// DECLARE_EFFECT_CREATE(1, MyOtherEffectClass(ARG_EFFECT_ONE, ARG_EFFECT_TWO))
// DECLARE_EFFECT_CREATE(2, MyOtherEffectClass(ARG_EFFECT))
// DECLARE_EFFECT_CREATE_END()
//
//    OR
//
// DECLARE_EFFECT_JUST_ONE(MyOnlyEffect(Arg1, Arg2, Arg3))
//
// You can, of course, simply declare the functions yourself.

#define DECLARE_EFFECT_COUNT(num)			\
extern "C" {								\
	__declspec(dllexport) unsigned int		\
	GetNumEffects()							\
	{										\
		return num;							\
	}										\
}

#define DECLARE_EFFECT_CREATE_BEG()			\
extern "C" {								\
	__declspec(dllexport) EBEffect*			\
	CreateEffect(unsigned int num)			\
	{										\
		switch (num) {

#define DECLARE_EFFECT_CREATE(n,c)			\
		case n: return new c; break;

#define DECLARE_EFFECT_CREATE_END()			\
		}									\
		return NULL;						\
	}										\
}

#define DECLARE_EFFECT_JUST_ONE(c)			\
	DECLARE_EFFECT_COUNT(1)					\
	DECLARE_EFFECT_CREATE_BEG()				\
	DECLARE_EFFECT_CREATE(0,c)				\
	DECLARE_EFFECT_CREATE_END()


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

#define EBEFFECT_DIRTY_PUBLICSTATE   (1 << 0)
#define EBEFFECT_DIRTY_PIXELSHADERS  (1 << 1)
#define EBEFFECT_DIRTY_VERTEXSHADERS (1 << 2)
#define EBEFFECT_DIRTY_REGCOMBSTATE  (1 << 3)

// Defined below
//
extern "C" __declspec(dllexport) void GetEffectApiVersion(unsigned int*, unsigned int*);


////////////////////////////////////////////////////////////////////////////////
// EBEffect class definition

class EBEffect : public EBPropertySet
{
public:
	EBEffect::EBEffect()
	  : m_pD3DDev(NULL),
		m_strEffectName("Change EffectName property"),
		m_strEffectLocation("Change EffectLocation property"),
		m_strEffectVertexShader("Change EffectVertexShader property"),
		m_strEffectPixelShader("Change EffectPixelShader property"),
		m_strEffectVertexProgram("Change EffectVertexProgram property"),
		m_strEffectTextureShader("Change EffectTextureShader property"),
		m_strEffectRegCombState("Change EffectRegisterCombiner property"),
		m_strLastError(""),
		m_nAboutLines(0), 
		m_pAboutLabelEnum(NULL), m_pAboutValueEnum(NULL), m_pAboutLinkEnum(NULL),
		m_idAboutIcon(-1),
		m_dwEffectDirtyFlags(0)
	{
		GetEffectApiVersion(&m_apiMajorVersion, &m_apiMinorVersion);
		m_hModule = hModule;
	}

	virtual ~EBEffect()
		{}

	// Destroy this effect (deletes it from memory)	
	virtual void DestroyEffect()
		{ delete this; }

	//////////////////////////////////////////////////////////////////////////////
	// These methods must be overridden in each effect
	//////////////////////////////////////////////////////////////////////////////
public:

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

	//////////////////////////////////////////////////////////////////////////////
	// These methods may be overridden in each effect
	//////////////////////////////////////////////////////////////////////////////
public:

	// May override for mouse/keyboard messages
	virtual void MouseMove(HWND hWnd, int x, int y)
		{ return; }
	virtual void MouseButton(HWND hWnd, eButtonID Button, bool bDown, int x, int y)
		{ return; }

	// Override either one of these, not both
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown, int x, int y)
		{ Keyboard(dwKey, nFlags, bDown); }
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
		{ return; }

	// Window resizing callback.  The default implementation does two things.  It 
	// saves the window's width, height and aspect in local member variables for
	// use by derived classes.  By default it also updates the graphics viewport
	// for OpenGL.  If you wish to call the parent class but not update the
	// viewport, pass false in the fourth argument (currently unimplemented, see
	// below).
	//
	// You are guaranteed to get at least one Resize call after Initialize() and
	// before Tick().
	//
	virtual void Resize(HWND hWnd, int w, int h, bool fUpdateViewport)
	{
		if ((w <= 0) || (h <= 0))
			return;
		
		m_iWidth  = (UINT) w;
		m_iHeight = (UINT) h;
		m_fAspect = ((float) w ) / ((float) h);
	};

	// Update the properties for this effect (must call this base class version if
	// you inherit from it).  The default properties listed below are the 'interface'
	// to the effects browser.  It's easy to extend the interface to the browser by
	// exporting new properties from the EBEffect and having the browser recognise
	// them - this doesn't break the class interface between the two.
	//
	virtual void UpdateProperties()
	{	
		// Discard existing properties
		EBPropertySet::UpdateProperties();
		m_nAboutLines = 0;
		
		// Add the effect name, location, and effect version (not the API version)
		AddProperty(new EBProperty("EffectName", OBJECT_MEMBER(m_strEffectName), 
			EBTYPE_STRING_PROP, PROPFLAG_PRIVATE));
		AddProperty(new EBProperty("EffectLocation", OBJECT_MEMBER(m_strEffectLocation), 
			EBTYPE_STRING_PROP, PROPFLAG_PRIVATE));
		AddProperty(new EBProperty("EffectVersion", OBJECT_MEMBER(m_strEffectVersion), 
			EBTYPE_STRING_PROP, PROPFLAG_PRIVATE));
		AddProperty(new EBProperty("EffectAboutIcon", OBJECT_MEMBER(m_idAboutIcon), 
			EBTYPE_DWORD_PROP, PROPFLAG_PRIVATE));

		// Add the administrative properties
		AddProperty(new EBProperty("LastError", OBJECT_MEMBER(m_strLastError),
			EBTYPE_STRING_PROP, PROPFLAG_PRIVATE));
		AddProperty(new EBProperty("EffectDirtyFlags", OBJECT_MEMBER(m_dwEffectDirtyFlags),
			EBTYPE_DWORD_PROP, PROPFLAG_PRIVATE));

		// Add the additional effect About information (optionally can be set with SetAboutInfo())
		m_pAboutLabelEnum = new EBEnumProperty("EffectAboutLabel",
			/* no default member */ (DWORD) -1, EBTYPE_STRING_PROP, PROPFLAG_PRIVATE);
		AddProperty(m_pAboutLabelEnum);
		m_pAboutValueEnum = new EBEnumProperty("EffectAboutValue",
			/* no default member */ (DWORD) -1, EBTYPE_STRING_PROP, PROPFLAG_PRIVATE);
		AddProperty(m_pAboutValueEnum);
		m_pAboutLinkEnum  = new EBEnumProperty("EffectAboutLink",
			/* no default member */ (DWORD) -1, EBTYPE_STRING_PROP, PROPFLAG_PRIVATE);
		AddProperty(m_pAboutLinkEnum);

		// D3D vertex/pixel shader info
		m_pVertexShaderEnum = new EBEnumProperty("EffectVertexShader", 
			OBJECT_MEMBER(m_strEffectVertexShader),	EBTYPE_STRING_PROP,
			PROPFLAG_PRIVATE | PROPFLAG_CALLBACK);
		AddProperty(m_pVertexShaderEnum);
		m_pPixelShaderEnum = new EBEnumProperty("EffectPixelShader", 
			OBJECT_MEMBER(m_strEffectPixelShader), EBTYPE_STRING_PROP, 
			PROPFLAG_PRIVATE | PROPFLAG_CALLBACK);
		AddProperty(m_pPixelShaderEnum);

		// OpenGL VP/TS/RC shader info
		m_pVertexProgramEnum = new EBEnumProperty("EffectVertexProgram", 
			OBJECT_MEMBER(m_strEffectVertexProgram), EBTYPE_STRING_PROP,
			PROPFLAG_PRIVATE | PROPFLAG_CALLBACK);
		AddProperty(m_pVertexProgramEnum);
		m_pTextureShaderEnum = new EBEnumProperty("EffectTextureShader", 
			OBJECT_MEMBER(m_strEffectTextureShader), EBTYPE_STRING_PROP,
			PROPFLAG_PRIVATE | PROPFLAG_CALLBACK);
		AddProperty(m_pTextureShaderEnum);
		m_pRegCombStateEnum = new EBEnumProperty("EffectRegisterCombiner", 
			OBJECT_MEMBER(m_strEffectRegCombState), EBTYPE_STRING_PROP,
			PROPFLAG_PRIVATE | PROPFLAG_CALLBACK);
		AddProperty(m_pRegCombStateEnum);
	};


	//////////////////////////////////////////////////////////////////////////////
	// These methods may be used to setup state in the Effect more easily
	//////////////////////////////////////////////////////////////////////////////
protected:

	// Sets up the effect's About dialog information
	//
	// The current effect About dialog contains up to three lines.  Each call
	// to this function defines one of these lines.  NULL may be legally passed
	// as the label and/or link argument.  If a label and value are both
	// specified the line will be displayed as:
	//
	//   Label:    Value
	// 
	// If just a label is provided, it will be displayed centered on the line.
	// Additionally, in either case, if a link is provided, the value text will
	// be linked to the provided URL.
	//
	// For example, an effect might say:
	//
	//  m_strEffectName     = "SpookyEffect";
	//  m_strEffectLocation = "Foobar Inc.\\Spooky Effect";
	//  m_strEffectVersion  = "1.1a5";
	//  
	//  SetAboutInfo(NULL,          _T("My Effect Page"), _T("http://www.doe.com/john/spookyeffect.html"));
	//  SetAboutInfo(_T("Author"),  _T("John Doe"), _T("http://www.doe.com/john"));
	//  SetAboutInfo(_T("Company"), _T("Foobar Inc."));
	//
	// And the about dialog would be displayed as:
	//
	//  +================================+ 
	//  |                                |
	//  |  SpookyEffect (version 1.1a5)  |
	//  |    (Effect API version 2.0)    |
	//  |                                |
	//  |            *******             |
	//  |            *******             |
	//  |            *******             |
	//  |                                |
	//  |        My Effect Page          |
	//  |  Author:      John Doe         |
	//  |  Company:     Foobar Inc.      |
	//  |                                |
	//  |              O K               |
	//  |                                |
	//  +================================+ 
	//
	// NOTE: the dialog size is not dynamic.  Do not make your labels/values 
	// too long!
	//
	virtual HRESULT SetAboutInfo(LPCTSTR lpLabel, LPCTSTR lpValue, LPCTSTR lpLink=NULL)
	{
		// Make sure that the properties are updated first
		if (!m_pAboutLabelEnum)
			return E_FAIL;

		if (!lpLabel) lpLabel = _T("");
		if (!lpLink)  lpLink  = _T("");

		// Make sure that we don't have too many lines...
		if (m_nAboutLines++ >= 3)
			return E_FAIL;

		EBString strLabel = lpLabel;
		EBString strValue = lpValue;
		EBString strLink  = lpLink;

		m_pAboutLabelEnum->AddEnumerant(new EBEnumValue(m_pAboutLabelEnum, "About Label",
														strLabel, EBTYPE_STRING_PROP));
		m_pAboutValueEnum->AddEnumerant(new EBEnumValue(m_pAboutValueEnum, "About Value",
														strValue, EBTYPE_STRING_PROP));
		m_pAboutLinkEnum->AddEnumerant(new EBEnumValue(m_pAboutLinkEnum, "About Link",
														strLink, EBTYPE_STRING_PROP));

		return S_OK;
	}

	// Set the default pane that will first be selected when this effect is displayed.
	// The title should match the title of one of the shaders added in a call to
	// AddShaderCodeFile() or AddShaderCodeBuffer().
	//
	virtual HRESULT SetDefaultCodePane(EBSHADERTYPE ShaderType, const char *strTitle)
	{
		switch (ShaderType) {
		case SHADERTYPE_VERTEXSHADER:   m_strEffectVertexShader  = strTitle;  break;
		case SHADERTYPE_PIXELSHADER:    m_strEffectPixelShader   = strTitle;  break;
		case SHADERTYPE_VERTEXPROG:     m_strEffectVertexProgram = strTitle;  break;
		case SHADERTYPE_TEXTURESHADER:  m_strEffectTextureShader = strTitle;  break;
		case SHADERTYPE_REGCOMBSTATE:   m_strEffectRegCombState  = strTitle;  break;
		default:  return E_FAIL;
		}
		return S_OK;
	}

	// Add a new shader to the list for this effect.  Provide the type of shader, a text
	// title for the tab control, and a file name containing the shader code itself.
	//
	virtual HRESULT AddShaderCodeFile(EBSHADERTYPE ShaderType, const char *strTitle, const char *strFile)
	{
		EBEnumProperty *pEnumProp = NULL;
		switch (ShaderType) {
		case SHADERTYPE_VERTEXSHADER:   pEnumProp = m_pVertexShaderEnum;   break;
		case SHADERTYPE_PIXELSHADER:    pEnumProp = m_pPixelShaderEnum;    break;
		case SHADERTYPE_VERTEXPROG:     pEnumProp = m_pVertexProgramEnum;  break;
		case SHADERTYPE_TEXTURESHADER:  pEnumProp = m_pTextureShaderEnum;  break;
		case SHADERTYPE_REGCOMBSTATE:   pEnumProp = m_pRegCombStateEnum;   break;
		default:  return E_FAIL;
		}
		if (!pEnumProp)
			return E_FAIL;

		EBEnumValue *pValue = new EBEnumValue(pEnumProp, strTitle, GetFilePath(strFile), EBTYPE_STRING_PROP);
		pEnumProp->AddEnumerant(pValue);

		return S_OK;
	}

	// Add a new shader to the list for this effect.  Provide the type of shader, a text
	// title for the tab control, and the text of the shader itself.
	//
	virtual HRESULT AddShaderCodeBuffer(EBSHADERTYPE ShaderType, const char *strTitle, const char *strBuffer)
	{
		EBEnumProperty *pEnumProp = NULL;
		switch (ShaderType) {
		case SHADERTYPE_VERTEXSHADER:   pEnumProp = m_pVertexShaderEnum;   break;
		case SHADERTYPE_PIXELSHADER:    pEnumProp = m_pPixelShaderEnum;    break;
		case SHADERTYPE_VERTEXPROG:     pEnumProp = m_pVertexProgramEnum;  break;
		case SHADERTYPE_TEXTURESHADER:  pEnumProp = m_pTextureShaderEnum;  break;
		case SHADERTYPE_REGCOMBSTATE:   pEnumProp = m_pRegCombStateEnum;   break;
		default:  return E_FAIL;
		}
		if (!pEnumProp)
			return E_FAIL;

		// Prepend to a '-' to signify that this is not a file name, but the code itself...
		std::string strCode = "-";
		strCode += strBuffer;

		EBEnumValue *pValue = new EBEnumValue(pEnumProp, strTitle, strCode, EBTYPE_STRING_PROP);
		pEnumProp->AddEnumerant(pValue);

		return S_OK;
	}


	// This method can be used to translate a key code into an action
	//
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

	
	//////////////////////////////////////////////////////////////////////////////
	// These methods should not be changed
	// They are public for convenient access.
	//////////////////////////////////////////////////////////////////////////////
public:

	// Methods to get the DLL location
	HMODULE GetModuleHandle() { return m_hModule; }
	static HMODULE hModule;

	// API version related methods
	unsigned int GetEffectApiMajorVersion() { return m_apiMajorVersion; }
	unsigned int GetEffectApiMinorVersion() { return m_apiMinorVersion; }

protected:
	// Load a .vso or .pso file and create a D3D vertex or pixel shader for it
	// Moved back to protected as there are global funcs for this
	// effect_api::LoadAndCreateShader( pDev, ..);
	virtual HRESULT LoadAndCreateShader(const std::string& strFilePath, const DWORD* pDeclaration,
		                                DWORD Usage, EBSHADERTYPE ShaderType, DWORD* pHandle)
	{
		HRESULT hr;
		hr = effect_api::LoadAndCreateShader(m_pD3DDev, strFilePath, pDeclaration, Usage, ShaderType, pHandle);
		m_strLastError = effect_api::strLastError;
		return hr;
	};

	// This function tries really hard to find the file you have asked for.
	// It looks in the current .dll's directory, it's media directory, 
	// the source process directory (nveffectsbrowser), and it's media directory.
	// ** 
	// There is also global function for this:
	// namespace effect_api::GetFilePath(..);
	//
	virtual std::string GetFilePath(const std::string& strFileName)
	{
		// This is just a convenient accessor to the real method
		return effect_api::GetFilePath(strFileName);
	}


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

	EBString m_strEffectName;			// A string holding the name of the effect
	EBString m_strEffectLocation;		// A string holding the location of the effect in the tree
	EBString m_strEffectLink;			// A string holding the URL link for this effect
	EBString m_strEffectVersion;		// A string holding the version of this effect
										// NOTE: this is not the Effect API version (see above)
	DWORD    m_idAboutIcon;             // The ID of the icon for the about box...

	EBString m_strLastError;			// A string holding the last error the effect had

	// Properties for About dialog
	int m_nAboutLines;
	EBEnumProperty *m_pAboutLabelEnum;
	EBEnumProperty *m_pAboutValueEnum;
	EBEnumProperty *m_pAboutLinkEnum;
	
	// Enumerated properties for pixel shaders
	EBEnumProperty* m_pVertexShaderEnum;
	EBEnumProperty* m_pPixelShaderEnum;
	EBEnumProperty* m_pVertexProgramEnum;
	EBEnumProperty* m_pTextureShaderEnum;
	EBEnumProperty* m_pRegCombStateEnum;

	EBString m_strEffectVertexShader;		// A string holding the current vertex shader
	EBString m_strEffectPixelShader;		// A string holding the current pixel shader
	EBString m_strEffectVertexProgram;		// A string holding the current vertex program
	EBString m_strEffectTextureShader;		// A string holding the current texture shader
	EBString m_strEffectRegCombState;		// A string holding the current register combiner

	DWORD m_dwEffectDirtyFlags;			// Flags to show dirty state.

	HMODULE      m_hModule;
	unsigned int m_apiMajorVersion;
	unsigned int m_apiMinorVersion;
};


////////////////////////////////////////////////////////////////////////////////
// EBEffect private prototypes and macros
//
// These prototypes and macros are used internally or by the browser.  Effect
// developer's shouldn't need to worry about them.

typedef void (*LPGETAPIVERSION)(unsigned int*, unsigned int*);
typedef void (*LPSETSCANOPTS)(bool, const EBString&);
typedef void (*LPGETSCANOPTS)(bool&, EBString&);

#define __DECLARE_EFFECT_STATICS()											\
bool effect_api::Rescanning;												\
EBString    effect_api::EffectState = "";									\
std::string effect_api::strStartPath = "";									\
HMODULE EBEffect::hModule;													\
std::string effect_api::strLastError = "";

#define __DECLARE_EFFECT_SCANFUNCS()										\
extern "C" {																\
	__declspec(dllexport) void												\
	SetScanOpts(bool rescanning, const EBString &state)						\
	{																		\
		effect_api::Rescanning = rescanning;								\
		effect_api::EffectState = state;									\
	}																		\
	__declspec(dllexport) void												\
	GetScanOpts(bool &rescanning, EBString &state)							\
	{																		\
		rescanning = effect_api::Rescanning;								\
		state = effect_api::EffectState;									\
	}																		\
}

#define __DECLARE_EFFECT_APIVERSION()										\
extern "C" {																\
	__declspec(dllexport) void												\
	GetEffectApiVersion(unsigned int *major, unsigned int *minor)			\
	{																		\
		*major = EBEFFECT_APIVERSION_MAJOR;									\
		*minor = EBEFFECT_APIVERSION_MINOR;									\
	}																		\
}

#define __DECLARE_EFFECT_DLLMAIN()											\
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ulReason, LPVOID lpReserved)	\
{																			\
	switch (ulReason)														\
	{																		\
		case DLL_PROCESS_ATTACH:											\
			{																\
				std::string strProcessPath;									\
				strProcessPath.resize(MAX_PATH);							\
				DWORD dwReturnSize = GetModuleFileName((HINSTANCE) hModule,	\
													   &strProcessPath[0],	\
													   MAX_PATH);			\
				assert(dwReturnSize <= MAX_PATH); /*Should never happen*/	\
				effect_api::SetModulePath(strProcessPath);					\
																			\
				EBEffect::hModule = (HMODULE) hModule;						\
			}																\
		case DLL_THREAD_ATTACH:												\
		case DLL_THREAD_DETACH:												\
		case DLL_PROCESS_DETACH:											\
			break;															\
    }																		\
    return TRUE;															\
}

#endif // __EBEffect_H

