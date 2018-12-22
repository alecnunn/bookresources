/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Dx8\NVEffectsBrowser\Effects\CA_RGB_Plasmas
File:  Effect_Plasmas.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/



#ifndef __NVEFFECT_PLASMAS_H
#define __NVEFFECT_PLASMAS_H

#include "MouseUI.h"


///////////////////////////////
// external:

class CA_Water;

///////////////////////////////
//  In this module:

class NVEffect_Plasmas;


///////////////////////////////


typedef enum tagPlasmaDisplayOptions
{
	DISPLAY_BLINN8BITSIGNED = 0,
	DISPLAY_BLINNHILOSIGNED = 1,
	DISPLAY_BLINNFORCEDWORD = 0xFFFFFFFF
} ePlasmaDisplayOptions;



class NVEffect_Plasmas : public EBEffect
{
public:
	NVEffect_Plasmas();
	~NVEffect_Plasmas();
	virtual void UpdateProperties();
	virtual HRESULT Initialize(IDirect3DDevice8* pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
	virtual void MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y);
	virtual void MouseMove(HWND hWnd, int x, int y);
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);
	virtual void PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten);

	void	TryDrawDroplet( float x, float y );
	void	RestoreRenderState();


protected:

	CA_Water *	m_pCA_Water;		// class generating procedural normal map
									//   animation

	bool m_bWireframe;
	MouseUI* m_pUI;
	

	bool	m_bShowProceduralMaps;

	bool	m_bDrawing;			// drawing on while dragging
	bool	m_bDrawModeOn;

	
	ePlasmaDisplayOptions m_eDisplayOption;

	bool m_bPause;

	DWORD m_dwBlinnVertexShader;
};


#endif				// __NVEFFECT_PLASMAS_H
