/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
        
        
******************************************************************************/

#ifndef __FILTER_BLIT_H
#define __FILTER_BLIT_H

class CFilterBlit : public EBEffect
{
public:
    typedef enum 
    {
        FIRST_FILTER_OPTION = 0,
	    CONE_FILTER     = 0,
	    BOX9_FILTER     ,
        BOX16_FILTER    ,
        SHARPEN_FILTER  ,
	    LUMINANCE_EDGE  ,
        NUM_FILTER_OPTIONS
    } eFilterOptions;

public:
	CFilterBlit(eFilterOptions initDisplayOption);
	~CFilterBlit();
    virtual void    UpdateProperties();
	virtual void    PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten);
	virtual HRESULT Initialize(LPDIRECT3DDEVICE8 pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
    virtual void    Keyboard(DWORD dwKey, UINT nFlags, bool bDown);

private:
    HRESULT CreateTextureRenderTarget();
    void    CreateAndWriteUVOffsets(int width, int height);

private:
    typedef struct tagQuadVertex
    {
	    D3DXVECTOR3 Position;
        D3DXVECTOR2 Tex;
    } QuadVertex;

    enum 
    {
        kMaxNumPasses = 3,
    };

private:
    eFilterOptions           meDisplayOption;
    eFilterOptions           meInitDisplayOption;
    bool                     mbWireFrame;

	nv_objects::NVDevice	*m_pNVDevice;
	nv_objects::NVTexture	*mpTextureToFilter;

	DWORD			         m_dwVertexShader;
	DWORD			         m_dwLuminancePixelShader;
	DWORD			         m_dwLuminanceDiagEdgePixelShader;
	DWORD			         m_dwLuminanceSensitiveDiagEdgePixelShader;
	DWORD			         m_dwBlurPixelShader;
	DWORD			         m_dwSharpenPixelShader;

	LPDIRECT3DVERTEXBUFFER8  m_pVertexBuffer;

    IDirect3DSurface8       *mpBackbufferColor; 
    IDirect3DSurface8       *mpBackbufferDepth; 

    IDirect3DTexture8       *mpTextureFiltered[kMaxNumPasses];       
    IDirect3DSurface8       *mpFilterTarget   [kMaxNumPasses]; 
};

#endif