/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// VisualSysOpenGLImpl_t.h: interface for the VisualSysOpenGLImpl_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VISUALSYSOPENGLIMPL_T_H__DD2E1714_205C_416A_8C76_7658F93F5B2A__INCLUDED_)
#define AFX_VISUALSYSOPENGLIMPL_T_H__DD2E1714_205C_416A_8C76_7658F93F5B2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Systems/VisualSys_t.h"
#include <gl/gl.h>
//#include <gl/glu.h>
class OpenGLFont_t;

class VisualSysOpenGLImpl_t : public VisualSys_t  
{
public:
	VisualSysOpenGLImpl_t();
	virtual ~VisualSysOpenGLImpl_t();

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

	HRESULT CreateDevice(HWND hWnd, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth);

	HDC	  m_hDC;
	HGLRC m_hRC;

	HWND m_hWnd;
	DWORD m_dwWidth;
	DWORD m_dwHeight;
	DWORD m_dwDepth;
	BOOL m_bWindowed;

    OpenGLFont_t *m_pFont;
};

#endif // !defined(AFX_VISUALSYSOPENGLIMPL_T_H__DD2E1714_205C_416A_8C76_7658F93F5B2A__INCLUDED_)
