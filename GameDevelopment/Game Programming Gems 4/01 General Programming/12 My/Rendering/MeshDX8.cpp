//--------------------------------------------------------------------------------------------------------------------//
//                                                DX8 MESH CLASS                                                      //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "MeshDX8.h"
#ifdef    _DEBUG
#include  "MeshDX8.inl"
#endif

#include  "Renderer.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI(CMeshDX8,CEngineMesh);

//--------------------------------------------------------------------------------------------------------------------//
//                                                MANAGEMENT                                                          //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Clone --------------------------------------------------------------//
// create a mesh sharing the same data
// in : renderer
// out: new mesh (= clone)
//--------------------------------------------------------------------------------------------------------------------//

CEngineMesh* CMeshDX8::Clone(IRenderer* pRenderer)
  {
  CEngineMesh* pClone = pRenderer->CreateMesh();            // this will set m_pDevice
  CMeshDX8* pCloneDX8 = DYNAMIC_CAST(CMeshDX8,pClone);
  if(!pCloneDX8) return NULL;

  pCloneDX8->m_spIB  = m_spIB;                              // this deletes the IB created by CreateIBVB
  pCloneDX8->m_spVB  = m_spVB;
  pCloneDX8->m_spMat = m_spMat;

  pCloneDX8->m_u32PrimType = m_u32PrimType;

  return pCloneDX8;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
