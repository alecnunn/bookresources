/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

#ifndef __SHADER_QUAD_H
#define __SHADER_QUAD_H
#include "MouseUI.h"

class CSimpleOGLQuad : public EBEffect
{
public:
	
	CSimpleOGLQuad();
	~CSimpleOGLQuad();
	virtual EBGFXAPI API() const { return GFXAPI_OPENGL; };
	
	virtual void UpdateProperties();
	virtual HRESULT Initialize(HGLRC);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(PIXELFORMATDESCRIPTOR*);
	virtual void Resize(HWND hWnd, int w, int h, bool bUpdate);
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);
	virtual void MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y);
	virtual void MouseMove(HWND hWnd, int x, int y);
	
private:
	 
	bool     m_bWireframe;
	MouseUI* m_pUI;
};

#endif
