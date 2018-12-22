//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR OBJECT CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "EditorObj.h"
#ifdef    _DEBUG
#include  "EditorObj.inl"
#endif

#include  "EditorObjects.h"
#include  "MenuEnum.h"
#include  "Properties.h"
#include  "RTTIMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI_PROP (CEditorObj,CPersistent);
IMPLEMENT_PROPERTIES(CEditorObj,CExtraProp)

//--------------------------------------------------------------------------------------------------------------------//
//                                                PROPERTIES                                                          //
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorObj::DefineProperties()                         // static
  {
  REGISTER_PROP(Bool,CEditorObj,m_boSelected,  "Selected",  CProperty::EXPOSE|CProperty::STREAM,"")
  REGISTER_PROP(Bool,CEditorObj,m_boDeletable, "Deletable", CProperty::EXPOSE|CProperty::STREAM|CProperty::READONLY,"")
  REGISTER_PROP(Bool,CEditorObj,m_boDraggable, "Draggable", CProperty::EXPOSE|CProperty::STREAM|CProperty::READONLY,"")
  REGISTER_PROP(Bool,CEditorObj,m_boDropTarget,"DropTarget",CProperty::EXPOSE|CProperty::STREAM|CProperty::READONLY,"")
  REGISTER_PROP(Bool,CEditorObj,m_boRenamable, "Renamable", CProperty::EXPOSE|CProperty::STREAM|CProperty::READONLY,"")
  REGISTER_PROP(SP,  CEditorObj,m_spEngineObj, "EngineObj",                   CProperty::STREAM,"")
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                FACTORY                                                             //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- CreateObject -------------------------------------------------------//
// create a derived object
// in : object menu ID
// out: object or NULL
//--------------------------------------------------------------------------------------------------------------------//

CEditorObj* CEditorObj::CreateObject(const u32 u32ObjType)
  {
  CEditorObj* pObj = NULL;

  switch(u32ObjType)
    {
    case _MENU_BOX_         : pObj = new CEditorBox;        break;
    case _MENU_CAMERA_      : pObj = new CEditorCamera;     break;
    case _MENU_FOLDER_      : pObj = new CEditorFolder;     break;
    case _MENU_MATERIAL_    : pObj = new CEditorMaterial;   break;
    case _MENU_NODE_        : pObj = new CEditorNode;       break;
    case _MENU_SPHERE_      : pObj = new CEditorSphere;     break;
    case _MENU_TEXTURE_     : pObj = new CEditorTexture;    break;
    }

  return pObj;
  }

//----------------------------------------------- CreateObject -------------------------------------------------------//
// create an object from its class name
// in : class name
// out: object or NULL
// rem: could be optimized with a map or something
//--------------------------------------------------------------------------------------------------------------------//

CEditorObj* CEditorObj::CreateObject(const CStdString& strClassName)
  {
  if(strClassName == CEditorBox       ::m_RTTI.GetClassName()) return CreateObject(_MENU_BOX_);
  if(strClassName == CEditorCamera    ::m_RTTI.GetClassName()) return CreateObject(_MENU_CAMERA_);
  if(strClassName == CEditorFolder    ::m_RTTI.GetClassName()) return CreateObject(_MENU_FOLDER_);
  if(strClassName == CEditorMaterial  ::m_RTTI.GetClassName()) return CreateObject(_MENU_MATERIAL_);
  if(strClassName == CEditorNode      ::m_RTTI.GetClassName()) return CreateObject(_MENU_NODE_);
  if(strClassName == CEditorSphere    ::m_RTTI.GetClassName()) return CreateObject(_MENU_SPHERE_);
  if(strClassName == CEditorTexture   ::m_RTTI.GetClassName()) return CreateObject(_MENU_TEXTURE_);
  return NULL;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                RTTI MAP                                                            //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- GetRTTIMap ---------------------------------------------------------//
// fill & return singleton map
// in :
// out: map[string->RTTI*]
//--------------------------------------------------------------------------------------------------------------------//

CRTTIMap* CEditorObj::GetRTTIMap()
  {
  static CRTTIMap map;
  static bool boFill = FillRTTIMap(map);
  return &map;
  }

//----------------------------------------------- FillRTTIMap --------------------------------------------------------//
// fill map
// in : map
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorObj::FillRTTIMap(CRTTIMap& map)
  {
  CEngineObj::FillRTTIMap(map);

  const CRTTI* pRTTI;
  pRTTI = &CEditorBox       ::m_RTTI;   map.AddRTTI(pRTTI->GetClassName(),pRTTI);
  pRTTI = &CEditorCamera    ::m_RTTI;   map.AddRTTI(pRTTI->GetClassName(),pRTTI);
  pRTTI = &CEditorFolder    ::m_RTTI;   map.AddRTTI(pRTTI->GetClassName(),pRTTI);
  pRTTI = &CEditorMaterial  ::m_RTTI;   map.AddRTTI(pRTTI->GetClassName(),pRTTI);
  pRTTI = &CEditorNode      ::m_RTTI;   map.AddRTTI(pRTTI->GetClassName(),pRTTI);
  pRTTI = &CEditorObj       ::m_RTTI;   map.AddRTTI(pRTTI->GetClassName(),pRTTI);
  pRTTI = &CEditorScene     ::m_RTTI;   map.AddRTTI(pRTTI->GetClassName(),pRTTI);
  pRTTI = &CEditorSceneItem ::m_RTTI;   map.AddRTTI(pRTTI->GetClassName(),pRTTI);
  pRTTI = &CEditorSphere    ::m_RTTI;   map.AddRTTI(pRTTI->GetClassName(),pRTTI);
  pRTTI = &CEditorTexture   ::m_RTTI;   map.AddRTTI(pRTTI->GetClassName(),pRTTI);
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
 
} // namespace
