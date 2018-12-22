//--------------------------------------------------------------------------------------------------------------------//
//                                                DX8 RENDERER CLASS                                                  //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_RENDERERDX8_H_
#define   _GPG4_RENDERERDX8_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#define   D3D_OVERLOADS
#include  <d3d8.h>
#include  <D3dx8core.h>

#include  "Renderer.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CEngineTexture;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CRendererDX8                                                        //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CRendererDX8 : public CRenderer
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CRendererDX8        (void);
    virtual                  ~CRendererDX8        (void);

    // basic ops

    virtual bool              Create              (const HWND hWnd,const u32 u32Flags=WINDOWED|ZBUFFER,
                                                   const u32 u32Width=0,const u32 u32Height=0);
    virtual void              Destroy             (void);

    virtual u32               CreateSwapChain     (const HWND hWnd,const u32 u32Flags=WINDOWED|ZBUFFER,
                                                   const u32 u32Width=0,const u32 u32Height=0);
    virtual void              ReleaseSwapChain    (const u32 u32Chain);
    virtual bool              SelectSwapChain     (const u32 u32Chain);

    virtual bool              BeginScene          (void);
    virtual bool              EndScene            (void);
    virtual bool              Clear               (const u32 u32Flags=IRenderer::CLEAR_BACKDEPTH);
    virtual bool              Swap                (const RECT* pSrcRect=NULL,const RECT* pDestRect=NULL,const u32 u32Vbls=0);
    virtual bool              SetWireframe        (const bool boON);

    // get

    virtual CStdString        GetSignature        (void) const;

    // IB,VB,mesh

    virtual CIBuffer*         CreateIB            (void);
    virtual CVBuffer*         CreateVB            (void);
    virtual CEngineMesh*      CreateMesh          (void);

    // texture

    virtual CEngineTexture*   CreateTexture       (void);
    virtual u32               Shoot               (const CStdString& strDir);

    // draw

    virtual u32               DrawVBIB            (RENDERER_PRIMITIVE Prim,CVBuffer* pVB,CIBuffer* pIB);

    // trf

    virtual bool              SetModel2World      (const CMat4x4& m4Matrix);
    virtual bool              SetWorld2View       (const CMat4x4& m4Matrix);
    virtual bool              SetPerspective      (const float fFov,const float fNear,const float fFar,const float fRatio=4.f/3.f);
    virtual bool              SetOrtho            (const CVect3D& v3Min,const CVect3D& v3Max);
    virtual bool              SetProjection       (const CMat4x4& m4Proj);

    // material

    virtual bool              SetMaterial         (const CEngineMaterial* pMaterial);

  // protected methods

    static void               FillPlain           (D3DXVECTOR4* pOut,D3DXVECTOR2* pTexCoord,D3DXVECTOR2* pTexelSize,LPVOID pData);
    void                      InitStates          (void);

  // protected data

  protected:

    // DX8 access

    LPDIRECT3DDEVICE8         m_pDevice;
    LPDIRECT3D8               m_pD3D;

    // material

    D3DMATERIAL8              m_matCurrent;
    LPDIRECT3DTEXTURE8        m_pTexWhite;
    D3DCOLOR                  m_colTexFactor;
  };

  // smart ptr

  SMART_POINTER(CRendererDX8);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "RendererDX8.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_RENDERERDX8_H_
