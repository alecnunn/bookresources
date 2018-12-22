//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE MESH CLASS                                                   //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_ENGINEMESH_INL_
#define   _GPG4_ENGINEMESH_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CEngineMesh::CEngineMesh()
  {
  m_u32PrimType = IRenderer::TRILIST;
  }

INLINE CEngineMesh::~CEngineMesh()
  {}

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET                                                                 //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CStdString CEngineMesh::GetEditName() const
  {
  return "Mesh";
  }

INLINE CIBufferSP CEngineMesh::GetIB() const
  {
  return m_spIB;
  }

INLINE CVBufferSP CEngineMesh::GetVB() const
  {
  return m_spVB;
  }

INLINE CEngineMaterialSP CEngineMesh::GetMaterial() const
  {
  return m_spMat;
  }

INLINE IRenderer::RENDERER_PRIMITIVE CEngineMesh::GetPrimitive() const
  {
  return m_u32PrimType;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                SET                                                                 //
//--------------------------------------------------------------------------------------------------------------------//

INLINE void CEngineMesh::SetMaterial(CEngineMaterial* pMat)
  {
  m_spMat = pMat;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_ENGINEMESH_INL_
