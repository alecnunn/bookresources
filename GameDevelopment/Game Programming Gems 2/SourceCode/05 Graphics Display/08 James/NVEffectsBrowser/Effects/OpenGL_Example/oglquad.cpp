/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

#include <windows.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "eb_effect.h"
#include "oglquad.h"
#include "nvinc.h"

using namespace std;

DECLARE_EFFECT_MAIN()

extern "C"
{

__declspec(dllexport) unsigned int GetNumEffects() { return 1; }

__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
{	
	switch(EffectNum)
	{
		case 0:
			return new CSimpleOGLQuad();
			break;
		default:
			return NULL;
	}
}

}

CSimpleOGLQuad::CSimpleOGLQuad()
  : m_bWireframe(false),
    m_pUI(NULL)
{
  m_strEffectLocation = "Simple Example\\OpenGL Quad";
  m_strEffectName = "Simple OpenGL Quad";
  m_strEffectVertexShader = "";
  m_strEffectPixelShader = "";
}

void CSimpleOGLQuad::UpdateProperties()
{
  EBEffect::UpdateProperties();

  AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));
}

CSimpleOGLQuad::~CSimpleOGLQuad()
{
  Free();	
}

HRESULT CSimpleOGLQuad::Initialize(HGLRC)
{
	// initialize mouse UI
	//
	// the actual shape will come in through the Resize method
	RECT rect;
	rect.left   = 0;
	rect.top    = 0;
	rect.bottom = 100;
	rect.right  = 100;
	m_pUI = new MouseUI((const RECT) rect);

	return S_OK;
}

HRESULT CSimpleOGLQuad::Free()
{
  return S_OK;
}

HRESULT CSimpleOGLQuad::Start()
{
  return S_OK;
}

HRESULT CSimpleOGLQuad::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;
	
	glClearColor(0.7f, 0.7f, 0.7f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -4.0f);
	glMultMatrixf((float*) &m_pUI->GetTranslationMatrix());
	glMultMatrixf((float*) &m_pUI->GetRotationMatrix());
	
	if (m_bWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glBegin(GL_QUADS);
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(-1.0f, -1.0f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex2f( 1.0f, -1.0f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2f( 1.0f,  1.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex2f(-1.0f,  1.0f);
	}
	glEnd();
	
	return hr;
}

HRESULT CSimpleOGLQuad::ConfirmDevice(PIXELFORMATDESCRIPTOR*)
{
  return S_OK;
}

void CSimpleOGLQuad::Resize(HWND hWnd, int w, int h, bool)
{
	EBEffect::Resize(hWnd, w, h, true);

	// initialize mouse UI
	RECT rect;
	rect.left   = 0;
	rect.top    = 0;
	rect.bottom = m_iHeight;
	rect.right  = m_iWidth;
	m_pUI->SetRECT(rect);

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, m_fAspect, 0.1f, 15.0f);
	
	glClearColor(0.7f, 0.7f, 0.7f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CSimpleOGLQuad::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
{
	// Flip coords from windows trackball to OpenGL space
	x = (m_iWidth-1)  - x;
	y = (m_iHeight-1) - y;
		
	if (button == MOUSE_LEFTBUTTON) {
		if (bDown)
			m_pUI->OnLButtonDown(x, y);
		else
			m_pUI->OnLButtonUp(x, y);
	}
	
	return;
}

void CSimpleOGLQuad::MouseMove(HWND hWnd, int x, int y)
{
	// Flip coords from windows trackball to OpenGL space
	x = (m_iWidth-1)  - x;
	y = (m_iHeight-1) - y;
		  
	m_pUI->OnMouseMove(x, y);
	return;
}

void CSimpleOGLQuad::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
  eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);
	
  switch (Action) {
  case EB_HELP:
    {
      ::MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n Left Button & Mouse - Rotate Object\n\n Shift Left Button & Mouse - Pan Camera \n\n Ctrl Left Button & Mouse - Move Camera In & Out\n\n",
		     "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
    }
    break;

  case EB_WIREFRAME:
    {
      m_bWireframe = !m_bWireframe;
      m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
    }
    break;

  case EB_RESET:
    {
      m_pUI->Reset();
      m_bWireframe = false;
      m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
    }
    break;

  default:
      break;
  }
}
