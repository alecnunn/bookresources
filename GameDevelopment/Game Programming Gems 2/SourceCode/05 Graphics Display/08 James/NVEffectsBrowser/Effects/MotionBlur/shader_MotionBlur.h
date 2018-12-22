/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
       
        
******************************************************************************/

#ifndef __SHADER_MOTIONBLUR_H
#define __SHADER_MOTIONBLUR_H

class CShaderMotionBlur;

class MotionBlurVertex
{
public:
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 Texture;
};

#define MOTIONBLURVERTEX_FVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define BACKGROUND_COLOR     0x0099aaee

// device overrides
class NVMotionBlurDevice : public nv_objects::NVDevice
{
public:
	NVMotionBlurDevice(LPDIRECT3DDEVICE8 pDev = NULL, CShaderMotionBlur* pMotionBlur = NULL) 
		: NVDevice(pDev),
			m_pMotionBlurShader(pMotionBlur)
	{ };
	
	CShaderMotionBlur* m_pMotionBlurShader;
};

class CShaderMotionBlur: public EBEffect
{
public:
	CShaderMotionBlur();
	~CShaderMotionBlur();
	virtual void UpdateProperties();
	virtual HRESULT Initialize(IDirect3DDevice8* pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);
    virtual void    Keyboard(DWORD dwKey, UINT nFlags, bool bDown);

private:
    typedef enum tageBlurLengths
    {
        FIRST_BLURLENGTH = 0,
	    HALFxBLURLENGTH  = 0,
	    ONExBLURLENGTH,
        ONEHALFxBLURLENGTH,
        NUM_BLURLENGTHS,
    } eBlurLengths;

private:
	friend class NVMotionBlurDevice;

	HRESULT SetTransform(bool bWithBlur);

	NVMotionBlurDevice*     m_pNVDevice;

	// The floor
	nv_objects::NVMesh*     m_pFloorMesh;
	nv_objects::NVMesh*     m_pBlurredObjectMesh;
    LPDIRECT3DTEXTURE8		m_pBlurredObjectMap;
     
    // the blurred object
    LPDIRECT3DVERTEXBUFFER8 m_pBlurredObjectVB;
	LPDIRECT3DINDEXBUFFER8  m_pBlurredObjectIB[6];
    DWORD                   m_numVertices;
    DWORD                   m_numIndices;

    DWORD                   m_dwMotionBlurShader;

    D3DXMATRIX              m_LastWorldTxf;
    float                   m_OneOverExtent;

    bool                    m_bEnableMotionBlur;
    bool                    m_bEnableSpeed2x;
    bool                    m_bPause;
    eBlurLengths            meBlurLengthOption;
    bool                    mbWireFrame;
};

#endif //__SHADER_MOTIONBLUR_H
