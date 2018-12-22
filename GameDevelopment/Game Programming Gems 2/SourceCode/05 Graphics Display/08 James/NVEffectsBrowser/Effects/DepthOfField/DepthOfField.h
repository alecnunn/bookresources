/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
        
******************************************************************************/

#ifndef __DEPTH_OF_FIELD_H
#define __DEPTH_OF_FIELD_H

#include "MouseUI.h"


class CDepthOfField : public EBEffect
{
public:
	CDepthOfField();
	~CDepthOfField();
    virtual void    UpdateProperties();
	virtual HRESULT Initialize(LPDIRECT3DDEVICE8 pDev);
	virtual HRESULT Free();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer* pTimer);
	virtual HRESULT ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format);

    virtual void MouseMove(HWND hWnd, int x, int y);
	virtual void MouseButton(HWND hWnd, eButtonID Button, bool bDown, int x, int y);
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown);

private:
    HRESULT InitWorldRendering();
    HRESULT InitBlurRendering();
    HRESULT InitTetrahedronRendering();

    HRESULT SetMatrices(D3DXMATRIX const &matWorld);
    HRESULT SetBlurRenderState();

    HRESULT RenderWorld();
    HRESULT RenderBlur();

    HRESULT CreateTextureRenderTarget();
    HRESULT UpdateCameraParameters();
    HRESULT GenerateCircleOfConfusionTexture();
    void    CreateAndWriteUVOffsets(int width, int height);

    HRESULT CreateWorldCube();
    HRESULT CreateTetrahedron();

    void SetViewFromKeyboardInput(EBTimer* pTimer);
    void ResetCameraPosition();

private:
    struct tQuadVertex
    {
	    tQuadVertex(D3DXVECTOR3 const & vecPosition, 
                    D3DXVECTOR2 const & vecUV)
            : mPosition(vecPosition)
            , mTexture (vecUV)
 	    {
            ;
        };

        D3DXVECTOR3 mPosition;
        D3DXVECTOR2 mTexture;
    };

    struct tTetrahedronVertex
    {
	    tTetrahedronVertex(D3DXVECTOR3 const & vecPosition, 
                           D3DXVECTOR3 const & vecNormal,
                           D3DXVECTOR2 const & vecUV)
            : mPosition(vecPosition)
            , mNormal  (vecNormal)
            , mUV      (vecUV)
 	    {
            ;
        };

        D3DXVECTOR3 mPosition;
	    D3DXVECTOR3 mNormal;
        D3DXVECTOR2 mUV;
   };

    enum 
    {
        kNumOfFilterSteps     =  5,

        kNumQuadsPerSide      = 32,     // tesselates large world polygons 
        kNumVerticesPerFace   = (kNumQuadsPerSide + 1) * (kNumQuadsPerSide + 1),
        kNumVertices          = 6 * kNumVerticesPerFace,
        kNumIndicesPerFace    = 2 * (kNumQuadsPerSide + 1) * kNumQuadsPerSide,
        kNumIndices           = 6 * kNumIndicesPerFace,
        kNumTrisPerStrip      = 2 * kNumQuadsPerSide,

        // the following parameters give a volume-map size of 4MB, 
        // while keeping interpolation errors very small
        kConfusionLookupWidth =  256,    // corresponds to distance to camera
        kConfusionLookupHeight=  128,    // corresponds to focal distance
        kConfusionLookupDepth =   32,    // corresponds to focal lengths

        kNumTetrahedra        = 1200,

        kMaxVKey              =   256,
    };

    typedef enum tageDisplayOptions
    {
	    SHOW_COLORS     = 0,
	    SHOW_DEPTH         ,
	    SHOW_BLURRINESS    ,
    } eDisplayOptions;

    static float const       kCloseClip;
    static float const       kFarClip;

    static float const       kMinFocusDistance;
    static float const       kMaxFocusDistance;

    static float const       kMinFocalLength;
    static float const       kMaxFocalLength;

    static float const       kFilmDimension;
    static float const       kBlurFactor;

private:
    bool                     m_bKey[kMaxVKey];

	nv_objects::NVDevice	*m_pNVDevice;

    // options variables
    bool                     mbWireFrame;
    eDisplayOptions          meDisplayOption;

    bool                     mbUsesVolumes;

    // camera data
    MouseUI                 *mpMouseUI;
    D3DXVECTOR3              mCamPosition;
    D3DXVECTOR3              mCamLookDirection;

    float                    mFStop;
    float                    mFocalLength;
    float                    mFocusDistance;

    // world box 
    D3DXVECTOR3              mWorldBoxDimensions;    
	LPDIRECT3DVERTEXBUFFER8  mpWorldBoxVertices;
	LPDIRECT3DINDEXBUFFER8   mpWorldBoxIndices;
	nv_objects::NVTexture   *mpWorldTextures[6];


    DWORD			         m_dwBlurVertexShader;
	DWORD			         m_dwBlurPixelShader;
    DWORD			         m_dwWorldVertexShader;
	DWORD			         m_dwWorldPixelShader;
	DWORD			         m_dwWorldNoDoFPixelShader;
    DWORD			         m_dwTetraVertexShader;
	DWORD			         m_dwTetraPixelShader;
	DWORD			         m_dwTetraNoDoFPixelShader;
	DWORD			         m_dwDepthOfFieldPixelShader;
	DWORD			         m_dwShowBlurrinessShader;

	LPDIRECT3DVERTEXBUFFER8  mpQuadVertexBuffer;
	LPDIRECT3DVERTEXBUFFER8  mpTetrahedronVertices;
	LPDIRECT3DINDEXBUFFER8   mpTetrahedronIndices;
	nv_objects::NVTexture   *mpObjectTexture;

    IDirect3DSurface8       *mpBackbufferColor; 
    IDirect3DSurface8       *mpBackbufferDepth; 
    IDirect3DSurface8       *mpDepthTarget; 

    IDirect3DTexture8       *mpCircleOfConfusionLookup;       
    IDirect3DVolumeTexture8 *mpVolCircleOfConfusionLookup;       

    IDirect3DTexture8       *mpTextureFiltered[3];       
    IDirect3DSurface8       *mpFilterTarget   [3]; 

    IDirect3DTexture8       *mpTempTexture[2];       
    IDirect3DSurface8       *mpTempTarget [2]; 

};

#endif