//--------------------------------------------------------------------------------------------------------------------//
//                                                DX8 INDEX BUFFER CLASS                                              //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_IBUFFERDX8_H_
#define   _GPG4_IBUFFERDX8_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#define   D3D_OVERLOADS
#include  <d3d8.h>

#include  "IBuffer.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CIBufferDX8                                                         //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CIBufferDX8 : public CIBuffer
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CIBufferDX8         (const LPDIRECT3DDEVICE8 pDevice);
    virtual                  ~CIBufferDX8         (void);

    // get

    LPDIRECT3DINDEXBUFFER8    GetInterface        (void) const;

    // management

    virtual u32               Create              (u16* pBuffer,const u32 u32Count,const bool boStatic=true,const bool boRAMCopy=false);
    virtual u32               CreateCopy          (u16* pBuffer,const u32 u32Count,const bool boStatic=true,const bool boRAMCopy=false);
    virtual u32               Destroy             (void);

    virtual u32               Write               (u16* pBuffer,const u32 u32Count,const u32 u32Pos=0);
    virtual u32               WriteToHW           (u16* pBuffer,const u32 u32Count,const u32 u32Pos=0);
    virtual u32               UpdateFromRAM       (             const u32 u32Count,const u32 u32Pos=0);

  // protected data

  protected:

    LPDIRECT3DDEVICE8         m_pDevice;
    LPDIRECT3DINDEXBUFFER8    m_pIB;
  };

  // smart ptr

  SMART_POINTER(CIBufferDX8);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "IBufferDX8.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_IBUFFERDX8_H_
