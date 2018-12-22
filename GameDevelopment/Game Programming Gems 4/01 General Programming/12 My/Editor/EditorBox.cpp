//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR BOX CLASS                                                    //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "EditorBox.h"
#ifdef    _DEBUG
#include  "EditorBox.inl"
#endif

#include  "EngineBox.h"
#include  "DlgEditBox.h"
#include  "DlgChooseMaterial.h"
#include  "Properties.h"
#include  "..\RTTIPropsDoc.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI(CEditorBox,CEditorNode);

//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE                                                              //
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorBox::CreateEngineObj()
  {
  CEngineObj* pEngineObj = CEngineObj::CreateObject(&CEngineBox::m_RTTI);
  if(!pEngineObj) return false;

  pEngineObj->SetName(m_strName);
  pEngineObj->SetEditorObj(this);
  SetEngineObj(pEngineObj);
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITING                                                             //
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorBox::Edit(CRTTIPropsDoc* pDoc,IRenderer* pRenderer)
  {
  CEngineBox* pBox = DYNAMIC_CAST(CEngineBox,m_spEngineObj);
  if(!pBox || !pRenderer) return false;

  // edit initial values

  CDlgEditBox dlgEdit;
  dlgEdit.m_pItem = pDoc->GetEditorScene()->GetRootItem();
  if(dlgEdit.DoModal() != IDOK) return false;

  CVect3D v3Dim(dlgEdit.m_fDimX,dlgEdit.m_fDimY,dlgEdit.m_fDimZ);
  CVect3D v3Pos(dlgEdit.m_fPosX,dlgEdit.m_fPosY,dlgEdit.m_fPosZ);

  switch(dlgEdit.m_iPosX)
    {
    case 0 : v3Pos.x += v3Dim.x*0.5f; break;
    case 2 : v3Pos.x -= v3Dim.x*0.5f; break;
    }
  switch(dlgEdit.m_iPosY)
    {
    case 0 : v3Pos.y += v3Dim.y*0.5f; break;
    case 2 : v3Pos.y -= v3Dim.y*0.5f; break;
    }
  switch(dlgEdit.m_iPosZ)
    {
    case 0 : v3Pos.z += v3Dim.z*0.5f; break;
    case 2 : v3Pos.z -= v3Dim.z*0.5f; break;
    }

  pBox->SetPosition(v3Pos);

  // create mesh

  if(!pBox->Create(pRenderer,v3Dim)) return false;

  if(dlgEdit.m_boNewMat)
    {
    CEngineMaterial* pMat = new CEngineMaterial;
    if(pMat)
      {
      float fR = (float)rand()/RAND_MAX; fR = fR*0.5f+0.5f;
      float fG = (float)rand()/RAND_MAX; fG = fG*0.5f+0.5f;
      float fB = (float)rand()/RAND_MAX; fB = fB*0.5f+0.5f;
      pMat->SetDiffuse(CVect4D(fR,fG,fB,1.f));
      pBox->SetMaterial(pMat);
      }
    }

  else pBox->SetMaterial(dlgEdit.m_pMat);
  return true;
  }

//

bool CEditorBox::EditExisting(CRTTIPropsDoc* pDoc,IRenderer* pRenderer)
  {
  CEngineBox* pBox = DYNAMIC_CAST(CEngineBox,m_spEngineObj);
  if(!pBox || !pRenderer) return false;

  // edit initial values

  CVect3D v3OldDim(pBox->GetExtent());
  CVect3D v3OldPos(pBox->GetPosition());

  CDlgEditBox dlgEdit;
  dlgEdit.m_boInitFromStatic = false;
  dlgEdit.m_fDimX = v3OldDim.x;
  dlgEdit.m_fDimY = v3OldDim.y;
  dlgEdit.m_fDimZ = v3OldDim.z;
  dlgEdit.m_fPosX = v3OldPos.x;
  dlgEdit.m_fPosY = v3OldPos.y;
  dlgEdit.m_fPosZ = v3OldPos.z;
  dlgEdit.m_iPosX = dlgEdit.m_iPosY = dlgEdit.m_iPosZ = 1;
  dlgEdit.m_pMat  = pBox->GetMaterial();
  dlgEdit.m_pItem = pDoc->GetEditorScene()->GetRootItem();

  if(dlgEdit.DoModal() != IDOK) return false;

  CVect3D v3Dim(dlgEdit.m_fDimX,dlgEdit.m_fDimY,dlgEdit.m_fDimZ);
  CVect3D v3Pos(dlgEdit.m_fPosX,dlgEdit.m_fPosY,dlgEdit.m_fPosZ);

  switch(dlgEdit.m_iPosX)
    {
    case 0 : v3Pos.x += v3Dim.x*0.5f; break;
    case 2 : v3Pos.x -= v3Dim.x*0.5f; break;
    }
  switch(dlgEdit.m_iPosY)
    {
    case 0 : v3Pos.y += v3Dim.y*0.5f; break;
    case 2 : v3Pos.y -= v3Dim.y*0.5f; break;
    }
  switch(dlgEdit.m_iPosZ)
    {
    case 0 : v3Pos.z += v3Dim.z*0.5f; break;
    case 2 : v3Pos.z -= v3Dim.z*0.5f; break;
    }

  pBox->SetPosition(v3Pos);

  // create mesh

  if(v3Dim != v3OldDim)
    {
    CEngineMaterialSP spMat = pBox->GetMaterial();
    if(!pBox->Create(pRenderer,v3Dim)) return false;
    pBox->SetMaterial(spMat);
    }

  if(dlgEdit.m_boNewMat)
    {
    CEngineMaterial* pMat = new CEngineMaterial;
    if(pMat)
      {
      float fR = (float)rand()/RAND_MAX; fR = fR*0.5f+0.5f;
      float fG = (float)rand()/RAND_MAX; fG = fG*0.5f+0.5f;
      float fB = (float)rand()/RAND_MAX; fB = fB*0.5f+0.5f;
      pMat->SetDiffuse(CVect4D(fR,fG,fB,1.f));
      pBox->SetMaterial(pMat);
      }
    }

  else pBox->SetMaterial(dlgEdit.m_pMat);
  return true;
  }

//----------------------------------------------- SpecialEditing -----------------------------------------------------//
// called when a property needs a special dlg
// in : property,new value,document
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorBox::SpecialEditing(CProperty* pProp,CStdString* pstrNew,CRTTIPropsDoc* pDoc)
  {
  if(!pProp || !pstrNew || !pDoc) return CEditorNode::SpecialEditing(pProp,pstrNew,pDoc);
  CStdString strRes = pProp->GetValue(m_spEngineObj);

  if(IsPropOfRTTI(pProp,&CEngineBox::m_RTTI))
    {
    if(pProp->GetName() == "Material")
      {
      CEngineBox* pBox = DYNAMIC_CAST(CEngineBox,m_spEngineObj);
      if(!pBox) return false;
      CEngineMaterial* pMat = pBox->GetMaterial();

      CDlgChooseMaterial dlg;
      dlg.m_pSceneItem = pDoc->GetEditorScene()->GetRootItem();
      dlg.m_pSelected  = pMat;
      if(dlg.DoModal() != IDOK) return false;               // cancelled

      if(dlg.m_pSelected == pMat) return false;             // unmodified
      pBox->SetMaterial(dlg.m_pSelected);
      //*pstrNew = pProp->GetValue(m_spEngineObj);
      return true;
      }
    }

  return CEditorNode::SpecialEditing(pProp,pstrNew,pDoc);
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
