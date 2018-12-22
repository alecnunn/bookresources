//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE MATERIAL CLASS                                               //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_ENGINEMATERIAL_INL_
#define   _GPG4_ENGINEMATERIAL_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET/SET                                                             //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CStdString CEngineMaterial::GetEditName() const
  {
  return "Material";
  }

INLINE CVect4D CEngineMaterial::GetDiffuse() const
  {
  return m_v4Diffuse;
  }

INLINE void CEngineMaterial::SetDiffuse(const CVect4D& v4Color)
  {
  m_v4Diffuse = v4Color;
  }

INLINE bool CEngineMaterial::IsLighted() const
  {
  return m_boLighted;
  }

INLINE void CEngineMaterial::EnableLighting(const bool boLighting)
  {
  m_boLighted = boLighting;
  }

INLINE bool CEngineMaterial::IsZEnabled() const
  {
  return m_boZTest;
  }

INLINE void CEngineMaterial::EnableZTest(const bool boZTest)
  {
  m_boZTest = boZTest;
  }

INLINE bool CEngineMaterial::IsCulled() const
  {
  return m_boCulling;
  }

INLINE void CEngineMaterial::EnableCulling(const bool boCull)
  {
  m_boCulling = boCull;
  }

INLINE CEngineTexture* CEngineMaterial::GetTexture(const u32 u32Slot) const
  {
  if(u32Slot >= NbSlots) return NULL;
  return m_spTex[u32Slot];
  }

INLINE bool CEngineMaterial::SetTexture(CEngineTexture* pTex,const u32 u32Slot)
  {
  if(u32Slot >= NbSlots) return false;
  m_spTex[u32Slot] = pTex;
  return true;
  }

INLINE CEngineMaterial::TEXTURE_OP CEngineMaterial::GetTexOp(const u32 u32Slot) const
  {
  if(u32Slot >= NbSlots) return NOP;
  return m_TexOp[u32Slot];
  }

INLINE bool CEngineMaterial::SetTexOp(TEXTURE_OP Op,const u32 u32Slot)
  {
  if(u32Slot >= NbSlots) return false;
  m_TexOp[u32Slot] = Op;
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_ENGINEMATERIAL_INL_
