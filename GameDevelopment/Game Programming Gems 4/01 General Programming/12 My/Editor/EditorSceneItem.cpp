//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR SCENE ITEM CLASS                                             //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "EditorSceneItem.h"
#ifdef    _DEBUG
#include  "EditorSceneItem.inl"
#endif

#include  "EditorScene.h"
#include  "MyAssert.h"
#include  "Properties.h"
#include  "LevelFiles.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI_PROP (CEditorSceneItem,CPersistent);
IMPLEMENT_PROPERTIES(CEditorSceneItem,CExtraProp);

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

CEditorSceneItem::CEditorSceneItem(CEditorScene* pScene,CEditorObj* pObj) : m_pScene(pScene), m_spEditorObj(pObj)
  {
  m_pParentItem = NULL;
  m_hItem       = NULL;
  m_boDeleted   = false;
  m_boExpanded  = false;

  MY_ASSERT(pObj,  "CEditorSceneItem::CEditorSceneItem : item's object can't be NULL");
  MY_ASSERT(pScene,"CEditorSceneItem::CEditorSceneItem : item's scene can't be NULL");
  if(pScene && pObj)
    {
    pScene->RegisterItem(pObj->GetName(),this);
    }
  if(pObj) m_strName = pObj->GetName();
  }

//

CEditorSceneItem::CEditorSceneItem()
  {
  m_pScene      = NULL;
  m_pParentItem = NULL;
  m_hItem       = NULL;
  m_boDeleted   = false;
  }

//

CEditorSceneItem::~CEditorSceneItem()
  {
  if(m_pScene && m_spEditorObj) m_pScene->UnregisterItem(m_spEditorObj->GetName(),this);
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                PROPERTIES                                                          //
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorSceneItem::DefineProperties()                   // static
  {
  REGISTER_PROP(Bool,  CEditorSceneItem,m_boDeleted,    "Deleted",    CProperty::STREAM,"")
  REGISTER_PROP(Bool,  CEditorSceneItem,m_boExpanded,   "Expanded",   CProperty::STREAM,"")
  REGISTER_PROP(String,CEditorSceneItem,m_strPrevParent,"Prev Parent",CProperty::STREAM,"")
  REGISTER_PROP(SP,    CEditorSceneItem,m_spEditorObj,  "EditorObj",  CProperty::STREAM,"")

  CProperty* pProp   = REGISTER_PROP(Fct,CEditorSceneItem,m_ChildItems,"Subitems",CProperty::STREAM,"")  // m_ChildItems is not important
  CPropFct*  pPropFn = DYNAMIC_CAST (CPropFct,pProp);
  if(!pPropFn) return false;
  pPropFn->SetFct(NULL,NULL,PropWriteChildren,PropReadChildren,PropLinkChildren);
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                STREAMING                                                           //
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorSceneItem::PropWriteChildren(const CPropFct* pProp,const CPersistent* pObj,CLevelFiles& Level)
  {
  CPersistent*      pObj2 = const_cast<CPersistent*>(pObj);
  CEditorSceneItem* pItem = DYNAMIC_CAST(CEditorSceneItem,pObj2);
  if(!pItem) return false;
  u32 u32Nb = pItem->GetNbChildren();                       // nb to be able to reload
  bool boRes;

  if(Level.BinaryMode()) boRes = (Level.GetDataFile().Write(&u32Nb,4) == 4);
  else                   boRes =  Level.GetDataFile().PutString(CPropU32::U32ToString(&u32Nb));
  if(!u32Nb) return boRes;

  CEditorSceneItem* pSub = pItem->GetFirstChild();
  while(pSub)
    {
    Level.AddObjToSave(pSub);
    if(Level.BinaryMode()) boRes &= (Level.GetDataFile().Write(&pSub,4) == 4);
    else                   boRes &=  Level.GetDataFile().PutString("\n  "+CPropPtr::Ptr2String(&pSub));
    pSub = pItem->GetNextChild();
    }
  return boRes;
  }

//

bool CEditorSceneItem::PropReadChildren(const CPropFct* pProp,const CPersistent* pObj,const CStdString& strVal,CLevelFiles& Level)
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
      CEditorSceneItem* pSub;
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
      CLinkLoad* pLink = new CLinkLoad((CPersistent*)pObj,const_cast<CPropFct*>(pProp),strID,NULL);
      if(pLink) Level.AddLink(pLink);
      }
    }
  return true;
  }

//

bool CEditorSceneItem::PropLinkChildren(const CPropFct* pProp,const CPersistent* pObj,const CStdString& strVal,CPersistent* pPointed)
  {
  CEditorSceneItem* pChild  = DYNAMIC_CAST(CEditorSceneItem,pPointed);
  CPersistent*      pObj2   = const_cast<CPersistent*>(pObj);
  CEditorSceneItem* pParent = DYNAMIC_CAST(CEditorSceneItem,pObj2);
  if(!pParent || !pChild) return false;
  return pParent->AddChild(pChild);
  }

//

void CEditorSceneItem::SetScene(CEditorScene* pScene)
  {
  if(m_pScene && m_spEditorObj) m_pScene->UnregisterItem(m_spEditorObj->GetName(),this);
  m_pScene = pScene;
  if(m_pScene && m_spEditorObj) m_pScene->RegisterItem  (m_spEditorObj->GetName(),this);
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
