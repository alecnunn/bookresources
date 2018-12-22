//--------------------------------------------------------------------------------------------------------------------//
//                                                DX8 TEXTURE CLASS                                                   //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_TEXTUREDX8_INL_
#define   _GPG4_TEXTUREDX8_INL_
#pragma    once

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CTextureDX8::CTextureDX8(const LPDIRECT3DDEVICE8 pDevice) : m_pDevice(pDevice), m_pTexInterface(NULL)
  {
  m_u32RGBAOrder = ORDER_BGRA;
  m_dx8TexFormat = D3DFMT_UNKNOWN;
  }

INLINE CTextureDX8::~CTextureDX8()
  {
  if(m_pTexInterface) m_pTexInterface->Release();
  m_pTexInterface = NULL;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET/SET                                                             //
//--------------------------------------------------------------------------------------------------------------------//

INLINE const CRTTI* CTextureDX8::GetWriteRTTI() const
  {
  return GetRTTI()->GetBaseRTTI();                          // save base class
  }

INLINE LPDIRECT3DTEXTURE8 CTextureDX8::GetInterface()
  {
  if(!m_pTexInterface) DownloadInit();
  return m_pTexInterface;
  }

INLINE LPDIRECT3DSURFACE8 CTextureDX8::GetSurface() const
  {
  if(!m_pTexInterface) return NULL;
  LPDIRECT3DSURFACE8 pSurf;
  m_pTexInterface->GetSurfaceLevel(0,&pSurf);
  return pSurf; 
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_TEXTUREDX8_INL_
