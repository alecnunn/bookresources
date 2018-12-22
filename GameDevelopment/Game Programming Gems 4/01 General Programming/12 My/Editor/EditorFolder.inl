//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR FOLDER CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_EDITORFOLDER_INL_
#define   _GPG4_EDITORFOLDER_INL_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "ImgMenu.h"

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CEditorFolder::CEditorFolder()
  {}

INLINE CEditorFolder::~CEditorFolder()
  {}

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET/SET                                                             //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CStdString CEditorFolder::GetDefaultName() const
  {
  return "folder";
  }

INLINE CStdString CEditorFolder::GetEditName() const
  {
  return "Folder";
  }

INLINE u32 CEditorFolder::GetIcon() const
  {
  return CImgMenu::GetImgID("_IMG_FOLDER_");
  }

INLINE u32 CEditorFolder::GetMenu() const
  {
  return CImgMenu::GetMenuID("_MENU_FOLDER_");
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_EDITORFOLDER_INL_
