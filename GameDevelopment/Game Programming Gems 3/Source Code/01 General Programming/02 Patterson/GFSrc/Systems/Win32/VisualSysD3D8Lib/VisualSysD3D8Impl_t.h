/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// VisualSysD3D8Impl_t.h: interface for the VisualSysD3D8Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VISUALSYSD3D8IMPL_T_H__94343A96_DE45_4DB4_8EF7_C8BEB2BC2E28__INCLUDED_)
#define AFX_VISUALSYSD3D8IMPL_T_H__94343A96_DE45_4DB4_8EF7_C8BEB2BC2E28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Systems/VisualSys_t.h"
#include <d3d8.h>
#include <d3dx8.h>
class D3D8Font_t;

class VisualSysD3D8Impl_t : public VisualSys_t  
{
public:
	VisualSysD3D8Impl_t();
	virtual ~VisualSysD3D8Impl_t();

	virtual bool Init( Systems_t *pSystems );
	virtual void Shutdown();

    virtual bool InitDeviceObjects();
    virtual bool RestoreDeviceObjects();
    virtual bool InvalidateDeviceObjects();
    virtual bool DeleteDeviceObjects();

	virtual bool BeginRender();
	virtual void EndRender();

	virtual void WindowMove( int nX, int nY );
	virtual void RenderBox( Box2D_t *pBox );
	virtual void RenderTextBox( TextBox2D_t *pTextBox );
	virtual void RenderMenuBox( MenuBox2D_t *pMenuBox );
	virtual void RenderStats();
protected:
	Systems_t *m_pSystems;

	HWND m_hWnd;
	DWORD m_dwWidth;
	DWORD m_dwHeight;
	DWORD m_dwDepth;
	BOOL m_bWindowed;

	LPDIRECT3D8 m_lpD3D;
	LPDIRECT3DDEVICE8 m_lpD3DDevice;
	D3DPRESENT_PARAMETERS m_d3dpp;
	D3DCOLOR m_backColor;

	HRESULT CreateDevice(HWND hWnd, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth);

    D3D8Font_t *m_pFont;
};

#endif // !defined(AFX_VISUALSYSD3D8IMPL_T_H__94343A96_DE45_4DB4_8EF7_C8BEB2BC2E28__INCLUDED_)
