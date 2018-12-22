//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR SPHERE CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "EditorSphere.h"
#ifdef    _DEBUG
#include  "EditorSphere.inl"
#endif

#include  "EngineSphere.h"
#include  "DlgEditSphere.h"
#include  "DlgChooseMaterial.h"
#include  "Properties.h"
#include  "..\RTTIPropsDoc.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI(CEditorSphere,CEditorNode);

//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE                                                              //
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorSphere::CreateEngineObj()
  {
  CEngineObj* pEngineObj = CEngineObj::CreateObject(&CEngineSphere::m_RTTI);
  if(!pEngineObj) return false;

  pEngineObj->SetName(m_strName);
  pEngineObj->SetEditorObj(this);
  SetEngineObj(pEngineObj);
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITING                                                             //
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorSphere::Edit(CRTTIPropsDoc* pDoc,IRenderer* pRenderer)
  {
  CEngineSphere* pSphere = DYNAMIC_CAST(CEngineSphere,m_spEngineObj);
  if(!pSphere || !pRenderer) return false;

  // edit initial values

  CDlgEditSphere dlgEdit;
  dlgEdit.m_pItem = pDoc->GetEditorScene()->GetRootItem();
  if(dlgEdit.DoModal() != IDOK) return false;

  CVect3D v3Pos(dlgEdit.m_fPosX,dlgEdit.m_fPosY,dlgEdit.m_fPosZ);
  pSphere->SetPosition(v3Pos);

  // create mesh

  if(!pSphere->Create(pRenderer,dlgEdit.m_fRadius,dlgEdit.m_uiLOD)) return false;

  if(dlgEdit.m_boNewMat)
    {
    CEngineMaterial* pMat = new CEngineMaterial;
    if(pMat)
      {
      float fR = (float)rand()/RAND_MAX; fR = fR*0.5f+0.5f;
      float fG = (float)rand()/RAND_MAX; fG = fG*0.5f+0.5f;
      float fB = (float)rand()/RAND_MAX; fB = fB*0.5f+0.5f;
      pMat   ->SetDiffuse(CVect4D(fR,fG,fB,1.f));
      pSphere->SetMaterial(pMat);
      }
    }

  else pSphere->SetMaterial(dlgEdit.m_pMat);
  return true;
  }

//

bool CEditorSphere::EditExisting(CRTTIPropsDoc* pDoc,IRenderer* pRenderer)
  {
  CEngineSphere* pSphere = DYNAMIC_CAST(CEngineSphere,m_spEngineObj);
  if(!pSphere || !pRenderer) return false;

  // edit initial values

  CVect3D v3OldPos    (pSphere->GetPosition());
  float   fOldRadius = pSphere->GetRadius();
  UINT    uiOldLOD   = pSphere->GetSectionPts();

  CDlgEditSphere dlgEdit;
  dlgEdit.m_boInitFromStatic = false;
  dlgEdit.m_fPosX = v3OldPos.x;
  dlgEdit.m_fPosY = v3OldPos.y;
  dlgEdit.m_fPosZ = v3OldPos.z;
  dlgEdit.m_fRadius = fOldRadius;
  dlgEdit.m_uiLOD   = uiOldLOD;
  dlgEdit.m_pMat  = pSphere->GetMaterial();
  dlgEdit.m_pItem = pDoc->GetEditorScene()->GetRootItem();

  if(dlgEdit.DoModal() != IDOK) return false;

  CVect3D v3Pos(dlgEdit.m_fPosX,dlgEdit.m_fPosY,dlgEdit.m_fPosZ);
  pSphere->SetPosition(v3Pos);

  // create mesh

  if((dlgEdit.m_fRadius != fOldRadius) || (dlgEdit.m_uiLOD != uiOldLOD))
    {
    CEngineMaterialSP spMat = pSphere->GetMaterial();
    if(!pSphere->Create(pRenderer,dlgEdit.m_fRadius,dlgEdit.m_uiLOD)) return false;
    pSphere->SetMaterial(spMat);
    }

  if(dlgEdit.m_boNewMat)
    {
    CEngineMaterial* pMat = new CEngineMaterial;
    if(pMat)
      {
      float fR = (float)rand()/RAND_MAX; fR = fR*0.5f+0.5f;
      float fG = (float)rand()/RAND_MAX; fG = fG*0.5f+0.5f;
      float fB = (float)rand()/RAND_MAX; fB = fB*0.5f+0.5f;
      pMat   ->SetDiffuse(CVect4D(fR,fG,fB,1.f));
      pSphere->SetMaterial(pMat);
      }
    }

  else pSphere->SetMaterial(dlgEdit.m_pMat);
  return true;
  }

//----------------------------------------------- SpecialEditing -----------------------------------------------------//
// called when a property needs a special dlg
// in : property,new value,document
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorSphere::SpecialEditing(CProperty* pProp,CStdString* pstrNew,CRTTIPropsDoc* pDoc)
  {
  if(!pProp || !pstrNew || !pDoc) return CEditorNode::SpecialEditing(pProp,pstrNew,pDoc);
  CStdString strRes = pProp->GetValue(m_spEngineObj);

  if(IsPropOfRTTI(pProp,&CEngineSphere::m_RTTI))
    {
    if(pProp->GetName() == "Material")
      {
      CEngineSphere* pSphere = DYNAMIC_CAST(CEngineSphere,m_spEngineObj);
      if(!pSphere) return false;
      CEngineMaterial* pMat = pSphere->GetMaterial();

      CDlgChooseMaterial dlg;
      dlg.m_pSceneItem = pDoc->GetEditorScene()->GetRootItem();
      dlg.m_pSelected  = pMat;
      if(dlg.DoModal() != IDOK) return false;               // cancelled

      if(dlg.m_pSelected == pMat) return false;             // unmodified
      pSphere->SetMaterial(dlg.m_pSelected);
      //*pstrNew = pProp->GetValue(m_spEngineObj);
      return true;
      }
    }

  return CEditorNode::SpecialEditing(pProp,pstrNew,pDoc);
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
