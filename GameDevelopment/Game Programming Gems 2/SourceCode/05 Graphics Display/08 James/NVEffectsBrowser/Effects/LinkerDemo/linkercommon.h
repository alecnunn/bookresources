/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#ifndef __LINKERCOMMON_H
#define __LINKERCOMMON_H

typedef enum tageTexGenOptions
{
	TEXGEN_NONE = 0,
	TEXGEN_CAMERASPACEREFLECTION = 1,
	TEXGEN_BLINNBUMPREFLECTION = 2,
	TEXGEN_FORCE_DWORD = 0xFFFFFFFF
} eTexGenOptions;

typedef enum tageFogOptions
{
	FOG_NONE = 0,
	FOG_LINEAR_RANGE = 1
} eFogOptions;

typedef struct tagDialogSettings
{
	BOOL m_bDirty;

	DWORD m_dwNumPointLights;
	DWORD m_dwNumDirectionalLights;
	eTexGenOptions m_TexGen;
	eFogOptions m_Fog;
	BOOL m_bEyeSpace;
	BOOL m_bSpecular;
	BOOL m_bLocalViewer;
	BOOL m_bLighting;
	BOOL m_bTextures;
	BOOL m_bWhiteLight;
	BOOL m_bSpecularRDotL;

	BOOL m_bEnableBump;
	BOOL m_bEnableReflect;
	BOOL m_bEnableTextures;
	

} DialogSettings;

extern "C" 
{

#ifdef LINKERDIALOG
_declspec(dllexport) BOOL LinkerDialog_ShowDialog(bool bShow);
_declspec(dllexport) BOOL LinkerDialog_Initialise(HWND hWndParent);
_declspec(dllexport) BOOL LinkerDialog_Free();
_declspec(dllexport) BOOL LinkerDialog_GetSettings(DialogSettings* pSettings);
_declspec(dllexport) BOOL LinkerDialog_SetSettings(DialogSettings* pSettings);

#else

typedef BOOL (*LPSHOWDIALOG)(bool);
typedef BOOL (*LPINITIALISE)(HWND);
typedef BOOL (*LPFREE)();
typedef BOOL (*LPGETSETTINGS)(DialogSettings*);
typedef BOOL (*LPSETSETTINGS)(DialogSettings*);
typedef struct tagLinkerDialogFunc
{
	LPSHOWDIALOG ShowDialog;
	LPINITIALISE Initialise;
	LPFREE Free;
	LPGETSETTINGS GetSettings;
	LPSETSETTINGS SetSettings;
} LinkerDialogFunc;

#endif // LINKERDIALOG

} // extern "C"

#endif // __LINKERCOMMON_H