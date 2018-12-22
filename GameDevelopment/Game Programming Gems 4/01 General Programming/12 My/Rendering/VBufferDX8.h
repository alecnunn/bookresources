//--------------------------------------------------------------------------------------------------------------------//
//                                                DX8 VERTEX BUFFER CLASS                                             //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_VBUFFERDX8_H_
#define   _GPG4_VBUFFERDX8_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#define   D3D_OVERLOADS
#include  <d3d8.h>

#include  "VBuffer.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CVBufferDX8                                                         //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CVBufferDX8 : public CVBuffer
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CVBufferDX8         (const LPDIRECT3DDEVICE8 pDevice);
    virtual                  ~CVBufferDX8         (void);

    // get

    LPDIRECT3DVERTEXBUFFER8   GetInterface        (void) const;
    u32                       GetFVF              (void) const;

    // management

    virtual u32               Create              (void* pBuffer,const u32 u32Count,
                                                   VTX_COMPONENTS& Vtx=m_VtxDefault,TEX_COMPONENTS& Tex=m_TexDefault,
                                                   const bool boStatic=true,const bool boRAMCopy=false);
    virtual u32               CreateCopy          (void* pBuffer,const u32 u32Count,
                                                   VTX_COMPONENTS& Vtx=m_VtxDefault,TEX_COMPONENTS& Tex=m_TexDefault,
                                                   const bool boStatic=true,const bool boRAMCopy=false);
    virtual u32               Destroy             (void);

    virtual u32               Write               (void* pBuffer,const u32 u32Count,const u32 u32Pos=0);
    virtual u32               WriteToHW           (void* pBuffer,const u32 u32Count,const u32 u32Pos=0);
    virtual u32               UpdateFromRAM       (              const u32 u32Count,const u32 u32Pos=0);

  // helpers

    static u32                CalcVertexFVF       (VTX_COMPONENTS& VtxFormat,TEX_COMPONENTS& TexFormat);

  // protected methods

  protected:

    u32                       CalcVertexFVF       (void);
    static u32                TexCoordSize        (const u32 u32Dim,const u32 u32Stage);

  // protected data

  protected:

    LPDIRECT3DDEVICE8         m_pDevice;
    LPDIRECT3DVERTEXBUFFER8   m_pVB;
    u32                       m_u32FVF;
  };

  // smart ptr

  SMART_POINTER(CVBufferDX8);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "VBufferDX8.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_VBUFFERDX8_H_
