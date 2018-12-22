//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE SPHERE CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "EngineSphere.h"
#ifdef    _DEBUG
#include  "EngineSphere.inl"
#endif

#include  "Properties.h"
#include  "LevelFiles.h"
#include  "Engine.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI_PROP (CEngineSphere,CEngineNode);
IMPLEMENT_PROPERTIES(CEngineSphere,CExtraProp);

//--------------------------------------------------------------------------------------------------------------------//
//                                                PROPERTIES                                                          //
//--------------------------------------------------------------------------------------------------------------------//

bool CEngineSphere::DefineProperties()                      // static
  {
  REGISTER_PROP(Float,CEngineSphere,m_fRadius,      "Radius",     CProperty::EXPOSE|CProperty::STREAM,"");
  REGISTER_PROP(U32,  CEngineSphere,m_u32SectionPts,"Section Pts",CProperty::EXPOSE|CProperty::STREAM,"multiple of 2");

  CProperty* pProp   = REGISTER_PROP(Fct,CEngineSphere,m_strName,"Material",CProperty::EXPOSE|CProperty::STREAM,"")  // m_strName is not important
  CPropFct*  pPropFn = DYNAMIC_CAST (CPropFct,pProp);
  if(!pPropFn) return false;

  pProp  ->SetCtrlType(CProperty::BUTTON);                  // we want special editing
  pPropFn->SetFct(PropGetMat,PropSetMat,PropWriteMat,PropReadMat,PropBuildMesh);
  return true;
  }

//

const CStdString CEngineSphere::PropGetMat(const CPropFct* pProp,const CPersistent* pObj)
  {
  CPersistent*   pObj2   = const_cast<CPersistent*>(pObj);
  CEngineSphere* pSphere = DYNAMIC_CAST(CEngineSphere,pObj2);
  if(!pSphere) return "";
  CEngineMaterial* pMat = pSphere->GetMaterial();
  if(!pMat) return "NO MATERIAL";
  if(pMat->GetName().empty()) return "UNNAMED MATERIAL";
  return pMat->GetName();
  }

//

bool CEngineSphere::PropSetMat(const CPropFct* pProp,const CPersistent* pObj, const CStdString& strVal)
  {
  // temp: modified somewhere else (see CEditorSphere)
  return true;
  }

//

bool CEngineSphere::PropWriteMat(const CPropFct* pProp,const CPersistent* pObj,CLevelFiles& Level)
  {
  CPersistent*   pObj2   = const_cast<CPersistent*>(pObj);
  CEngineSphere* pSphere = DYNAMIC_CAST(CEngineSphere,pObj2);
  if(!pSphere) return false;
  CEngineMaterial* pMat = pSphere->GetMaterial();
  if(pMat) Level.AddObjToSave(pMat);

  if(Level.BinaryMode()) return(Level.GetDataFile().Write(&pMat,4) == 4);
  return Level.GetDataFile().PutString(CPropPtr::Ptr2String(&pMat));
  }

//

bool CEngineSphere::PropReadMat(const CPropFct* pProp,const CPersistent* pObj,const CStdString& strVal,CLevelFiles& Level)
  {
  if(Level.BinaryMode())
    {
    CEngineMaterial* pMat;
    if(Level.GetDataFile().Read(&pMat,4) != 4) return false;

    CLinkLoad* pLink = new CLinkLoad(pObj,const_cast<CPropFct*>(pProp),"",pMat);
    if(!pLink) return false;
    Level.AddLink(pLink);
    return true;
    }

  CLinkLoad* pLink = new CLinkLoad(pObj,const_cast<CPropFct*>(pProp),strVal,NULL);
  if(!pLink) return false;
  Level.AddLink(pLink);
  return true;
  }

//

bool CEngineSphere::PropBuildMesh(const CPropFct* pProp,const CPersistent* pObj,const CStdString& strVal,CPersistent* pPointed)
  {
  // create mesh

  CPersistent*   pObj2 = const_cast<CPersistent*>(pObj);
  CEngineSphere* pThis = DYNAMIC_CAST(CEngineSphere,pObj2);
  if(!pThis) return false;
  if(!pThis->Create(CEngine::GetEngine()->m_spRenderer,pThis->GetRadius(),pThis->GetSectionPts())) return false;

  // link material

  CEngineMaterial* pMat = DYNAMIC_CAST(CEngineMaterial,pPointed);
  pThis->SetMaterial(pMat);
  return true;
  }

//

bool CEngineSphere::PostRead()
  {
  CEngineNode::PostRead();

  if(m_spMesh) return true;                                 // already done
  return PropBuildMesh(NULL,this,"",NULL);
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITING                                                             //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- ModifyProp ---------------------------------------------------------//
// attempt to modify a property in the grid; do whatever is needed
// in : property, value
// out: property handled by the function ?
//--------------------------------------------------------------------------------------------------------------------//

bool CEngineSphere::ModifyProp(CProperty* pProp,const CStdString& strNew)
  {
  if(!IsPropOfRTTI(pProp,&CEngineSphere::m_RTTI)) return CEngineNode::ModifyProp(pProp,strNew);

  if(pProp->GetName() == "Radius")
    {
    CPropFloat* pPropF = DYNAMIC_CAST(CPropFloat,pProp);
    if(!pPropF) return true;
    float fRadius;
    if(!pPropF->String2Float(strNew,&fRadius)) return true;
    m_fRadius = fRadius;

    CEngineMaterialSP spMat = GetMaterial();
    Create(CEngine::GetEngine()->m_spRenderer,m_fRadius,m_u32SectionPts);
    SetMaterial(spMat);
    return true;
    }

  if(pProp->GetName() == "Section Pts")
    {
    CPropU32* pPropU = DYNAMIC_CAST(CPropU32,pProp);
    if(!pPropU) return true;
    u32 u32Pts;
    if(!pPropU->String2U32(strNew,&u32Pts)) return true;
    m_u32SectionPts = u32Pts;

    CEngineMaterialSP spMat = GetMaterial();
    Create(CEngine::GetEngine()->m_spRenderer,m_fRadius,m_u32SectionPts);
    SetMaterial(spMat);
    return true;
    }

  return false;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                MANAGEMENT                                                          //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Create -------------------------------------------------------------//
// create sphere's mesh
// in : renderer,radius,pts on section
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CEngineSphere::Create(IRenderer* pRenderer,const float fRadius,const u32 u32SectionPts)
  {
  CEngineMesh* pMesh = CEngineMesh::CreateSphere(pRenderer,fRadius,u32SectionPts);
  if(!pMesh) return false;

  m_spMesh  = pMesh;
  m_fRadius = fRadius;
  m_u32SectionPts = u32SectionPts;
  return true;
  }

//----------------------------------------------- Clone --------------------------------------------------------------//
// create a sphere sharing the same data
// in : renderer
// out: new sphere (= clone)
//--------------------------------------------------------------------------------------------------------------------//

CEngineSphere* CEngineSphere::Clone(IRenderer* pRenderer)
  {
  CEngineSphere* pClone = new CEngineSphere;
  if(!pClone) return NULL;

  pClone->m_fRadius       = m_fRadius;
  pClone->m_u32SectionPts = m_u32SectionPts;
  pClone->m_spMesh        = m_spMesh->Clone(pRenderer);     // !!! this allows <> materials and section pts; bad ?
  // otherwise: a change in radius/nb pts should affect all clones

  return pClone;
  UNREFERENCED_PARAMETER(pRenderer);
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                DRAW                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Draw ---------------------------------------------------------------//
// render sphere
// in : renderer,flags,condition callback
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CEngineSphere::Draw(IRenderer* pRenderer,const u32 u32Flags,pfnDrawCondition pfnCondition)
  {
  if(!pRenderer) return -1;
  CEngineNode::Draw(pRenderer,u32Flags,pfnCondition);       // draw subnodes if any
  if(!m_boDrawNode || m_boDeleted || !m_spMesh) return 0;

  if(pfnCondition && !(*pfnCondition)(this)) return 0;

  m_spMesh->SetLocalTrf(GetWorldPosition(),GetWorldRotation(),GetWorldScale(),GetWorldTrf());
  return m_spMesh->Draw(pRenderer,u32Flags,NULL);           // rem: the condition has been tested before
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
