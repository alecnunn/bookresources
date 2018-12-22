/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// VisualSysD3D7Impl_t.h: interface for the VisualSysD3D7Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VISUALSYSD3D7IMPL_T_H__C33C87DD_944D_40F4_98AB_2548D9AEB7F9__INCLUDED_)
#define AFX_VISUALSYSD3D7IMPL_T_H__C33C87DD_944D_40F4_98AB_2548D9AEB7F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Systems/VisualSys_t.h"
#include <d3d.h>
#include <d3dx.h>
#include "d3denum.h"
class D3D7Font_t;

class VisualSysD3D7Impl_t : public VisualSys_t  
{
public:
	VisualSysD3D7Impl_t();
	virtual ~VisualSysD3D7Impl_t();

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
	BOOL m_bZBuffer;

    RECT m_rcScreenRect;       // Screen rect for window
    DWORD m_dwRenderWidth;
    DWORD m_dwRenderHeight;

	LPDIRECT3D7			m_pD3D;
	LPDIRECT3DDEVICE7	m_pD3DDevice;
	LPDIRECTDRAW7		m_pDD;             // IDirectDraw7 Interface

    LPDIRECTDRAWSURFACE7 m_pddsFrontBuffer;    // The primary surface
    LPDIRECTDRAWSURFACE7 m_pddsBackBuffer;     // The backbuffer surface
    LPDIRECTDRAWSURFACE7 m_pddsZBuffer;        // The zbuffer surface
    DWORD                m_dwDeviceMemType;
	DDPIXELFORMAT		 m_ddpfBackBufferPixelFormat;

    D3DEnum_DeviceInfo*  m_pDeviceInfo;
    HRESULT              (*m_fnConfirmDevice)(DDCAPS*, D3DDEVICEDESC7*);

	D3DCOLOR m_backColor;

	HRESULT CreateDevice(HWND hWnd, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth);
	HRESULT CreateEnvironment( GUID* pDriverGUID, GUID* pDeviceGUID, DDSURFACEDESC2* pMode );
	HRESULT CreateDirectDraw( GUID* pDriverGUID );
	HRESULT CreateFullscreenBuffers( DDSURFACEDESC2* pddsd );
	HRESULT CreateWindowedBuffers();
	HRESULT CreateZBuffer( GUID* pDeviceGUID );
	HRESULT CreateDirect3D( GUID* pDeviceGUID );
	HRESULT FlipToGDISurface( BOOL bDrawFrame );
	HRESULT ShowFrame();

    D3D7Font_t *m_pFont;
};

#endif // !defined(AFX_VISUALSYSD3D7IMPL_T_H__C33C87DD_944D_40F4_98AB_2548D9AEB7F9__INCLUDED_)
