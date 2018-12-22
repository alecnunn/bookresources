//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR MATERIAL CLASS                                               //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "EditorMaterial.h"
#ifdef    _DEBUG
#include  "EditorMaterial.inl"
#endif

#include  "EngineMaterial.h"
#include  "Properties.h"
#include  "DlgChooseTexture.h"
#include  "..\RTTIPropsDoc.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI(CEditorMaterial,CEditorObj);

//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE                                                              //
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorMaterial::CreateEngineObj()
  {
  CEngineObj* pEngineObj = CEngineObj::CreateObject(&CEngineMaterial::m_RTTI);
  if(!pEngineObj) return false;

  pEngineObj->SetName(m_strName);
  pEngineObj->SetEditorObj(this);
  SetEngineObj(pEngineObj);
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

bool CEditorMaterial::SpecialEditing(CProperty* pProp,CStdString* pstrNew,CRTTIPropsDoc* pDoc)
  {
  if(!pProp || !pstrNew || !pDoc) return CEditorObj::SpecialEditing(pProp,pstrNew,pDoc);
  CStdString strRes = pProp->GetValue(m_spEngineObj);

  if(IsPropOfRTTI(pProp,&CEngineMaterial::m_RTTI))
    {
    if(pProp->GetName().Left(3) == "Tex")
      {
      // slot

      u32 u32Slot;
      if(sscanf(pProp->GetName(),"Tex%u",&u32Slot) != 1) return false;
      if(u32Slot >= CEngineMaterial::NbSlots) return false;

      // texture

      CEngineMaterial* pMat = DYNAMIC_CAST(CEngineMaterial,m_spEngineObj);
      if(!pMat) return false;
      CEngineTexture*  pTex = pMat->GetTexture(u32Slot);

      CDlgChooseTexture dlg;
      dlg.m_pSceneItem = pDoc->GetEditorScene()->GetRootItem();
      dlg.m_pSelected  = pTex;
      if(dlg.DoModal() != IDOK) return false;               // cancelled

      if(dlg.m_pSelected == pTex) return false;             // unmodified
      pMat->SetTexture(dlg.m_pSelected,u32Slot);
      *pstrNew = pProp->GetValue(m_spEngineObj);

    // temp
      if(!dlg.m_pSelected) pMat->SetTexOp(CEngineMaterial::NOP,     u32Slot);
      else                 pMat->SetTexOp(CEngineMaterial::MODULATE,u32Slot);
    //
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

CStdString CEditorMaterial::SpecialGetValue(CProperty* pProp)
  {
  if(!pProp) return CEditorObj::SpecialGetValue(pProp);
  CStdString strRes = pProp->GetValue(m_spEngineObj);

  if(IsPropOfRTTI(pProp,&CEngineMaterial::m_RTTI))
    {
    if(pProp->GetName().Left(3) == "Tex")
      {
      CPropSP* pPropSP = DYNAMIC_CAST(CPropSP,pProp);
      if(!pPropSP) return strRes;
      void* pVoid;
      if(!pPropSP->String2Ptr(strRes,&pVoid)) return strRes;

      CEngineTexture* pTex = STATIC_CAST(CEngineTexture,pVoid);
      if(!pTex) return "NO TEXTURE DEFINED";
      if(pTex->GetName().empty()) return strRes;
      return pTex->GetName();
      }
    }

  return strRes;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
