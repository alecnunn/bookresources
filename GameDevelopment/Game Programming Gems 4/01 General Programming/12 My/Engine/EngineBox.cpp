//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE BOX CLASS                                                    //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "EngineBox.h"
#ifdef    _DEBUG
#include  "EngineBox.inl"
#endif

#include  "Properties.h"
#include  "LevelFiles.h"
#include  "Engine.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI_PROP (CEngineBox,CEngineNode);
IMPLEMENT_PROPERTIES(CEngineBox,CExtraProp);

//--------------------------------------------------------------------------------------------------------------------//
//                                                PROPERTIES                                                          //
//--------------------------------------------------------------------------------------------------------------------//

bool CEngineBox::DefineProperties()                         // static
  {
  REGISTER_PROP(Vect3D,CEngineBox,m_v3Extent,"Extent",CProperty::EXPOSE|CProperty::STREAM,"")

  CProperty* pProp   = REGISTER_PROP(Fct,CEngineBox,m_strName,"Material",CProperty::EXPOSE|CProperty::STREAM,"")
  CPropFct*  pPropFn = DYNAMIC_CAST (CPropFct,pProp);
  if(!pPropFn) return false;

  pProp  ->SetCtrlType(CProperty::BUTTON);                  // we want special editing
  pPropFn->SetFct(PropGetMat,PropSetMat,PropWriteMat,PropReadMat,PropBuildMesh);
  return true;
  }

//

const CStdString CEngineBox::PropGetMat(const CPropFct* pProp,const CPersistent* pObj)
  {
  CPersistent* pObj2 = const_cast<CPersistent*>(pObj);
  CEngineBox*  pBox  = DYNAMIC_CAST(CEngineBox,pObj2);
  if(!pBox) return "";
  CEngineMaterial* pMat = pBox->GetMaterial();
  if(!pMat) return "NO MATERIAL";
  if(pMat->GetName().empty()) return "UNNAMED MATERIAL";
  return pMat->GetName();
  }

//

bool CEngineBox::PropSetMat(const CPropFct* pProp,const CPersistent* pObj,const CStdString& strVal)
  {
  // temp: modified somewhere else (see CEditorBox)
  return true;
  }

//

bool CEngineBox::PropWriteMat(const CPropFct* pProp,const CPersistent* pObj,CLevelFiles& Level)
  {
  CPersistent* pObj2 = const_cast<CPersistent*>(pObj);
  CEngineBox*  pBox  = DYNAMIC_CAST(CEngineBox,pObj2);
  if(!pBox) return false;
  CEngineMaterial* pMat = pBox->GetMaterial();
  if(pMat) Level.AddObjToSave(pMat);

  if(Level.BinaryMode()) return(Level.GetDataFile().Write(&pMat,4) == 4);
  return Level.GetDataFile().PutString(CPropPtr::Ptr2String(&pMat));
  }

//

bool CEngineBox::PropReadMat(const CPropFct* pProp,const CPersistent* pObj,const CStdString& strVal,CLevelFiles& Level)
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

bool CEngineBox::PropBuildMesh(const CPropFct* pProp,const CPersistent* pObj,const CStdString& strVal,CPersistent* pPointed)
  {
  // create mesh

  CPersistent* pObj2 = const_cast<CPersistent*>(pObj);
  CEngineBox*  pThis = DYNAMIC_CAST(CEngineBox,pObj2);
  if(!pThis) return false;
  if(!pThis->Create(CEngine::GetEngine()->m_spRenderer,pThis->GetExtent())) return false;

  // link material

  CEngineMaterial* pMat = DYNAMIC_CAST(CEngineMaterial,pPointed);
  pThis->SetMaterial(pMat);
  return true;
  }

//

bool CEngineBox::PostRead()
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

bool CEngineBox::ModifyProp(CProperty* pProp,const CStdString& strNew)
  {
  if(!IsPropOfRTTI(pProp,&CEngineBox::m_RTTI)) return CEngineNode::ModifyProp(pProp,strNew);

  if(pProp->GetName() == "Extent")
    {
    CPropVect3D* pPropV3 = DYNAMIC_CAST(CPropVect3D,pProp);
    if(!pPropV3) return true;
    CVect3D v3Extent;
    if(!pPropV3->String2Floats(strNew,&v3Extent[0])) return true;
    m_v3Extent = v3Extent;

    CEngineMaterialSP spMat = GetMaterial();
    Create(CEngine::GetEngine()->m_spRenderer,m_v3Extent);
    SetMaterial(spMat);
    return true;
    }

  return false;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                MANAGEMENT                                                          //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Create -------------------------------------------------------------//
// create box's mesh
// in : renderer,extents
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CEngineBox::Create(IRenderer* pRenderer,const CVect3D& v3Extent)
  {
  CEngineMesh* pMesh = CEngineMesh::CreateBox(pRenderer,v3Extent*-0.5f,v3Extent*0.5f);
  if(!pMesh) return false;

  m_spMesh   = pMesh;
  m_v3Extent = v3Extent;
  return true;
  }

//----------------------------------------------- Clone --------------------------------------------------------------//
// create a box sharing the same data
// in : renderer
// out: new box (= clone)
//--------------------------------------------------------------------------------------------------------------------//

CEngineBox* CEngineBox::Clone(IRenderer* pRenderer)
  {
  CEngineBox* pClone = new CEngineBox;
  if(!pClone) return NULL;

  pClone->m_v3Extent = m_v3Extent;
  pClone->m_spMesh   = m_spMesh->Clone(pRenderer);          // a mesh doesn't take a lot of mem, and this allows <> materials

  return pClone;
  UNREFERENCED_PARAMETER(pRenderer);
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                DRAW                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Draw ---------------------------------------------------------------//
// render box
// in : renderer,flags,condition callback
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CEngineBox::Draw(IRenderer* pRenderer,const u32 u32Flags,pfnDrawCondition pfnCondition)
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
