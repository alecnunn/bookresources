//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR NODE CLASS                                                   //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "EditorNode.h"
#ifdef    _DEBUG
#include  "EditorNode.inl"
#endif

#include  "EngineNode.h"
#include  "Properties.h"
#include  "DlgRotation.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI      (CEditorNode,CEditorObj);
IMPLEMENT_PROPERTIES(CEditorNode,CExtraProp);

//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE                                                              //
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorNode::CreateEngineObj()
  {
  CEngineObj* pEngineObj = CEngineObj::CreateObject(&CEngineNode::m_RTTI);
  if(!pEngineObj) return false;

  pEngineObj->SetName(m_strName);
  pEngineObj->SetEditorObj(this);
  SetEngineObj(pEngineObj);
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                PROPERTIES                                                          //
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorNode::DefineProperties()                        // static
  {
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                STATE                                                               //
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorNode::Store()
  {
  if(!CEditorObj::Store()) return false;

  m_boStored = false;
  if(!m_spEngineObj) return false;
  CEngineNode* pNode = DYNAMIC_CAST(CEngineNode,m_spEngineObj);
  if(!pNode) return false;

  m_v3Pos   = pNode->GetPosition();
  m_qRot    = pNode->GetRotation();
  m_v3Scale = pNode->GetScale();

  m_boStored = true;
  return true;
  }

//

bool CEditorNode::Reset()
  {
  if(!CEditorObj::Reset()) return false;

  if(!m_spEngineObj) return false;
  CEngineNode* pNode = DYNAMIC_CAST(CEngineNode,m_spEngineObj);
  if(!pNode) return false;

  pNode->SetPosition(m_v3Pos);
  pNode->SetRotation(m_qRot);
  pNode->SetScale   (m_v3Scale);

  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITING                                                             //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- SpecialEditing -----------------------------------------------------//
// called when a property needs a special dlg
// in : property,new value,document
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorNode::SpecialEditing(CProperty* pProp,CStdString* pstrNew,CRTTIPropsDoc* pDoc)
  {
  if(!pProp || !pstrNew || !pDoc) return CEditorObj::SpecialEditing(pProp,pstrNew,pDoc);
  CStdString strRes = pProp->GetValue(m_spEngineObj);

  if(IsPropOfRTTI(pProp,&CEngineNode::m_RTTI))
    {
    if(pProp->GetName() == "Rotation")
      {
      CEngineNode* pNode = DYNAMIC_CAST(CEngineNode,m_spEngineObj);
      if(!pNode) return false;
      CQuaternion qRot(pNode->GetRotation());

      CDlgRotation dlg;
      if(dlg.DoModal() != IDOK) return false;

      pNode->SetRotation(dlg.m_qRot);
      *pstrNew = pProp->GetValue(m_spEngineObj);
      return true;
      }
    }

  return CEditorObj::SpecialEditing(pProp,pstrNew,pDoc);
  }

//----------------------------------------------- SpecialGetValue ----------------------------------------------------//
// called when a property needs special displaying in the grid
// in : property,new value
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

CStdString CEditorNode::SpecialGetValue(CProperty* pProp)
  {
  if(!pProp) return CEditorObj::SpecialGetValue(pProp);
  CStdString strRes = pProp->GetValue(m_spEngineObj);

  return strRes;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
