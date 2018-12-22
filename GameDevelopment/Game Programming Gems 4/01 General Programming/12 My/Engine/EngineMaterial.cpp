//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE MATERIAL CLASS                                               //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "EngineMaterial.h"
#ifdef    _DEBUG
#include  "EngineMaterial.inl"
#endif

#include  "Properties.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI_PROP (CEngineMaterial,CEngineObj);
IMPLEMENT_PROPERTIES(CEngineMaterial,CExtraProp);

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

CEngineMaterial::CEngineMaterial()
  {
  m_v4Diffuse.Set(1.f);
  m_boLighted = true;
  m_boZTest   = true;
  m_boCulling = true;

  for(u32 u32Slot = 0; u32Slot < NbSlots; u32Slot++)
    {
    m_TexOp[u32Slot] = NOP;
    }
  }

//

CEngineMaterial::~CEngineMaterial()
  {}

//--------------------------------------------------------------------------------------------------------------------//
//                                                PROPERTIES                                                          //
//--------------------------------------------------------------------------------------------------------------------//

bool CEngineMaterial::DefineProperties()                    // static
  {
  CProperty* pProp = REGISTER_PROP(Vect4D,CEngineMaterial,m_v4Diffuse,"Diffuse",CProperty::EXPOSE|CProperty::STREAM,"RGBA in [0;1]");
  pProp->SetCtrlType(CProperty::COLOR);

  REGISTER_PROP(Bool,CEngineMaterial,m_boLighted,"Lighted",CProperty::EXPOSE|CProperty::STREAM,"");
  REGISTER_PROP(Bool,CEngineMaterial,m_boZTest,  "Z Test", CProperty::EXPOSE|CProperty::STREAM,"");
  REGISTER_PROP(Bool,CEngineMaterial,m_boCulling,"Culling",CProperty::EXPOSE|CProperty::STREAM,"");

  CStdString strName;
  for(u32 u32Slot = 0; u32Slot < NbSlots; u32Slot++)
    {
    strName.Format("Tex%u",u32Slot);
    pProp = REGISTER_PROP(SP,CEngineMaterial,m_spTex[u32Slot],strName.c_str(),CProperty::EXPOSE|CProperty::STREAM,"");
    pProp->SetCtrlType(CProperty::BUTTON);                  // special editing
    }

  for(u32Slot = 0; u32Slot < NbSlots; u32Slot++)
    {
    strName.Format("Op%u",u32Slot);
    pProp = REGISTER_PROP(U32,CEngineMaterial,m_TexOp[u32Slot],strName.c_str(),CProperty::STREAM,"");
    pProp->SetCtrlType(CProperty::BUTTON);                  // special editing
    }

  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
