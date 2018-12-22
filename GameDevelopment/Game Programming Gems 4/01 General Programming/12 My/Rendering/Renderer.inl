//--------------------------------------------------------------------------------------------------------------------//
//                                                RENDERER BASE CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_RENDERER_INL_
#define   _GPG4_RENDERER_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET                                                                 //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CStdString CRenderer::GetSignature() const
  {
  return "NO RENDERER";
  }

INLINE bool CRenderer::IsInitialized() const
  {
  return m_boInit;
  }

INLINE CSwapChain* CRenderer::GetCurrentChain() const
  {
  return m_pCurrentChain;
  }

INLINE const CMat4x4& CRenderer::GetWorldTrf() const
  {
  return m_m4World;
  }

INLINE const CMat4x4& CRenderer::GetViewTrf() const
  {
  return m_m4View;
  }

INLINE const CMat4x4& CRenderer::GetProjection() const
  {
  return m_m4Proj;
  }

INLINE CEngineMaterial* CRenderer::GetMatWhiteSelf() const
  {
  return m_pMatWhiteSelf;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_RENDERER_INL_
