//--------------------------------------------------------------------------------------------------------------------//
//                                                RENDERER INTERFACE                                                  //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_RENDERER_H_
#define   _GPG4_RENDERER_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "Persistent.h"
#include  "StlIncludes.h"
#include  "VectorMatrix.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CEngineCamera;
  class CEngineMaterial;
  class CEngineMesh;
  class CEngineTexture;

  class CIBuffer;
  class CSwapChain;
  class CVBuffer;

  class CMat4x4;
  class CVect3D;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                IRenderer                                                           //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class IRenderer : public CPersistent
  {
  DECLARE_RTTI;

  // public enums

  public:

    typedef enum
      {
      WINDOWED                = 1 << 0,
      ZBUFFER                 = 1 << 1,
      } RENDERER_CREATE;

    typedef enum
      {
      CLEAR_BACK              = 1 << 0,
      CLEAR_DEPTH             = 1 << 1,
      CLEAR_STENCIL           = 1 << 2,

      CLEAR_BACKDEPTH         = CLEAR_BACK | CLEAR_DEPTH,
      CLEAR_ALL               = 0xffffffff,
      } RENDERER_CLEAR;

    typedef enum
      {
      TRILIST                 = 0,
      LINELIST,
      LINESTRIP,
      } RENDERER_PRIMITIVE;

  // public methods

  public:

    // constructors & destructor

                              IRenderer           (void) {}
    virtual                  ~IRenderer           (void) {}

    // basic ops

    virtual bool              Create              (const HWND hWnd,const u32 u32Flags=WINDOWED|ZBUFFER,
                                                   const u32 u32Width=0,const u32 u32Height=0) = 0;
    virtual void              Destroy             (void) = 0;

    virtual u32               CreateSwapChain     (const HWND hWnd,const u32 u32Flags=WINDOWED|ZBUFFER,
                                                   const u32 u32Width=0,const u32 u32Height=0) = 0;
    virtual void              ReleaseSwapChain    (const u32 u32Chain) = 0;
    virtual bool              SelectSwapChain     (const u32 u32Chain) = 0;
    virtual bool              AddSwapChain        (CSwapChain* pSwapChain) = 0;
    virtual bool              RemoveSwapChain     (CSwapChain* pSwapChain) = 0;

    virtual bool              BeginScene          (void) = 0;
    virtual bool              EndScene            (void) = 0;
    virtual bool              Clear               (const u32 u32Flags=CLEAR_BACKDEPTH) = 0;
    virtual bool              Swap                (const RECT* pSrcRect=NULL,const RECT* pDestRect=NULL,const u32 u32Vbls=0) = 0;
    virtual bool              SetWireframe        (const bool boON) = 0;

    // get

    virtual CStdString        GetSignature        (void) const = 0;
    virtual bool              IsInitialized       (void) const = 0;
    virtual CSwapChain*       GetSwapChain        (const u32 u32Chain) = 0;
    virtual CSwapChain*       GetCurrentChain     (void) const = 0;

    virtual CVect3D           GetClearColor       (void) = 0;
    virtual bool              SetClearColor       (const CVect3D& v3Color) = 0;

    virtual u32               GetWidth            (void) = 0;
    virtual u32               GetHeight           (void) = 0;

    virtual CEngineMaterial*  GetMatWhiteSelf     (void) const = 0;

    // IB,VB,mesh

    virtual CIBuffer*         CreateIB            (void) = 0;
    virtual CVBuffer*         CreateVB            (void) = 0;
    virtual CEngineMesh*      CreateMesh          (void) = 0;

    // texture

    virtual CEngineTexture*   CreateTexture       (void) = 0;
    virtual u32               Shoot               (const CStdString& strDir)  = 0;

    virtual void              AddTexture          (const CStdString& strFile,CEngineTexture* pTex) = 0;
    virtual bool              RemoveTexture       (CEngineTexture* pTex) = 0;
    virtual CEngineTexture*   GetTexture          (const CStdString& strFile) = 0;

    // draw

    virtual u32               DrawVBIB            (RENDERER_PRIMITIVE Prim,CVBuffer* pVB,CIBuffer* pIB) = 0;

    // trf

    virtual bool              SetModel2World      (const CMat4x4& m4Matrix) = 0;
    virtual bool              SetWorld2View       (const CMat4x4& m4Matrix) = 0;
    virtual bool              SetPerspective      (const float fFov,const float fNear,const float fFar,const float fRatio=4.f/3.f) = 0;
    virtual bool              SetOrtho            (const CVect3D& v3Min,const CVect3D& v3Max) = 0;

    virtual const CMat4x4&    GetWorldTrf         (void) const = 0;
    virtual const CMat4x4&    GetViewTrf          (void) const = 0;
    virtual const CMat4x4&    GetProjection       (void) const = 0;
    virtual bool              SetProjection       (const CMat4x4& m4Proj) = 0;

    virtual bool              SetCamera           (const CEngineCamera* pCam) = 0;

    // material

    virtual bool              SetMaterial         (const CEngineMaterial* pMaterial) = 0;
  };

  // smart ptr

  SMART_POINTER(IRenderer);
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CRenderer                                                           //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CRenderer : public IRenderer
  {
  DECLARE_RTTI;

  // typedefs

  public:

    typedef std::list< CSwapChain* >              listChain;
    typedef listChain::iterator                   iterChain;

    typedef std::map<CStdString,CEngineTexture*>  mapStr2Tex;
    typedef mapStr2Tex::iterator                  iterStr2Tex;
  
  // public methods

  public:

    // constructors & destructor

                              CRenderer           (void);
    virtual                  ~CRenderer           (void);

    // basic ops

    virtual void              Destroy             (void);
    virtual bool              AddSwapChain        (CSwapChain* pSwapChain);
    virtual bool              RemoveSwapChain     (CSwapChain* pSwapChain);

    // get

    virtual CStdString        GetSignature        (void) const;
    virtual bool              IsInitialized       (void) const;
    virtual CSwapChain*       GetSwapChain        (const u32 u32Chain);
    virtual CSwapChain*       GetCurrentChain     (void) const;

    virtual u32               GetWidth            (void);
    virtual u32               GetHeight           (void);

    virtual CVect3D           GetClearColor       (void);
    virtual bool              SetClearColor       (const CVect3D& pfColor);

    virtual const CMat4x4&    GetWorldTrf         (void) const;
    virtual const CMat4x4&    GetViewTrf          (void) const;
    virtual const CMat4x4&    GetProjection       (void) const;

    virtual CEngineMaterial*  GetMatWhiteSelf     (void) const;

    // texture

    virtual void              AddTexture          (const CStdString& strFile,CEngineTexture* pTex);
    virtual bool              RemoveTexture       (CEngineTexture* pTex);
    virtual CEngineTexture*   GetTexture          (const CStdString& strFile);

    // trf

    virtual bool              SetCamera           (const CEngineCamera* pCam);

  // protected data

  protected:

    bool                      m_boInit;
    listChain                 m_SwapChains;
    u32                       m_u32CurrentChain;
    CSwapChain*               m_pCurrentChain;

    bool                      m_boLighted;
    bool                      m_boZTest;
    bool                      m_boCulling;
    bool                      m_boTexture;

    CMat4x4                   m_m4World;
    CMat4x4                   m_m4View;
    CMat4x4                   m_m4Proj;

    CEngineMaterial*          m_pMatWhiteSelf;

    mapStr2Tex                m_Textures;
  };

  // smart ptr

  SMART_POINTER(CRenderer);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "Renderer.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_RENDERER_H_
