/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// VisualSys_t.h: interface for the VisualSys_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VISUALSYS_T_H__EF8AFEBD_40CC_4271_BCBE_FE9B452A19F5__INCLUDED_)
#define AFX_VISUALSYS_T_H__EF8AFEBD_40CC_4271_BCBE_FE9B452A19F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Systems_t;
class Box2D_t;
class TextBox2D_t;
class MenuBox2D_t;

class VisualSys_t  
{
public:
	VisualSys_t() {};
	virtual ~VisualSys_t() {};

	virtual bool Init( Systems_t *pSystems ) = 0;
	virtual void Shutdown() = 0;

    virtual bool InitDeviceObjects() = 0;
    virtual bool RestoreDeviceObjects() = 0;
    virtual bool InvalidateDeviceObjects() = 0;
    virtual bool DeleteDeviceObjects() = 0;

	virtual bool BeginRender() = 0;
	virtual void EndRender() = 0;

	virtual void WindowMove( int nX, int nY ) = 0;
	virtual void RenderBox( Box2D_t *pBox ) = 0;
	virtual void RenderTextBox( TextBox2D_t *pTextBox ) = 0;
	virtual void RenderMenuBox( MenuBox2D_t *pMenuBox ) = 0;
	virtual void RenderStats() = 0;
};

#endif // !defined(AFX_VISUALSYS_T_H__EF8AFEBD_40CC_4271_BCBE_FE9B452A19F5__INCLUDED_)
