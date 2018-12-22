//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE TEXTURE CLASS                                                //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_ENGINETEXTURE_INL_
#define   _GPG4_ENGINETEXTURE_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET/SET                                                             //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CStdString CEngineTexture::GetEditName() const
  {
  return "Texture";
  }

INLINE const CStdString& CEngineTexture::GetFileName() const
  {
  return m_strFileName;
  }

INLINE void CEngineTexture::SetFileName(const CStdString& strFile)
  {
  m_strFileName = strFile;
  }

INLINE u32 CEngineTexture::GetWidth() const
  {
  return m_u32Width;
  }

INLINE u32 CEngineTexture::GetHeight() const
  {
  return m_u32Height;
  }

INLINE u32 CEngineTexture::GetDepth() const
  {
  return m_u32Depth;
  }

INLINE u32 CEngineTexture::GetFormat() const
  {
  return m_u32Format;
  }

INLINE u32 CEngineTexture::GetMemBytes() const
  {
  return m_u32MemBytes;
  }

INLINE void* CEngineTexture::GetSurfacePtr() const
  {
  if(!m_boInRAM) return NULL;
  return m_pu8RAM;
  }

INLINE HRESULT CEngineTexture::ReleaseSurfacePtr(const void* pvSurface)
  {
  return S_OK;
  }

INLINE bool CEngineTexture::IsInRAM() const
  {
  return m_boInRAM;
  }

INLINE bool CEngineTexture::IsInVideo() const
  {
  return m_boInVideo;
  }

INLINE u32 CEngineTexture::GetUsedRAM()
  {
  return m_u32UsedRAM;
  }

INLINE u32 CEngineTexture::GetUsedVideo()
  {
  return m_u32UsedVideo;
  }

INLINE u32 CEngineTexture::GetType() const
  {
  return m_u32Type;
  }

INLINE u32 CEngineTexture::GetRGBAOrder() const
  {
  return m_u32RGBAOrder;
  }

INLINE void CEngineTexture::SetRGBAOrder(const u32 u32Order)
  {
  m_u32RGBAOrder = u32Order;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_ENGINETEXTURE_INL_
