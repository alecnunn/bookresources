//--------------------------------------------------------------------------------------------------------------------//
//                                                VERTEX BUFFER CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_VBUFFER_H_
#define   _GPG4_VBUFFER_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "Persistent.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CVBuffer                                                            //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CVBuffer : public CRefCount
  {
  DECLARE_RTTI;

  // public typedefs

  public:

    typedef struct
      {
      u8                      XYZ       : 1;
      u8                      Weights   : 2;                // 0 to 3 weights
      u8                      Indices   : 1;                // last weight = 4 [0;255] indices
      u8                      Normal    : 1;
      u8                      PtSize    : 1;
      u8                      Diffuse   : 1;
      u8                      Specular  : 1;
      u8                      Tangent   : 1;
      u8                      Binormal  : 1;

      u8                      Tex       : 4;                // 0 to 8 texture coords
      } VTX_COMPONENTS;

    typedef struct
      {
      u8                      Tex0      : 2;
      u8                      Tex1      : 2;
      u8                      Tex2      : 2;
      u8                      Tex3      : 2;
      u8                      Tex4      : 2;
      u8                      Tex5      : 2;
      u8                      Tex6      : 2;
      u8                      Tex7      : 2;
      } TEX_COMPONENTS;                                     // 00=2D, 01=3D, 10=4D, 11=1D

  // public data

  public:

    static VTX_COMPONENTS     m_VtxDefault;
    static TEX_COMPONENTS     m_TexDefault;

  // public methods

  public:

    // constructors & destructor

                              CVBuffer            (void);
    virtual                  ~CVBuffer            (void);

    // get

    u32                       GetCount            (void) const;
    u32                       GetActiveCount      (void) const;
    bool                      IsStatic            (void) const;
    bool                      IsDynamic           (void) const;
    bool                      HasRAMCopy          (void) const;
    u32                       GetVertexSize       (void) const;

    static u32                CalcVertexSize      (const VTX_COMPONENTS& VtxFmt,const TEX_COMPONENTS& TexFmt);
    u32                       SetActiveCount      (const u32 u32Count);

    // management

    virtual u32               Create              (void* pBuffer,const u32 u32Count,
                                                   VTX_COMPONENTS& Vtx=m_VtxDefault,TEX_COMPONENTS& Tex=m_TexDefault,
                                                   const bool boStatic=true,const bool boRAMCopy=false);
    virtual u32               CreateCopy          (void* pBuffer,const u32 u32Count,
                                                   VTX_COMPONENTS& Vtx=m_VtxDefault,TEX_COMPONENTS& Tex=m_TexDefault,
                                                   const bool boStatic=true,const bool boRAMCopy=false);
    virtual u32               Destroy             (void);

    virtual u32               Write               (void* pBuffer,const u32 u32Count,const u32 u32Pos=0);
    virtual u32               Read                (void* pBuffer,const u32 u32Count,const u32 u32Pos=0);

    u32                       WriteToRAM          (void* pBuffer,const u32 u32Count,const u32 u32Pos=0);
    virtual u32               UpdateFromRAM       (              const u32 u32Count,const u32 u32Pos=0);

  // protected methods

  protected:

    u32                       CalcVertexSize      (void);

  // protected data

  protected:

    void*                     m_pBuffer;
    u32                       m_u32Count;
    u32                       m_u32ActiveCount;

    bool                      m_boStatic;
    bool                      m_boRAMCopy;

    // vertex format

    VTX_COMPONENTS            m_VtxFormat;
    TEX_COMPONENTS            m_TexFormat;
    u32                       m_u32VtxSize;                 // sizeof(one vertex)

  // friends (allowed to access m_pBuffer directly for READING)

    friend class              CBVMesh;
  };

  // smart ptr

  SMART_POINTER(CVBuffer);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "VBuffer.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_VBUFFER_H_
