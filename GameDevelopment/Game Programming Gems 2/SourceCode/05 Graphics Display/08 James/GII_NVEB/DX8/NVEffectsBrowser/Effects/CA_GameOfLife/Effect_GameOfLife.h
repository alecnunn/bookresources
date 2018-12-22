/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Dx8\NVEffectsBrowser\Effects\CA_Fire
File:  Effect_Fire.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/

#ifndef __EFFECT_LIFE_GJ_H
#define __EFFECT_LIFE_GJ_H


#include "eb_effect.h"		// used only for GetFilePath()


class ShaderManager;		// utility for loading/creating vert & pixel shaders
class CA_GameOfLife;


/////////////////////////////////////////////////////////

class Effect_GameOfLife : public EBEffect
{
private:
	// Requires GUI update, so it's private
    bool			m_bWireframe;		// Only affect final rendering
										//   not render to texture steps

public:
	Effect_GameOfLife();
	~Effect_GameOfLife();

    virtual void    UpdateProperties();
	virtual void    PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten);

	virtual HRESULT Initialize(LPDIRECT3DDEVICE8 pDev);

	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);

	virtual void	MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y);
	virtual void	MouseMove(HWND hWnd, int x, int y);
    virtual void    Keyboard(DWORD dwKey, UINT nFlags, bool bDown);


	CA_GameOfLife	* m_pCA_GameOfLife;


	ShaderManager	* m_pShaderManager;

};


#endif					// __EFFECT_LIFE_GJ_H