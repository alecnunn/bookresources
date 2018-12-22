//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR OBJECT CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_EDITOROBJ_INL_
#define   _GPG4_EDITOROBJ_INL_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "ImgMenu.h"

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CEditorObj::CEditorObj() : m_boSelected(false)
  {
  m_boDeletable  = false;//true;
  m_boDraggable  = true;
  m_boDropTarget = true;
  m_boRenamable  = true;

  m_boStored     = false;
  }

INLINE CEditorObj::~CEditorObj()
  {}

//

INLINE bool CEditorObj::CreateEngineObj()
  {
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITING                                                             //
//--------------------------------------------------------------------------------------------------------------------//

INLINE bool CEditorObj::Store()
  {
  m_boStored = true;
  return true;
  }

INLINE bool CEditorObj::Reset()
  {
  if(!m_boStored) return false;
  return true;
  }

INLINE bool CEditorObj::Edit(CRTTIPropsDoc* pDoc,IRenderer* pRenderer)
  {
  return true;
  }

INLINE bool CEditorObj::EditExisting(CRTTIPropsDoc* pDoc,IRenderer* pRenderer)
  {
  return true;
  }

INLINE bool CEditorObj::SpecialEditing(CProperty* pProp,CStdString* pstrNew,CRTTIPropsDoc* pDoc)
  {
  return false;
  }

INLINE CStdString CEditorObj::SpecialGetValue(CProperty* pProp)
  {
  return "! NOT IMPLEMENTED !";
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET/SET                                                             //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CStdString CEditorObj::GetEditName() const
  {
  return "Editing";
  }

INLINE u32 CEditorObj::GetIcon() const
  {
  return CImgMenu::GetImgID("_IMG_EMPTY_");
  }

INLINE u32 CEditorObj::GetMenu() const
  {
  return CImgMenu::GetMenuID("_MENU_NONE_");
  }

INLINE CEngineObj* CEditorObj::GetEngineObj() const
  {
  return m_spEngineObj;
  }

INLINE void CEditorObj::SetEngineObj(CEngineObj* pEngineObj)
  {
  m_spEngineObj = pEngineObj;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                FLAGS                                                               //
//--------------------------------------------------------------------------------------------------------------------//

INLINE bool CEditorObj::IsSelected() const
  {
  return m_boSelected;
  }

INLINE bool CEditorObj::IsDeletable() const
  {
  return m_boDeletable;
  }

INLINE bool CEditorObj::IsDraggable() const
  {
  return m_boDraggable;
  }

INLINE bool CEditorObj::IsDropTarget() const
  {
  return m_boDropTarget;
  }

INLINE bool CEditorObj::IsRenamable() const
  {
  return m_boRenamable;
  }

//

INLINE void CEditorObj::Select(const bool boSelect)
  {
  m_boSelected = boSelect;
  }

INLINE void CEditorObj::SetDeleteFlag(const bool boDelete)
  {
  m_boDeletable = boDelete;
  }

INLINE void CEditorObj::SetDragFlag(const bool boDrag)
  {
  m_boDraggable = boDrag;
  }

INLINE void CEditorObj::SetDropFlag(const bool boDrop)
  {
  m_boDropTarget = boDrop;
  }

INLINE void CEditorObj::SetRenameFlag(const bool boRename)
  {
  m_boRenamable = boRename;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_EDITOROBJ_INL_
