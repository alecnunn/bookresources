//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE NODE CLASS                                                   //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "EngineNode.h"
#ifdef    _DEBUG
#include  "EngineNode.inl"
#endif

#include  "EngineMesh.h"
#include  "Properties.h"
#include  "LevelFiles.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI_PROP (CEngineNode,CEngineObj);
IMPLEMENT_PROPERTIES(CEngineNode,CExtraProp);

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

CEngineNode::CEngineNode()
  {
  m_qRot.Identity();
  m_v3Pos.Set(0.f);
  m_v3Scale.Set(1.f);

  m_boLocalDirty = true;
  m_boWorldDirty = true;
  UpdateLocalTrf();

  m_pParentNode = NULL;

  m_boDrawNode = true;
  m_boDrawMesh = true;
  m_boCollide  = true;
  }

//

CEngineNode::~CEngineNode()
  {
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                PROPERTIES                                                          //
//--------------------------------------------------------------------------------------------------------------------//

bool CEngineNode::DefineProperties()                        // static
  {
  CProperty* pProp = REGISTER_PROP(Fct,CEngineNode,m_strName,"Subnodes",CProperty::STREAM,"")  // m_strName is not important
  CPropFct*  pFn   = DYNAMIC_CAST (CPropFct,pProp);
  if(!pFn) return false;
  pFn->SetFct(NULL,NULL,WriteNodes,ReadNodes,LinkNodes);

  pProp = REGISTER_PROP(Vect4D,CEngineNode,m_qRot,"Rotation", CProperty::EXPOSE|CProperty::STREAM,"");
  pProp->SetCtrlType(CProperty::BUTTON);                    // we want special editing

  REGISTER_PROP(Vect3D,CEngineNode,m_v3Pos,       "Position", CProperty::EXPOSE|CProperty::STREAM,"");
  REGISTER_PROP(Vect3D,CEngineNode,m_v3Scale,     "Scale",/*CProperty::EXPOSE|*/CProperty::STREAM,"");

  REGISTER_PROP(Bool,  CEngineNode,m_boDrawNode,  "Draw Node",CProperty::EXPOSE|CProperty::STREAM,"");
  REGISTER_PROP(Bool,  CEngineNode,m_boCollide,   "Collide",  CProperty::EXPOSE|CProperty::STREAM,"");
  return true;
  }

//

bool CEngineNode::WriteNodes(const CPropFct* pProp,const CPersistent* pObj,CLevelFiles& Level)
  {
  CPersistent* pObj2 = const_cast<CPersistent*>(pObj);
  CEngineNode* pNode = DYNAMIC_CAST(CEngineNode,pObj2);
  if(!pNode) return false;
  u32 u32Nb = pNode->GetNbSubnodes();                       // nb to be able to reload
  bool boRes;

  if(Level.BinaryMode()) boRes = (Level.GetDataFile().Write(&u32Nb,4) == 4);
  else                   boRes =  Level.GetDataFile().PutString(CPropU32::U32ToString(&u32Nb));
  if(!u32Nb) return boRes;

  CEngineNode* pSub = pNode->GetFirstSubnode();
  while(pSub)
    {
    Level.AddObjToSave(pSub);
    if(Level.BinaryMode()) boRes &= (Level.GetDataFile().Write(&pSub,4) == 4);
    else                   boRes &=  Level.GetDataFile().PutString("\n  "+CPropPtr::Ptr2String(&pSub));
    pSub = pNode->GetNextSubnode();
    }
  return boRes;
  }

//

bool CEngineNode::ReadNodes(const CPropFct* pProp,const CPersistent* pObj,const CStdString& strVal,CLevelFiles& Level)
  {
  u32 u32Nb = 0;

  if(Level.BinaryMode())
    {
    if(Level.GetDataFile().Read(&u32Nb,4) != 4) return false;
    }
  else
    {
    if(sscanf(strVal,"%u",&u32Nb) != 1) return false;
    }

  if(u32Nb == 0) return true;                               // no child

  CPersistent Persist;
  CStdString  strID;

  if(Level.BinaryMode())
    {
    while(u32Nb--)
      {
      CEngineNode* pSub;
      if(Level.GetDataFile().Read(&pSub,4) != 4) return false;

      CLinkLoad* pLink = new CLinkLoad(pObj,const_cast<CPropFct*>(pProp),"",pSub);
      if(pLink) Level.AddLink(pLink);
      }
    return true;
    }

  while(u32Nb--)
    {
    if(Persist.ReadValue(Level,&strID))
      {
      CLinkLoad* pLink = new CLinkLoad(pObj,const_cast<CPropFct*>(pProp),strID,NULL);
      if(pLink) Level.AddLink(pLink);
      }
    }
  return true;
  }

//

bool CEngineNode::LinkNodes(const CPropFct* pProp,const CPersistent* pObj,const CStdString& strVal,CPersistent* pPointed)
  {
  CPersistent* pObj2   = const_cast<CPersistent*>(pObj);
  CEngineNode* pParent = DYNAMIC_CAST(CEngineNode,pObj2);
  if(!pParent) return false;

  CEngineNode* pChild  = DYNAMIC_CAST(CEngineNode,pPointed);
  if(!pChild) return false;
  return pParent->AddSubnode(pChild);
  }

//

bool CEngineNode::PostRead()
  {
  CEngineObj::PostRead();

  SetLocalDirty();
  UpdateLocalTrf();
  UpdateWorldTrf();
  return true;
  }

//----------------------------------------------- ModifyProp ---------------------------------------------------------//
// attempt to modify a property in the grid; do whatever is needed
// in : property, value
// out: property handled by the function ?
//--------------------------------------------------------------------------------------------------------------------//

bool CEngineNode::ModifyProp(CProperty* pProp,const CStdString& strNew)
  {
  if(!IsPropOfRTTI(pProp,&CEngineNode::m_RTTI)) return CEngineObj::ModifyProp(pProp,strNew);

  if(pProp->GetName() == "Position")
    {
    CPropVect3D* pPropV3 = DYNAMIC_CAST(CPropVect3D,pProp);
    if(!pPropV3) return true;
    CVect3D v3Pos;
    if(!pPropV3->String2Floats(strNew,&v3Pos[0])) return true;

    SetPosition(v3Pos);
    return true;
    }

  if(pProp->GetName() == "Rotation")
    {
    CPropVect4D* pPropV4 = DYNAMIC_CAST(CPropVect4D,pProp);
    if(!pPropV4) return true;
    CVect4D v4Rot;
    if(!pPropV4->String2Floats(strNew,&v4Rot[0])) return true;

    SetRotation(CQuaternion(v4Rot));
    return true;
    }

  return false;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                TRFS GET/SET                                                        //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- UpdateWorldTrf -----------------------------------------------------//
// update model->world trf
// in :
// out: updated trf
//--------------------------------------------------------------------------------------------------------------------//

const CMat4x4& CEngineNode::UpdateWorldTrf()
  {
  if(!m_boWorldDirty) return m_m4World;
  m_m4World = GetLocalTrf();

  CEngineNode* pParent  = GetParentNode();
  if(pParent) m_m4World = pParent->GetWorldTrf()*m_m4World;

  m_boWorldDirty = false;
  return m_m4World;
  }

//----------------------------------------------- UpdateLocalTrf -----------------------------------------------------//
// update model->parent node trf
// in :
// out: updated trf
//--------------------------------------------------------------------------------------------------------------------//

const CMat4x4& CEngineNode::UpdateLocalTrf()
  {
  if(!m_boLocalDirty) return m_m4Local;

  m_qRot.ToRotation(&m_m4Local);
  m_m4Local.Col[POS].Set(m_v3Pos,1.f);

  CMat4x4 m4Scale;
  m4Scale.Scale(m_v3Scale);
  m_m4Local *= m4Scale;

  m_boLocalDirty = false;
  return m_m4Local;
  }

//----------------------------------------------- SetWorldPosition ---------------------------------------------------//
// set local position to match given world pos
// in : world pos
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CEngineNode::SetWorldPosition(const CVect3D& v3Pos)
  {
  CEngineNode* pParent = GetParentNode();
  if(!pParent) return SetPosition(v3Pos);

  CMat4x4 m4World = pParent->GetWorldTrf();
  m4World.Invert();
  return SetPosition(CVect3D(m4World*v3Pos,W));
  }

//----------------------------------------------- SetWorldRotation ---------------------------------------------------//
// set local rotation to match given world rotation
// in : world rotation
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CEngineNode::SetWorldRotation(const CQuaternion& qRot)
  {
  CEngineNode* pParent = GetParentNode();
  if(!pParent) return SetRotation(qRot);

  CQuaternion qWorld = pParent->GetWorldRotation();
  return SetRotation(qRot*qWorld.Conjugate());
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                DRAW                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Draw ---------------------------------------------------------------//
// render subnodes
// in : renderer,flags,condition callback
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CEngineNode::Draw(IRenderer* pRenderer,const u32 u32Flags,pfnDrawCondition pfnCondition)
  {
  if(!pRenderer)  return -1;

  if(m_boDrawNode)
    {
    CEngineNode* pNode = GetFirstSubnode();
    while(pNode)
      {
      pNode->Draw(pRenderer,u32Flags,pfnCondition);
      pNode = GetNextSubnode();
      }
    }

  return 0;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
