//--------------------------------------------------------------------------------------------------------------------//
//                                                DX8 VERTEX BUFFER CLASS                                             //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_VBUFFERDX8_INL_
#define   _GPG4_VBUFFERDX8_INL_

#include  "MyAssert.h"

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CVBufferDX8::CVBufferDX8(const LPDIRECT3DDEVICE8 pDevice) : m_pDevice(pDevice)
  {
  MY_ASSERT(pDevice,"device can't be NULL");
  m_pVB = NULL;
  }

INLINE CVBufferDX8::~CVBufferDX8()
  {
  Destroy();
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET                                                                 //
//--------------------------------------------------------------------------------------------------------------------//

INLINE LPDIRECT3DVERTEXBUFFER8 CVBufferDX8::GetInterface() const
  {
  return m_pVB;
  }

INLINE u32 CVBufferDX8::GetFVF() const
  {
  return m_u32FVF;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_VBUFFERDX8_INL_
