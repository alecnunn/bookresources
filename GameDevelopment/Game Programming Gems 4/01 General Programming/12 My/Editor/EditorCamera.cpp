//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR CAMERA CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "EditorCamera.h"
#ifdef    _DEBUG
#include  "EditorCamera.inl"
#endif

#include  "EngineCamera.h"
#include  "DlgEditCamera.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI(CEditorCamera,CEditorNode);

//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE                                                              //
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorCamera::CreateEngineObj()
  {
  CEngineObj* pEngineObj = CEngineObj::CreateObject(&CEngineCamera::m_RTTI);
  if(!pEngineObj) return false;

  pEngineObj->SetName(m_strName);
  pEngineObj->SetEditorObj(this);
  SetEngineObj(pEngineObj);
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                EDIT                                                                //
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorCamera::Edit(CRTTIPropsDoc* pDoc,IRenderer* pRenderer)
  {
  CEngineCamera* pCamera = DYNAMIC_CAST(CEngineCamera,m_spEngineObj);
  if(!pCamera || !pRenderer) return false;

  // edit initial values

  CDlgEditCamera dlgEdit;
  if(dlgEdit.DoModal() != IDOK) return false;

  CVect3D v3Pos(dlgEdit.m_fPosX,dlgEdit.m_fPosY,dlgEdit.m_fPosZ);
  float fCamDist = 10.f;

  switch(dlgEdit.m_iDir)
    {
    case 0 : pCamera->SetRepere(v3Pos,v3Pos+CVect3D(+fCamDist,0.f,0.f),CVect3D(0.f,+1.f,0.f)); break;         // left
    case 1 : pCamera->SetRepere(v3Pos,v3Pos+CVect3D(-fCamDist,0.f,0.f),CVect3D(0.f,+1.f,0.f)); break;         // right
    case 2 : pCamera->SetRepere(v3Pos,v3Pos+CVect3D(0.f,-fCamDist,0.f),CVect3D(0.f,0.f,-1.f)); break;         // top
    case 3 : pCamera->SetRepere(v3Pos,v3Pos+CVect3D(0.f,+fCamDist,0.f),CVect3D(0.f,0.f,+1.f)); break;         // bottom
    case 4 : pCamera->SetRepere(v3Pos,v3Pos+CVect3D(0.f,0.f,-fCamDist),CVect3D(0.f,+1.f,0.f)); break;         // front
    case 5 : pCamera->SetRepere(v3Pos,v3Pos+CVect3D(0.f,0.f,+fCamDist),CVect3D(0.f,+1.f,0.f)); break;         // back
    }

  pCamera->SetFov (DEG2RAD(dlgEdit.m_fFov));
  pCamera->SetNear(dlgEdit.m_fNear);
  pCamera->SetFar (dlgEdit.m_fFar);
  return true;
  }

//

bool CEditorCamera::EditExisting(CRTTIPropsDoc* pDoc,IRenderer* pRenderer)
  {
  CEngineCamera* pCamera = DYNAMIC_CAST(CEngineCamera,m_spEngineObj);
  if(!pCamera || !pRenderer) return false;

  // edit initial values

  CVect3D v3OldPos(pCamera->GetPosition());

  CDlgEditCamera dlgEdit;
  dlgEdit.m_boInitFromStatic = false;
  dlgEdit.m_fPosX = v3OldPos.x;
  dlgEdit.m_fPosY = v3OldPos.y;
  dlgEdit.m_fPosZ = v3OldPos.z;
  dlgEdit.m_iDir  = -1; // TBI: could be improved to detect predefined dirs
  dlgEdit.m_fFov  = RAD2DEG(pCamera->GetFov());
  dlgEdit.m_fNear = pCamera->GetNear();
  dlgEdit.m_fFar  = pCamera->GetFar();

  if(dlgEdit.DoModal() != IDOK) return false;

  CVect3D v3Pos(dlgEdit.m_fPosX,dlgEdit.m_fPosY,dlgEdit.m_fPosZ);
  float fCamDist = 10.f;

  switch(dlgEdit.m_iDir)
    {
    case 0 : pCamera->SetRepere(v3Pos,v3Pos+CVect3D(+fCamDist,0.f,0.f),CVect3D(0.f,+1.f,0.f)); break;         // left
    case 1 : pCamera->SetRepere(v3Pos,v3Pos+CVect3D(-fCamDist,0.f,0.f),CVect3D(0.f,+1.f,0.f)); break;         // right
    case 2 : pCamera->SetRepere(v3Pos,v3Pos+CVect3D(0.f,-fCamDist,0.f),CVect3D(0.f,0.f,-1.f)); break;         // top
    case 3 : pCamera->SetRepere(v3Pos,v3Pos+CVect3D(0.f,+fCamDist,0.f),CVect3D(0.f,0.f,+1.f)); break;         // bottom
    case 4 : pCamera->SetRepere(v3Pos,v3Pos+CVect3D(0.f,0.f,-fCamDist),CVect3D(0.f,+1.f,0.f)); break;         // front
    case 5 : pCamera->SetRepere(v3Pos,v3Pos+CVect3D(0.f,0.f,+fCamDist),CVect3D(0.f,+1.f,0.f)); break;         // back
    default:
      {
      CVect3D v3Delta(pCamera->GetTarget()-pCamera->GetPosition());
      CVect3D v3Up   (pCamera->GetUp());
      pCamera->SetRepere(v3Pos,v3Pos+v3Delta,v3Up);
      break;
      }
    }

  pCamera->SetFov (DEG2RAD(dlgEdit.m_fFov));
  pCamera->SetNear(dlgEdit.m_fNear);
  pCamera->SetFar (dlgEdit.m_fFar);
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                STATE                                                               //
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorCamera::Store()
  {
  if(!CEditorObj::Store()) return false;

  m_boStored = false;
  if(!m_spEngineObj) return false;
  CEngineCamera* pCam = DYNAMIC_CAST(CEngineCamera,m_spEngineObj);
  if(!pCam) return false;

  m_v3Pos    = pCam->GetPosition();
  m_v3Target = pCam->GetTarget();
  m_v3Up     = pCam->GetUp();

  m_fScaleOrtho = pCam->GetScaleOrtho();

  m_boStored = true;
  return true;
  }

//

bool CEditorCamera::Reset()
  {
  if(!CEditorObj::Reset()) return false;

  if(!m_spEngineObj) return false;
  CEngineCamera* pCam = DYNAMIC_CAST(CEngineCamera,m_spEngineObj);
  if(!pCam) return false;

  pCam->SetRepere(m_v3Pos,m_v3Target,m_v3Up);
  pCam->SetScaleOrtho(m_fScaleOrtho);
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
