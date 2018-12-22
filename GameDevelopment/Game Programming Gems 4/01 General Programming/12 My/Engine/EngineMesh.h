//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE MESH CLASS                                                   //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_ENGINEMESH_H_
#define   _GPG4_ENGINEMESH_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "EngineNode.h"
#include  "EngineMaterial.h"
#include  "IBuffer.h"
#include  "VBuffer.h"
#include  "Renderer.h"
#include  "VectorMatrix.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CEngineMesh                                                          //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CEngineMesh : public CEngineNode
  {
  DECLARE_RTTI;

  // public typedefs

  public:

    typedef struct VERTEX_SIMPLE
      {
      CVect3D m_XYZ;
      CVect3D m_Normal;
      CVect2D m_Tex;
      VERTEX_SIMPLE(CVect3D& XYZ,CVect3D& Normal,CVect2D Tex) { m_XYZ = XYZ; m_Normal = Normal; m_Tex = Tex; }
      VERTEX_SIMPLE() {}
      } VERTEX_SIMPLE;

  // public methods

  public:

    // constructors & destructor

                              CEngineMesh         (void);
    virtual                  ~CEngineMesh         (void);

    // get

    virtual CStdString        GetEditName         (void) const;

    CIBufferSP                GetIB               (void) const;
    CVBufferSP                GetVB               (void) const;
    CEngineMaterialSP         GetMaterial         (void) const;
    IRenderer::RENDERER_PRIMITIVE GetPrimitive    (void) const;

    // set

    void                      SetMaterial         (CEngineMaterial* pMat);

    // management

    virtual u32               CreateIBVB          (IRenderer* pRenderer);       // empty IB & VB
    virtual u32               Create              (void* pVtx,const u32 u32Vtx,u16* pIdx,const u32 u32Idx,
                                                   CVBuffer::VTX_COMPONENTS& Vtx=CVBuffer::m_VtxDefault,
                                                   CVBuffer::TEX_COMPONENTS& Tex=CVBuffer::m_TexDefault,
                                                   const bool boStatic=true,const bool boRAMCopy=false);
    virtual u32               CreateCopy          (void* pVtx,const u32 u32Vtx,u16* pIdx,const u32 u32Idx,
                                                   CVBuffer::VTX_COMPONENTS& Vtx=CVBuffer::m_VtxDefault,
                                                   CVBuffer::TEX_COMPONENTS& Tex=CVBuffer::m_TexDefault,
                                                   const bool boStatic=true,const bool boRAMCopy=false);

    virtual CEngineMesh*      Clone               (IRenderer* pRenderer);
    virtual bool              Clone               (CEngineMesh* pClone);

    // helper

    static CEngineMesh*       CreateBox           (IRenderer* pRenderer,const CVect3D& v3Min,  const CVect3D& v3Max);
    static CEngineMesh*       CreateEmptyGrid     (IRenderer* pRenderer,const CVect3D& v3Lines,const CVect3D& v3Extent);
    static CEngineMesh*       CreateSphere        (IRenderer* pRenderer,const float    fRadius,const u32      u32SectionPts);
    static CEngineMesh*       CreateRectangle     (IRenderer* pRenderer,const CVect3D& v3Left, const CVect3D& v3Right,
                                                   const CVect2D& v2UV0,const CVect2D& v2UV1);
    static CEngineMesh*       CreateQuads         (IRenderer* pRenderer,const u32 u32Quads,const VERTEX_SIMPLE* pVtx);
    static CEngineMesh*       CreateBlock         (IRenderer* pRenderer,const CEngineTexture& HeightMap,const CVect2D* pv2UV,
                                                   const CVect2D* pv2XZ,const CVect2D& v2Alti,const u32 u32Resol,const CVect2D& v2Tiling);
    static CEngineMesh*       CreateFromIBVB      (IRenderer* pRenderer,CVBuffer* pVB,CIBuffer* pIB,IRenderer::RENDERER_PRIMITIVE Prim);

    // draw

    virtual u32               Draw                (IRenderer* pRenderer,const u32 u32Flags,pfnDrawCondition pfnCondition);

  // protected data

  protected:

    CIBufferSP                m_spIB;
    CVBufferSP                m_spVB;
    CEngineMaterialSP         m_spMat;

    IRenderer::RENDERER_PRIMITIVE       m_u32PrimType;
  };

  // smart ptr

  SMART_POINTER(CEngineMesh);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "EngineMesh.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_ENGINEMESH_H_
