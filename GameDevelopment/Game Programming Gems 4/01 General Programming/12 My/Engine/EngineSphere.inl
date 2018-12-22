//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE SPHERE CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_ENGINESPHERE_INL_
#define   _GPG4_ENGINESPHERE_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CEngineSphere::CEngineSphere()
  {
  m_fRadius = 1.f;
  m_u32SectionPts = 8;
  }

INLINE CEngineSphere::~CEngineSphere()
  {}

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET/SET                                                             //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CStdString CEngineSphere::GetEditName() const
  {
  return "Sphere";
  }

INLINE float CEngineSphere::GetRadius() const
  {
  return m_fRadius;
  }

INLINE u32 CEngineSphere::GetSectionPts() const
  {
  return m_u32SectionPts;
  }

INLINE CEngineMeshSP CEngineSphere::GetMesh() const
  {
  return m_spMesh;
  }
INLINE CEngineMaterial* CEngineSphere::GetMaterial() const
  {
  if(!m_spMesh) return NULL;
  return m_spMesh->GetMaterial();
  }

INLINE void CEngineSphere::SetMaterial(CEngineMaterial* pMat)
  {
  if(m_spMesh) m_spMesh->SetMaterial(pMat);
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_ENGINESPHERE_INL_
