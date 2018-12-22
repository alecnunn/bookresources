//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE CAMERA CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "EngineCamera.h"
#ifdef    _DEBUG
#include  "EngineCamera.inl"
#endif

#include  "Properties.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI_PROP (CEngineCamera,CEngineNode);
IMPLEMENT_PROPERTIES(CEngineCamera,CExtraProp);

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

CEngineCamera::CEngineCamera()
  {
  m_v3Pos   .Set(0.f,0.f,10.f);
  m_v3Target.Set(0.f);
  m_v3Up    .Set(0.f,1.f,0.f);
  SetInitialUp(m_v3Up);
  RecalcUp();

  m_fFov   = PI /3.f;
  m_fRatio = 4.f/3.f;
  m_fNear  = 1.f;
  m_fFar   = 1000.f;

  m_boPerspective  = true;
  m_fScaleOrtho    = 10.f;
  m_boRotations    = true;
  m_boPerspAllowed = true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                PROPERTIES                                                          //
//--------------------------------------------------------------------------------------------------------------------//

bool CEngineCamera::DefineProperties()                      // static
  {
  REGISTER_PROP(Vect3D,CEngineCamera,m_v3Target,       "Target",     CProperty::EXPOSE|CProperty::STREAM,"");
  REGISTER_PROP(Vect3D,CEngineCamera,m_v3Up,           "Up",         CProperty::EXPOSE|CProperty::STREAM,"");
  REGISTER_PROP(Vect3D,CEngineCamera,m_v3InitialUp,    "Initial Up", CProperty::EXPOSE|CProperty::STREAM,"");

  REGISTER_PROP(Float, CEngineCamera,m_fFov,           "FOV",        CProperty::EXPOSE|CProperty::STREAM,"Field of View");
  REGISTER_PROP(Float, CEngineCamera,m_fRatio,         "Ratio",      CProperty::EXPOSE|CProperty::STREAM,"");
  REGISTER_PROP(Float, CEngineCamera,m_fNear,          "Near",       CProperty::EXPOSE|CProperty::STREAM,"");
  REGISTER_PROP(Float, CEngineCamera,m_fFar,           "Far",        CProperty::EXPOSE|CProperty::STREAM,"");

  REGISTER_PROP(Bool,  CEngineCamera,m_boPerspective,  "Perspective",CProperty::EXPOSE|CProperty::STREAM,"");
  REGISTER_PROP(Float, CEngineCamera,m_fScaleOrtho,    "Scale Ortho",CProperty::EXPOSE|CProperty::STREAM,"");
  REGISTER_PROP(Bool,  CEngineCamera,m_boRotations,    "Rotations",                    CProperty::STREAM|CProperty::READONLY,"");
  REGISTER_PROP(Bool,  CEngineCamera,m_boPerspAllowed, "PerspAllow",                   CProperty::STREAM|CProperty::READONLY,"");
  return true;
  }

//----------------------------------------------- ModifyProp ---------------------------------------------------------//
// attempt to modify a property in the grid; do whatever is needed
// in : property, value
// out: property handled by the function ?
//--------------------------------------------------------------------------------------------------------------------//

bool CEngineCamera::ModifyProp(CProperty* pProp,const CStdString& strNew)
  {
  CVect3D      v3Val;
  bool         boConvert;
  CPropVect3D* pPropV3   = DYNAMIC_CAST(CPropVect3D,pProp);
  if(pPropV3)  boConvert = pPropV3->String2Floats(strNew,&v3Val[0]);

  if(pProp->GetName() == "Position")
    {
    if(!boConvert) return true;
    if(CanRotate()) SetRepere(v3Val,m_v3Target,m_v3Up);
    else            SetRepere(v3Val,m_v3Target+v3Val-m_v3Pos,m_v3Up); // target follows position's move
    return true;
    }

  if(!IsPropOfRTTI(pProp,&CEngineCamera::m_RTTI)) return CEngineNode::ModifyProp(pProp,strNew);

  if(pProp->GetName() == "Target")
    {
    if(!boConvert) return true;
    SetRepere(m_v3Pos,v3Val,m_v3Up);
    return true;
    }

  if(pProp->GetName() == "Up")
    {
    if(!boConvert) return true;
    SetRepere(m_v3Pos,m_v3Target,v3Val);
    return true;
    }

  return false;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITING                                                             //
//--------------------------------------------------------------------------------------------------------------------//

bool CEngineCamera::IsPropExposed(CProperty* pProp)
  {
  return CEngineNode::IsPropExposed(pProp);
  }

bool CEngineCamera::IsPropReadOnly(CProperty* pProp)
  {
  if(!CanRotate())
    {
    CStdString strName = pProp->GetName();
    if((strName == "Target") || (strName == "Up") || (strName == "Initial Up")) return true;
    }
  return CEngineNode::IsPropReadOnly(pProp);
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                TRFS GET/SET                                                        //
//--------------------------------------------------------------------------------------------------------------------//

bool CEngineCamera::SetPosition(const CVect3D& v3Pos)
  {
  return CEngineNode::SetPosition(v3Pos);
  }

//

bool CEngineCamera::SetRotation(const CQuaternion& qRot)
  {
  float fTargetDist = (m_v3Target-m_v3Pos).Length();
  CEngineNode::SetRotation(qRot);
  UpdateLocalTrf();

  m_v3Up     =  CVect3D(m_m4Local[UP], W);
  m_v3Target = -CVect3D(m_m4Local[DIR],W)*fTargetDist+m_v3Pos;
  return true;
  }

//

bool CEngineCamera::SetTarget(const CVect3D& v3Target)
  {
  m_v3Target = v3Target;
  return RecalcUp();
  }

//

bool CEngineCamera::SetUp(const CVect3D& v3Up)
  {
  m_v3Up = v3Up;
  return RecalcUp();
  }

//

bool CEngineCamera::SetRepere(const CVect3D& v3Pos,const CVect3D& v3Target,const CVect3D& v3Up)
  {
  SetWorldDirty();
  m_v3Pos    = v3Pos;
  m_v3Target = v3Target;
  m_v3Up     = v3Up;
  return RecalcUp();
  }

//

bool CEngineCamera::RecalcUp()
  {
  CVect3D v3Dir(m_v3Target-m_v3Pos);
  CVect3D v3X  (v3Dir^m_v3Up);
  m_v3Up.Set   (v3X  ^v3Dir);

  if(!m_v3Up.Normalize1()) return false;
  return RecalcRotation();
  }

//

bool CEngineCamera::RecalcRotation()
  {
  CVect3D v3Dir(m_v3Target-m_v3Pos);
  CVect3D v3X  (v3Dir^m_v3Up);
  CMat3x3 m3Rot(v3X.Normalized(),m_v3Up.Normalized(),-v3Dir.Normalized());

  return CEngineNode::SetRotation(m3Rot);
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
