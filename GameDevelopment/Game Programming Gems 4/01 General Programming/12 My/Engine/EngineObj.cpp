//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE OBJECT CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "EngineObj.h"
#ifdef    _DEBUG
#include  "EngineObj.inl"
#endif

#include  "Engine.h"
#include  "EngineObjects.h"
#include  "Properties.h"
#include  "RTTIMap.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI_PROP (CEngineObj,CPersistent);
IMPLEMENT_PROPERTIES(CEngineObj,CExtraProp);

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

CEngineObj::~CEngineObj()
  {
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                PROPERTIES                                                          //
//--------------------------------------------------------------------------------------------------------------------//

bool CEngineObj::DefineProperties()                         // static
  {
  REGISTER_PROP(Bool,CEngineObj,m_boDeleted,"Deleted",CProperty::STREAM|CProperty::READONLY,"");
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                FACTORY                                                             //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- CreateObject -------------------------------------------------------//
// create a derived object
// in : class RTTI object
// out: object or NULL
//--------------------------------------------------------------------------------------------------------------------//

CEngineObj* CEngineObj::CreateObject(const CRTTI* pRTTI)
  {
  if(!pRTTI) return NULL;
  CEngineObj* pObj = NULL;

  if(pRTTI == &CEngineBox       ::m_RTTI) pObj = new CEngineBox;
  if(pRTTI == &CEngineCamera    ::m_RTTI) pObj = new CEngineCamera;
  if(pRTTI == &CEngineMaterial  ::m_RTTI) pObj = new CEngineMaterial;
  if(pRTTI == &CEngineMesh      ::m_RTTI) pObj = new CEngineMesh;
  if(pRTTI == &CEngineNode      ::m_RTTI) pObj = new CEngineNode;
  if(pRTTI == &CEngineSphere    ::m_RTTI) pObj = new CEngineSphere;

  if(pRTTI == &CEngineTexture   ::m_RTTI)
    {
    IRenderer* pRenderer = CEngine::GetEngine()->GetRenderer();
    if(pRenderer)   pObj = pRenderer->CreateTexture();
    else            pObj = new CEngineTexture;
    }

  return pObj;
  }

//----------------------------------------------- CreateObject -------------------------------------------------------//
// create an object from its class name
// in : class name
// out: object or NULL
// rem: could be optimized but not really needed
//--------------------------------------------------------------------------------------------------------------------//

CEngineObj* CEngineObj::CreateObject(const CStdString& strClassName)
  {
  CRTTIMap* pMap = GetRTTIMap();
  return CreateObject(pMap->GetRTTI(strClassName));
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                RTTI MAP                                                            //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- GetRTTIMap ---------------------------------------------------------//
// fill & return singleton map
// in :
// out: map[string->RTTI*]
//--------------------------------------------------------------------------------------------------------------------//

CRTTIMap* CEngineObj::GetRTTIMap()
  {
  static CRTTIMap map;
  static bool boFill = FillRTTIMap(map);
  return &map;
  }

//----------------------------------------------- FillRTTIMap --------------------------------------------------------//
// fill map
// in : map
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CEngineObj::FillRTTIMap(CRTTIMap& map)
  {
  const CRTTI* pRTTI;
  pRTTI = &CRefCount        ::m_RTTI;   map.AddRTTI(pRTTI->GetClassName(),pRTTI);
  pRTTI = &CPersistent      ::m_RTTI;   map.AddRTTI(pRTTI->GetClassName(),pRTTI);

  pRTTI = &CEngineBox       ::m_RTTI;   map.AddRTTI(pRTTI->GetClassName(),pRTTI);
  pRTTI = &CEngineCamera    ::m_RTTI;   map.AddRTTI(pRTTI->GetClassName(),pRTTI);
  pRTTI = &CEngineMaterial  ::m_RTTI;   map.AddRTTI(pRTTI->GetClassName(),pRTTI);
  pRTTI = &CEngineMesh      ::m_RTTI;   map.AddRTTI(pRTTI->GetClassName(),pRTTI);
  pRTTI = &CEngineNode      ::m_RTTI;   map.AddRTTI(pRTTI->GetClassName(),pRTTI);
  pRTTI = &CEngineObj       ::m_RTTI;   map.AddRTTI(pRTTI->GetClassName(),pRTTI);
  pRTTI = &CEngineSphere    ::m_RTTI;   map.AddRTTI(pRTTI->GetClassName(),pRTTI);
  pRTTI = &CEngineTexture   ::m_RTTI;   map.AddRTTI(pRTTI->GetClassName(),pRTTI);

  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
