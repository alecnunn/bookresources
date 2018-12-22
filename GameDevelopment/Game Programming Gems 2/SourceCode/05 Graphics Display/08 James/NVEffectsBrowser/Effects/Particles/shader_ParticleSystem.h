/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
        
        
******************************************************************************/

#ifndef __SHADER_PSYSTEM_H
#define __SHADER_PSYSTEM_H

#include <sys/types.h>
#include <sys/timeb.h>

#include "MouseUI.h"


typedef struct tagFloorVertex
{
	D3DXVECTOR4 position;
	D3DXVECTOR2 texture;
} FloorVertex;

typedef struct tagPSystemVertex
{
	D3DXVECTOR4 timeSeeds;
	D3DXVECTOR4 physics;
	D3DXVECTOR3 color;
} PSystemVertex;

class CShaderPSystem : public EBEffect
{
public:
	CShaderPSystem();
	~CShaderPSystem();
	virtual void    UpdateProperties();
	virtual HRESULT Initialize(IDirect3DDevice8* pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
    virtual void    Keyboard(DWORD dwKey, UINT nFlags, bool bDown);
    virtual void    MouseMove(HWND hWnd, int x, int y);
	virtual void    MouseButton(HWND hWnd, eButtonID Button, bool bDown, int x, int y);
    
private:

    void WriteVertexShaderConstants() const;
    void SetViewFromKeyboardInput(EBTimer* pTimer, D3DXMATRIX &matView);

    enum {
        kStartParticles =  1000,
        kNumParticles   = 10000,
        kNumRndVectors  =    40,
        kMaxVKey        =   256,
    };

    bool                    m_bKey[kMaxVKey];

	DWORD                   mParticleShader;
	DWORD                   mFloorShader;
	LPDIRECT3DVERTEXBUFFER8 m_pParticleVB;
	LPDIRECT3DVERTEXBUFFER8 m_pFloorVB;
	LPDIRECT3DTEXTURE8		m_pParticleTexture;
	LPDIRECT3DTEXTURE8		m_pFloorTexture;

    int                     mNumParticles;
    bool                    mbFreezeFrame;
    bool                    mbWireFrame;

    struct _timeb           mStartTime;

    MouseUI                *mpMouseUI;

};

#endif