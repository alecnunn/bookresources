//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE BOX CLASS                                                    //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_ENGINEBOX_INL_
#define   _GPG4_ENGINEBOX_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CEngineBox::CEngineBox()
  {
  m_v3Extent.Set(1.f);
  }

INLINE CEngineBox::~CEngineBox()
  {}

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET/SET                                                             //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CStdString CEngineBox::GetEditName() const
  {
  return "Box";
  }

INLINE const CVect3D& CEngineBox::GetExtent() const
  {
  return m_v3Extent;
  }

INLINE CEngineMeshSP CEngineBox::GetMesh() const
  {
  return m_spMesh;
  }

INLINE CEngineMaterial* CEngineBox::GetMaterial() const
  {
  if(!m_spMesh) return NULL;
  return m_spMesh->GetMaterial();
  }

INLINE void CEngineBox::SetMaterial(CEngineMaterial* pMat)
  {
  if(m_spMesh) m_spMesh->SetMaterial(pMat);
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_ENGINEBOX_INL_
